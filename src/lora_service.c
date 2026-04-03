#include "sink_lora_forwarder/services/lora_service.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include "sink_lora_forwarder/logger_helper.h"

#define LORA_MAX_PAYLOAD    256
#define LORA_HEX_BUF        (LORA_MAX_PAYLOAD * 2 + 1)
#define LORA_CMD_BUF        (LORA_HEX_BUF + 32)
#define LORA_RSP_BUF        512
#define LORA_RSP_TIMEOUT_MS 3000

static int         g_lora_fd  = -1;
static const char *g_port     = NULL;
static int32_t     g_baud     = -1;
static uint8_t     g_fport    = 10;

void lora_service_set_port(const char *port) {
    g_port = port;
}

void lora_service_set_baudrate(int32_t baud) {
    g_baud = baud;
}

void lora_service_set_fport(uint8_t fport) {
    g_fport = fport;
}

/* Map a termios B-constant to speed_t; logs an error and returns B115200 for unknown values */
static speed_t baud_to_flag(int32_t baud)
{
    switch (baud) {
    case B9600:   return B9600;
    case B19200:  return B19200;
    case B38400:  return B38400;
    case B57600:  return B57600;
    case B115200: return B115200;
    default:
        zlog_error(error_cat, "LoRa: unsupported baud flag %d, defaulting to B115200", baud);
        return B115200;
    }
}

/* Open and configure a UART port using POSIX termios */
static int open_uart(const char *path, int32_t baud)
{
    const int fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) return -1;

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        return -1;
    }

    const speed_t speed = baud_to_flag(baud);
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    /* 8N1, raw mode, no flow control */
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |=  CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |=  CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        close(fd);
        return -1;
    }

    return fd;
}

lora_init_status lora_service_init(void)
{
    g_lora_fd = open_uart(g_port, g_baud);
    if (g_lora_fd < 0) {
        zlog_error(error_cat, "LoRa: failed to open UART %s", g_port ? g_port : "(null)");
        return LORA_INIT_ERROR;
    }
    zlog_info(ok_cat, "LoRa: opened %s fport=%u", g_port, (unsigned)g_fport);
    return LORA_INIT_OK;
}

void lora_service_close(void)
{
    if (g_lora_fd >= 0) {
        close(g_lora_fd);
        g_lora_fd = -1;
    }
}

/* Returns current time in milliseconds */
static long now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

/*
 * Read the RAK11720 AT response into buf until:
 *   - "OK" token is found  (success)
 *   - "ERROR" or "+ERR" token is found  (failure)
 *   - total_timeout_ms elapses
 * Returns number of bytes accumulated, or -1 on hard read error.
 * buf is always NUL-terminated on return.
 */
static int read_at_response(int fd, char *buf, size_t cap, int total_timeout_ms)
{
    size_t pos = 0;
    buf[0] = '\0';

    const long deadline = now_ms() + total_timeout_ms;

    while (pos + 1 < cap) {
        const long ms_left = deadline - now_ms();
        if (ms_left <= 0) break;

        struct timeval tv;
        tv.tv_sec  = ms_left / 1000;
        tv.tv_usec = (ms_left % 1000) * 1000;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        const int r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r < 0)  return -1;  /* select error */
        if (r == 0) break;      /* deadline reached */

        const ssize_t n = read(fd, buf + pos, cap - pos - 1);
        if (n <= 0) break;
        pos += (size_t)n;
        buf[pos] = '\0';

        if (strstr(buf, "OK")    ||
            strstr(buf, "ERROR") ||
            strstr(buf, "+ERR")) break;
    }

    buf[pos] = '\0';
    return (int)pos;
}

lora_send_status lora_service_forward(const uint8_t *buf, size_t len)
{
    if (!buf || len == 0) {
        zlog_error(error_cat, "LoRa: empty payload, skipping");
        return LORA_SEND_ERROR;
    }

    if (len > LORA_MAX_PAYLOAD) {
        zlog_error(error_cat, "LoRa: payload too large (%zu > %d)", len, LORA_MAX_PAYLOAD);
        return LORA_SEND_ERROR;
    }

    /* Hex-encode the raw SCHC bytes */
    char hex[LORA_HEX_BUF];
    for (size_t i = 0; i < len; i++) {
        snprintf(&hex[i * 2], 3, "%02X", buf[i]);
    }

    /* Build AT+SEND=<fport>:<hex>\r\n */
    char cmd[LORA_CMD_BUF];
    const int cmd_len = snprintf(cmd, sizeof(cmd), "AT+SEND=%u:%s\r\n", (unsigned)g_fport, hex);
    if (cmd_len <= 0 || (size_t)cmd_len >= sizeof(cmd)) {
        zlog_error(error_cat, "LoRa: AT command buffer overflow");
        return LORA_SEND_ERROR;
    }

    zlog_info(rx_cat, "LoRa TX: AT+SEND=%u:<schc len=%zu>", (unsigned)g_fport, len);

    tcflush(g_lora_fd, TCIOFLUSH);

    if (write(g_lora_fd, cmd, (size_t)cmd_len) != cmd_len) {
        zlog_error(error_cat, "LoRa: UART write failed");
        return LORA_SEND_ERROR;
    }

    /* Read and evaluate RAK11720 response */
    char rsp[LORA_RSP_BUF];
    const int rsp_len = read_at_response(g_lora_fd, rsp, sizeof(rsp), LORA_RSP_TIMEOUT_MS);

    if (rsp_len < 0) {
        zlog_error(error_cat, "LoRa: UART read error");
        return LORA_SEND_ERROR;
    }

    if (rsp_len == 0) {
        zlog_error(error_cat, "LoRa: AT response timeout (no data)");
        return LORA_SEND_ERROR;
    }

    zlog_info(rx_cat, "LoRa RSP: %s", rsp);

    if (strstr(rsp, "OK") != NULL) {
        zlog_info(ok_cat, "LoRa: AT+SEND OK");
        return LORA_SEND_OK;
    }

    zlog_error(error_cat, "LoRa: AT+SEND failed");
    return LORA_SEND_ERROR;
}
