#include "sink_lora_forwarder/l2/l2.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include <ahoilib.h>

#include "sink_lora_forwarder/logger_helper.h"

#define AHOI_FRAME_CAP   512
#define AHOI_TIMEOUT_MS  1500

#define AHOI_DLE 0x10
#define AHOI_STX 0x02
#define AHOI_ETX 0x03

#define AHOI_SCHC_OFFSET 6

static int g_ahoi_fd = -1;
static const char* port = NULL;
static int32_t baudrate = -1;
static uint8_t modem_id = 0x00;
static uint32_t modem_id_32 = 0x00;

static bool g_mock = false;

static long now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

static void dump_hex(const char *prefix, const uint8_t *buf, size_t len)
{
    if (!buf) return;

    printf("%s (%zu): ", prefix, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
    fflush(stdout);
}

static int read_ahoi_serial_frame(uint8_t *frame, size_t cap, int timeout_ms)
{
    if (!frame || cap == 0) return -1;

    bool in_frame = false;
    bool dle_seen = false;
    size_t pos = 0;

    const long deadline = now_ms() + timeout_ms;

    while (now_ms() < deadline) {
        const long ms_left = deadline - now_ms();
        if (ms_left <= 0) break;

        struct timeval tv;
        tv.tv_sec = ms_left / 1000;
        tv.tv_usec = (ms_left % 1000) * 1000;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(g_ahoi_fd, &fds);

        const int r = select(g_ahoi_fd + 1, &fds, NULL, NULL, &tv);
        if (r < 0) return -1;
        if (r == 0) break;

        uint8_t b = 0;
        const ssize_t n = read(g_ahoi_fd, &b, 1);
        if (n <= 0) continue;

        if (!in_frame) {
            if (!dle_seen) {
                dle_seen = (b == AHOI_DLE);
                continue;
            }

            if (b == AHOI_STX) {
                in_frame = true;
                dle_seen = false;
                pos = 0;
                continue;
            }

            dle_seen = (b == AHOI_DLE);
            continue;
        }

        if (!dle_seen) {
            if (b == AHOI_DLE) {
                dle_seen = true;
                continue;
            }

            if (pos >= cap) return -2;
            frame[pos++] = b;
            continue;
        }

        if (b == AHOI_ETX) {
            return (int)pos;
        }

        if (b == AHOI_DLE) {
            if (pos >= cap) return -2;
            frame[pos++] = AHOI_DLE;
            dle_seen = false;
            continue;
        }

        if (b == AHOI_STX) {
            pos = 0;
            dle_seen = false;
            continue;
        }

        if (pos >= cap) return -2;
        frame[pos++] = b;
        dle_seen = false;
    }

    return 0;
}

l2_init_status l2_init(void)
{
    if (port && strcmp(port, "mock") == 0) {
        g_mock = true;
        zlog_info(ok_cat, "AHOI MOCK: mock mode active, no serial port opened");
        return L2_INIT_OK;
    }

    g_ahoi_fd = open_serial_port((const uint8_t*)port, baudrate);
    if (g_ahoi_fd == -1) {
        zlog_error(error_cat, "Error opening serial port");
        return L2_INIT_ERROR;
    }

    tcflush(g_ahoi_fd, TCIOFLUSH);
    usleep(300000);
    tcflush(g_ahoi_fd, TCIOFLUSH);

    set_ahoi_id(g_ahoi_fd, modem_id);
    usleep(300000);
    tcflush(g_ahoi_fd, TCIOFLUSH);

    zlog_info(ok_cat, "AHOI initialized safely on %s", port);
    return L2_INIT_OK;
}

void l2_ahoi_set_port(const char* val)
{
    port = val;
}

void l2_ahoi_set_baudrate(const int32_t val)
{
    baudrate = val;
}

void l2_set_id(const uint32_t id)
{
    modem_id = (uint8_t) id;
    modem_id_32 = id;
}

uint32_t* l2_get_id(void)
{
    return &modem_id_32;
}

uint8_t* l2_get_id_byte(void)
{
    return &modem_id;
}

void l2_recv_prepare(const void* params)
{
    (void)params;
}

l2_recv_status l2_recv_run(uint8_t* payload, const size_t cap, size_t* out_len)
{
    if (!payload || !out_len) return L2_RECV_KO;
    *out_len = 0;

    if (g_mock) {
        static bool mock_sent = false;
        if (!mock_sent) {
            static const uint8_t mock_payload[] = {
                0x1c, 0x1e, 0xe3, 0xcf, 0xa4, 0x0c,
                0xea, 0x1f, 0x54, 0x01, 0x10
            };

            const size_t mock_len = sizeof(mock_payload);
            if (mock_len > cap) return L2_RECV_KO;

            memcpy(payload, mock_payload, mock_len);
            *out_len = mock_len;
            mock_sent = true;

            zlog_info(ok_cat, "AHOI MOCK: injecting fixed payload");
            return L2_RECV_OK;
        }

        return L2_RECV_TIMEOUT;
    }

    uint8_t frame[AHOI_FRAME_CAP];

    const int frame_len = read_ahoi_serial_frame(
        frame,
        sizeof(frame),
        AHOI_TIMEOUT_MS
    );

    if (frame_len == 0) {
        return L2_RECV_TIMEOUT;
    }

    if (frame_len < 0) {
        zlog_error(error_cat, "AHOI safe frame read failed: %d", frame_len);
        return L2_RECV_KO;
    }

    dump_hex("AHOI SERIAL FRAME", frame, (size_t)frame_len);

    if (frame_len <= AHOI_SCHC_OFFSET) {
        zlog_error(error_cat, "AHOI frame too short: %d", frame_len);
        return L2_RECV_KO;
    }

    const size_t schc_offset = AHOI_SCHC_OFFSET;
    const size_t schc_len = frame[5];

    if (schc_len == 0) {
        zlog_warn(rx_cat, "AHOI frame has empty SCHC payload");
        return L2_RECV_TIMEOUT;
    }

    if (schc_offset + schc_len > (size_t)frame_len) {
        zlog_error(error_cat,
                   "AHOI frame invalid: offset=%zu len=%zu frame_len=%d",
                   schc_offset,
                   schc_len,
                   frame_len);
        return L2_RECV_KO;
    }

    if (schc_len > cap) {
        zlog_error(error_cat,
                   "SCHC payload too large: %zu > %zu",
                   schc_len,
                   cap);
        return L2_RECV_KO;
    }

    memcpy(payload, frame + schc_offset, schc_len);
    *out_len = schc_len;

    dump_hex("SCHC PAYLOAD", payload, *out_len);

    return L2_RECV_OK;
}
