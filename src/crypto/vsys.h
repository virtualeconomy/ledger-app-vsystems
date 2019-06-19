#ifndef __vsys_H__
#define __vsys_H__

#include <stdint.h>
#include <stdbool.h>

#include "base58.h"
#include "os.h"
#include "cx.h"

#define PAYMENT_TX 2
#define LEASE_TX 3
#define CANCEL_LEASE_TX 4

typedef unsigned char ed25519_signature[64];
typedef unsigned char ed25519_public_key[32];
typedef unsigned char ed25519_secret_key[32];

void vsys_public_key_to_address(const ed25519_public_key public_key, const unsigned char network_byte, unsigned char *output);
void vsys_message_sign(const cx_ecfp_private_key_t *private_key, const ed25519_public_key public_key,
    const unsigned char *message, const size_t message_size, ed25519_signature signature);
void copy_in_reverse_order(unsigned char *to, const unsigned char *from, const unsigned int len);
bool is_tx_type_supported(uint8_t type);
uint64_t get_amount_from_buff(unsigned char *buff, uint8_t type);
void get_recipient_from_buff(unsigned char *buff, uint8_t type, unsigned char *recipient);
void get_tx_id_from_buff(unsigned char *buff, uint8_t type, unsigned char *tx_id);

#endif
