#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "sink_lora_forwarder/logger_helper.h"
#include "sink_lora_forwarder/cli_helper.h"
#include "sink_lora_forwarder/services/lora_service.h"
#include "sink_lora_forwarder/l2/l2.h"

#define RX_BUF_CAP      1024
#define AHOI_BAUD       B115200
#define LORA_BAUD       B115200
#define LORA_FPORT      10

static void forward_one_frame(const uint8_t *buf, size_t len)
{
    if (!buf || len == 0) {
        zlog_error(error_cat, "Empty frame, skipping");
        return;
    }

    hex_dump(rx_cat, "SCHC RX", buf, len);

    const lora_send_status st = lora_service_forward(buf, len);

    if (st != LORA_SEND_OK) {
        zlog_error(error_cat, "LoRa forward failed (len=%zu)", len);
    } else {
        zlog_info(ok_cat, "LoRa forward OK (len=%zu)", len);
    }
}

int main(int argc, char *argv[])
{
    if (logger_init() != LOGGER_INIT_OK) {
        fprintf(stderr, "Logger initialization failed\n");
        return EXIT_FAILURE;
    }

    uint8_t  id_arg    = 0x00;
    char    *port      = NULL;
    char    *lora_port = NULL;

    if (parse_cli_arguments(argc, argv, &id_arg, &port, &lora_port) != CLI_PARSE_OK) {
        zlog_error(error_cat, "Error parsing CLI arguments");
        zlog_fini();
        return EXIT_FAILURE;
    }

    zlog_info(ok_cat, "Forwarder starting (id=%u fport=%d)", (unsigned)id_arg, LORA_FPORT);

    l2_set_id((uint32_t)id_arg);

#ifdef L2_AHOI_EXT
    l2_ahoi_set_port(port);
    l2_ahoi_set_baudrate(AHOI_BAUD);
#endif

    if (l2_init() != L2_INIT_OK) {
        zlog_error(error_cat, "Layer 2 init failed");
        zlog_fini();
        return EXIT_FAILURE;
    }
    zlog_info(ok_cat, "Layer 2 initialized");

    lora_service_set_port(lora_port);
    lora_service_set_baudrate(LORA_BAUD);
    lora_service_set_fport(LORA_FPORT);

    if (lora_service_init() != LORA_INIT_OK) {
        zlog_error(error_cat, "LoRa service init failed");
        lora_service_close();
        zlog_fini();
        return EXIT_FAILURE;
    }
    zlog_info(ok_cat, "LoRa service initialized");

    zlog_info(ok_cat, "Waiting for packets...");

    uint8_t rx_buf[RX_BUF_CAP];
    size_t  rx_len = 0;

    for (;;) {
        rx_len = 0;

        const l2_recv_status r = l2_recv_run(rx_buf, sizeof(rx_buf), &rx_len);

        if (r == L2_RECV_TIMEOUT) {
            continue;
        }

        if (r != L2_RECV_OK) {
            zlog_error(error_cat, "L2 receive failed");
            continue;
        }

        forward_one_frame(rx_buf, rx_len);

        /* MOCK: exit cleanly after one full cycle in full mock mode - remove when hardware is available */
        if (strcmp(port, "mock") == 0 && strcmp(lora_port, "mock") == 0) {
            zlog_info(ok_cat, "MOCK: full mock cycle complete, exiting");
            lora_service_close();
            zlog_fini();
            return EXIT_SUCCESS;
        }
        /* END MOCK */
    }
}
