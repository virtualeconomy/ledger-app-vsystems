#include "vsys.h"
#include "ledger_crypto.h"

void vsys_secure_hash(const uint8_t *message, size_t message_len, uint8_t hash[32])
{
    blake2b_256(message, message_len, hash);
    keccak_256(hash, 32, hash);
}

void vsys_message_sign(const cx_ecfp_private_key_t *private_key, const ed25519_public_key public_key, const unsigned char *message, const size_t message_size, ed25519_signature signature) {
    // ed25519 signature with the sha512 hashing
    cx_eddsa_sign(private_key, 0, CX_SHA512, message, message_size, NULL, 0, signature, 64, NULL);
    // set the sign bit from ed25519 public key (using 31 byte) for curve25519 validation used in vsys (this makes the ed25519 signature invalid)
    unsigned char sign_bit = public_key[31] & 0x80;
    signature[63] |= sign_bit;
}

// Build vsys address from the curve25519 public key
void vsys_public_key_to_address(const ed25519_public_key public_key, const unsigned char network_byte, unsigned char *output) {
    uint8_t public_key_hash[32];
    uint8_t address[26];
    uint8_t checksum[32];
    vsys_secure_hash(public_key, 32, public_key_hash);

    address[0] = 0x05;
    address[1] = network_byte;
    os_memmove(&address[2], public_key_hash, 20);

    vsys_secure_hash(address, 22, checksum);

    os_memmove(&address[22], checksum, 4);

    size_t length = 36;
    b58enc((char *) output, &length, address, 26);
}

void copy_in_reverse_order(unsigned char *to, const unsigned char *from, const unsigned int len) {
    for (uint16_t i = 0; i < len; i++) {
        to[i] = from[(len - 1) - i];
    }
}

bool is_tx_type_supported(uint8_t type) {
    return type == PAYMENT_TX || type == LEASE_TX || type == CANCEL_LEASE_TX;
}


uint64_t get_amount_from_buff(unsigned char *buff, uint8_t type) {
    uint32_t offset;
    if (type == PAYMENT_TX) {
        offset = 9;
    } else if (type == LEASE_TX) {
        offset = 27;
    } else {
        return 0;
    }
    uint32_t i; 
    uint64_t amount = 0;
    for (i = offset; i < offset + 8; i++) {
        amount = amount * 256 + buff[i];
    }
    return amount;
}

void get_recipient_from_buff(unsigned char *buff, uint8_t type, unsigned char *recipient) {
    uint32_t offset;
    if (type == PAYMENT_TX) {
        offset = 27;
    } else if (type == LEASE_TX) {
        offset = 1;
    } else {
        recipient[0] = '\0';
        return;
    }
    size_t length = 36;
    b58enc((char *) recipient, &length, buff + offset, 26);
}

void get_tx_id_from_buff(unsigned char *buff, uint8_t type, unsigned char *tx_id) {
    uint32_t offset;
    if (type == CANCEL_LEASE_TX) {
        offset = 19;
    } else {
        tx_id[0] = '\0';
        return;
    }
    size_t length = 45;
    b58enc((char *) tx_id, &length, buff + offset, 32);
}