
/*******************************************************************************
*   Burstcoin Wallet App for Nano Ledger S. Updated By vsys community.
*   Copyright (c) 2017-2018 Jake B.
* 
*   Based on Sample code provided and (c) 2016 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "ui.h"
#include <stdbool.h>
#include "../glyphs.h"
#include "../main.h"
#include "../crypto/vsys.h"
#ifdef TARGET_NANOS
#include "nanos/ui_menus_nanos.h"
#include "nanos/ui_menus_buttons.h"
#include "nanos/ui_menus_prepro.h"
#endif
#ifdef TARGET_BLUE
#include "blue/ui_menus_blue.h"
#include "blue/ui_menus_blue_prepro.h"
#endif

ux_state_t ux;

// UI currently displayed
enum UI_STATE ui_state;

int ux_step, ux_step_count;

bool print_amount(uint64_t amount, int decimals, unsigned char *out, uint8_t len);

void menu_address_init() {
    ux_step = 0;
    ux_step_count = 2;
    #if defined(TARGET_BLUE)
        UX_DISPLAY(ui_address_blue, ui_address_blue_prepro);
    #elif defined(TARGET_NANOS)
        UX_DISPLAY(ui_address_nanos, ui_address_prepro);
    #endif // #if TARGET_ID
}

// Idle state, sow the menu
void ui_idle() {
    ux_step = 0; ux_step_count = 0;
    ui_state = UI_IDLE;
    #if defined(TARGET_BLUE)
        UX_DISPLAY(ui_idle_blue, ui_idle_blue_prepro);
    #elif defined(TARGET_NANOS)
        UX_MENU_DISPLAY(0, menu_main, NULL);
    #endif // #if TARGET_ID
}

// Show the transaction details for the user to approve
void menu_sign_init() {
    os_memset((unsigned char *) &ui_context, 0, sizeof(uiContext_t));
    unsigned char tx_type = tmp_ctx.signing_context.data_type;

    if (tx_type == PAYMENT_TX) {
        os_memmove(&ui_context.line1, &"Payment ", 8);
        print_amount(tmp_ctx.signing_context.amount, VSYS_DECIMALS, (unsigned char*) ui_context.line1 + 8, 35);
    } else if (tx_type == LEASE_TX) {
        os_memmove(&ui_context.line1, &"Lease ", 6);
        print_amount(tmp_ctx.signing_context.amount, VSYS_DECIMALS, (unsigned char*) ui_context.line1 + 6, 35);
    } else if (tx_type == CANCEL_LEASE_TX) {
        os_memmove(&ui_context.line1, &"Cancel lease\0", 13);
    } else {
        THROW(SW_FUNC_NOT_SUPPORTED);
    }

    if (tx_type == PAYMENT_TX || tx_type == LEASE_TX) {
        os_memmove(&ui_context.line2, &"To\0", 3);
        os_memmove(&ui_context.line3, &tmp_ctx.signing_context.to, 36);
    } else if (tx_type == CANCEL_LEASE_TX) {
        os_memmove(&ui_context.line2, &"Cancel lease ID\0", 16);
        os_memmove(&ui_context.line3, &tmp_ctx.signing_context.id, 45);
    }

    // Get the public key and return it.
    cx_ecfp_public_key_t public_key;

    if (!get_curve25519_public_key_for_path((uint32_t *) tmp_ctx.signing_context.bip32, &public_key)) {
        THROW(INVALID_PARAMETER);
    }

    vsys_public_key_to_address(public_key.W, tmp_ctx.signing_context.network_byte, ui_context.line4);

    ux_step = 0; ux_step_count = 3;
    ui_state = UI_VERIFY;
    #if defined(TARGET_BLUE)
        UX_DISPLAY(ui_approval_blue, ui_approval_blue_prepro);
    #elif defined(TARGET_NANOS)
        UX_DISPLAY(ui_verify_transaction_nanos, ui_verify_transaction_prepro);
    #endif // #if TARGET_ID
}


// borrowed from the Stellar wallet code and modified
bool print_amount(uint64_t amount, int decimals, unsigned char *out, uint8_t len) {
    char buffer[len];
    uint64_t dVal = amount;
    int i, j;

    if (decimals == 0) decimals--;

    memset(buffer, 0, len);
    for (i = 0; dVal > 0 || i < decimals + 2; i++) {
        if (dVal > 0) {
            buffer[i] = (char) ((dVal % 10) + '0');
            dVal /= 10;
        } else {
            buffer[i] = '0';
        }
        if (i == decimals - 1) {
            i += 1;
            buffer[i] = '.';
        }
        if (i >= len) {
            return false;
        }
    }
    // reverse order
    for (i -= 1, j = 0; i >= 0 && j < len-1; i--, j++) {
        out[j] = buffer[i];
    }
    if (decimals > 0) {
        // strip trailing 0s
        for (j -= 1; j > 0; j--) {
            if (out[j] != '0') break;
        }
        j += 1;
        if (out[j - 1] == '.') j -= 1;
    }

    out[j] = '\0';
    return  true;
}