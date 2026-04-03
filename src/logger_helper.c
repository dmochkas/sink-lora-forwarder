#include "sink_lora_forwarder/logger_helper.h"

#include <stdio.h>

/* ------------------------ zlog categories ------------------------ */

zlog_category_t* ok_cat    = NULL;
zlog_category_t* error_cat = NULL;
zlog_category_t* rx_cat    = NULL;

logger_status logger_init(void)
{
    const int rc = zlog_init(LOG_CONFIG_FILE);
    if (rc) {
        fprintf(stderr, "Config file %s is corrupt\n", LOG_CONFIG_FILE);
        return LOGGER_INIT_KO;
    }

    ok_cat = zlog_get_category("ok");
    if (!ok_cat) {
        fprintf(stderr, "OK category init failed\n");
        zlog_fini();
        return LOGGER_INIT_KO;
    }

    error_cat = zlog_get_category("error");
    if (!error_cat) {
        fprintf(stderr, "Error category init failed\n");
        zlog_fini();
        return LOGGER_INIT_KO;
    }

    rx_cat = zlog_get_category("rx");
    if (!rx_cat) {
        fprintf(stderr, "RX category init failed\n");
        zlog_fini();
        return LOGGER_INIT_KO;
    }

    zlog_info(ok_cat, "Logger initialized");
    return LOGGER_INIT_OK;
}

/* ------------------------ generic logging helpers ------------------------ */

void hex_dump(zlog_category_t* cat, const char* prefix, const uint8_t* buf, size_t len)
{
    if (!cat || !prefix || (!buf && len)) return;

    char line[2048];
    size_t pos = 0;

    pos += (size_t)snprintf(line + pos, sizeof(line) - pos, "%s (%zu): ", prefix, len);
    for (size_t i = 0; i < len && pos + 4 < sizeof(line); i++) {
        pos += (size_t)snprintf(line + pos, sizeof(line) - pos, "%02x ", buf[i]);
    }

    zlog_info(cat, "%s", line);
}

void log_ahoi_packet(zlog_category_t* cat, const ahoi_packet_t* p)
{
    if (!cat || !p) return;

    zlog_info(cat,
              "AHOI RX: src=%u dst=%u type=%u flags=%u seq=%u pl_size=%u",
              p->src, p->dst, p->type, p->flags, p->seq, p->pl_size);
}
