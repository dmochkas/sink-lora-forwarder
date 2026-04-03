#include "sink_lora_forwarder/cli_helper.h"

#include <getopt.h>
#include <stdlib.h>

#include "sink_lora_forwarder/logger_helper.h"

cli_parse_status parse_cli_arguments(
    int argc,
    char *argv[],
    uint8_t *id,
    char **port,
    char **lora_port)
{
    static struct option options[] = {
        {"id",        required_argument, 0, 'i'},
        {"port",      required_argument, 0, 'p'},
        {"lora-port", required_argument, 0, 'P'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:p:P:", options, NULL)) != -1) {
        switch (opt) {
        case 'i':
            if (id) *id = (uint8_t)atoi(optarg);
            break;

        case 'p':
            if (port) *port = optarg;
            break;

        case 'P':
            if (lora_port) *lora_port = optarg;
            break;

        default:
            return CLI_PARSE_KO;
        }
    }

    if (!port || !*port) {
        zlog_error(error_cat, "--port is required");
        return CLI_PARSE_KO;
    }

    if (!lora_port || !*lora_port) {
        zlog_error(error_cat, "--lora-port is required");
        return CLI_PARSE_KO;
    }

    return CLI_PARSE_OK;
}
