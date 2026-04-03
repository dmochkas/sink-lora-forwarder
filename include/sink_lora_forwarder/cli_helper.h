#pragma once

#include <stdint.h>

typedef enum {
    CLI_PARSE_OK,
    CLI_PARSE_KO
} cli_parse_status;

cli_parse_status parse_cli_arguments(
    int argc,
    char *argv[],
    uint8_t *id,
    char **port,
    char **lora_port
);
