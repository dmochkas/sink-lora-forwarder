#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    LORA_INIT_OK,
    LORA_INIT_ERROR
} lora_init_status;

typedef enum {
    LORA_SEND_OK,
    LORA_SEND_ERROR
} lora_send_status;

/* Configure before calling lora_service_init() */
void lora_service_set_port(const char *port);
void lora_service_set_baudrate(int32_t baud);
void lora_service_set_fport(uint8_t fport);

lora_init_status lora_service_init(void);

/* Hex-encode buf and send AT+SEND=<fport>:<hex> to the RAK11720 */
lora_send_status lora_service_forward(const uint8_t *buf, size_t len);

void lora_service_close(void);
