/* key_data.c
 *
 * Copyright (C) 2006-2025 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#include "key_data.h"

/*-------------------------------------------------------------------------
      TSIP v1.09 or later
--------------------------------------------------------------------------*/
#if defined(WOLFSSL_RENESAS_TSIP_TLS) && (WOLFSSL_RENESAS_TSIP_VER >=109)

const st_key_block_data_t g_key_block_data =
{
    /* uint8_t encrypted_provisioning_key[R_TSIP_AES_CBC_IV_BYTE_SIZE * 2]; */
    {
        0x94, 0x4D, 0x99, 0x53, 0xE9, 0x15, 0xD1, 0xD9, 0x0A, 0x2C, 0x17, 0x48,
        0x87, 0x2F, 0x22, 0xA8, 0x90, 0xB3, 0xDE, 0x25, 0x17, 0xEA, 0xE6, 0x31,
        0x28, 0x3F, 0xB9, 0x8F, 0xC6, 0xE3, 0xE4, 0x85
    },
    /* uint8_t iv[R_TSIP_AES_CBC_IV_BYTE_SIZE]; */
    {
        0xF6, 0xA9, 0x83, 0x5A, 0xA1, 0x65, 0x1D, 0x28, 0xC8, 0x1A, 0xA6, 0x9D,
        0x34, 0xB2, 0x4D, 0x92
    },
    /* 
     * uint8_t 
     * encrypted_user_rsa2048_ne_key[R_TSIP_RSA2048_NE_KEY_BYTE_SIZE + 16];
     */
    {
        0x7F, 0xE5, 0x80, 0x89, 0xD7, 0x3E, 0xB9, 0x92, 0xF6, 0xBD, 0x13, 0x4B,
        0x8D, 0xE8, 0x96, 0xC5, 0xAB, 0x56, 0x45, 0x55, 0xD4, 0xA6, 0x57, 0x73,
        0xB5, 0xA8, 0xD7, 0x35, 0xF4, 0x4B, 0x0D, 0xA2, 0x30, 0x5A, 0xFE, 0xCB,
        0x18, 0x06, 0x55, 0xB2, 0x51, 0xF2, 0xA4, 0x0E, 0xCB, 0x6E, 0x6C, 0x88,
        0x03, 0xF3, 0x5C, 0x1E, 0xF0, 0xA4, 0xA8, 0x6E, 0x48, 0xE7, 0xB4, 0x87,
        0xE9, 0xE9, 0xA0, 0xF0, 0xB2, 0xD3, 0x24, 0x8D, 0x2E, 0x8C, 0x11, 0x2C,
        0x05, 0x26, 0x7C, 0xEE, 0x15, 0x67, 0xB8, 0xBF, 0xCA, 0xBC, 0x44, 0x8D,
        0x80, 0xED, 0x94, 0xF1, 0x5B, 0x88, 0xE1, 0xB1, 0x81, 0x7D, 0x4D, 0x92,
        0x6E, 0x1E, 0x3E, 0xF5, 0x7B, 0x77, 0x0A, 0xC8, 0x60, 0xB8, 0x7F, 0x43,
        0x2F, 0x07, 0x3B, 0xCA, 0xF5, 0xC7, 0x6F, 0x8F, 0x9E, 0xC1, 0x39, 0x29,
        0x10, 0xFA, 0xBA, 0xCD, 0x51, 0xDF, 0xF6, 0xAE, 0x6A, 0x84, 0xF4, 0xE0,
        0xED, 0xFC, 0xE2, 0xCE, 0x68, 0x3A, 0x38, 0xBF, 0x9B, 0xAD, 0x6F, 0x8B,
        0x84, 0x95, 0xAA, 0x5B, 0x4C, 0x73, 0xCE, 0x34, 0x8D, 0x84, 0x78, 0x1E,
        0xBF, 0xD6, 0xE2, 0x12, 0xEB, 0x27, 0xA6, 0x96, 0x4C, 0x76, 0x9C, 0x19,
        0x1C, 0x3C, 0x7D, 0xF7, 0xB0, 0xDB, 0xD6, 0x64, 0xFD, 0x67, 0xEB, 0x83,
        0xC1, 0x60, 0x8F, 0x65, 0x19, 0xC0, 0x78, 0xFD, 0x09, 0xD4, 0x52, 0x74,
        0xD6, 0x96, 0x89, 0x91, 0xEF, 0xF6, 0xB6, 0xAB, 0x27, 0x37, 0x7B, 0x43,
        0xA9, 0xEC, 0xDA, 0x68, 0x5F, 0x3A, 0x32, 0xFE, 0xE8, 0x4E, 0x7B, 0xDC,
        0xE4, 0x18, 0x5C, 0x53, 0x15, 0x5B, 0x5E, 0xC7, 0x08, 0x93, 0xF0, 0xBD,
        0xF6, 0xC3, 0x78, 0x80, 0x3B, 0x1F, 0xC8, 0xBA, 0x0F, 0x58, 0xF7, 0x1E,
        0x9C, 0xFB, 0x53, 0xCA, 0xA2, 0xBF, 0x9A, 0x18, 0xEE, 0x26, 0xD2, 0xA8,
        0x88, 0x64, 0x13, 0xC8, 0xEE, 0xD2, 0x79, 0xB5, 0x67, 0xD4, 0x10, 0xB3,
        0xF4, 0xC9, 0xCC, 0xCE, 0x4A, 0xE2, 0x38, 0x8B, 0x77, 0xEB, 0xD2, 0x89,
        0xB0, 0x66, 0xFF, 0xCD, 0x76, 0xC1, 0x28, 0x65, 0xC2, 0xA3, 0xE3, 0x45
    },
    /* uint8_t encrypted_user_update_key[R_TSIP_AES256_KEY_BYTE_SIZE + 16]; */
    {
        0
    },
    /* uint8_t
     * encrypted_user_rsa2048_public_key[R_TSIP_RSA2048_NE_KEY_BYTE_SIZE + 16]
     */
    {
        0x22, 0xEC, 0xE3, 0x79, 0xD1, 0x0C, 0xB4, 0xED, 0xE0, 0xA9, 0x0F, 0xBD,
        0xC7, 0x0B, 0xB4, 0x1E, 0x82, 0x27, 0x79, 0x20, 0x6A, 0x15, 0x56, 0xD6,
        0x0F, 0xFA, 0xE4, 0x61, 0x04, 0xDA, 0x81, 0x33, 0x42, 0xBA, 0x6D, 0xB9,
        0x34, 0x81, 0xFD, 0x67, 0xDF, 0x1D, 0xCB, 0x52, 0x64, 0x9A, 0x2E, 0x30,
        0x12, 0x60, 0xCB, 0x48, 0x27, 0xB6, 0x38, 0x0B, 0x54, 0x5A, 0x70, 0xCB,
        0x6F, 0x05, 0x53, 0x46, 0x8B, 0x08, 0x86, 0x24, 0xCB, 0xA2, 0x36, 0x54,
        0xCD, 0xC9, 0x61, 0x37, 0x43, 0xCB, 0xCC, 0x7C, 0x6B, 0x20, 0x07, 0xB6,
        0x01, 0x96, 0x32, 0xF5, 0x30, 0x0E, 0x96, 0x05, 0xC4, 0xF5, 0xF0, 0xB0,
        0xD3, 0x26, 0xAF, 0x6E, 0x39, 0x01, 0x2D, 0xAC, 0x7E, 0x39, 0xCF, 0x48,
        0x74, 0x7D, 0x98, 0x00, 0xA6, 0x95, 0xDA, 0x4C, 0xAC, 0x81, 0xC7, 0x9A,
        0x93, 0x69, 0x23, 0xD4, 0x39, 0xCD, 0xB3, 0x62, 0x4A, 0x4A, 0x26, 0x95,
        0x51, 0xE1, 0x06, 0x8F, 0xB2, 0x3C, 0x8F, 0xDE, 0xF1, 0xC3, 0x69, 0xFC,
        0xF5, 0xC3, 0xCE, 0x39, 0x3F, 0x7F, 0x2D, 0xA8, 0x8B, 0xB3, 0x2D, 0xE4,
        0xB8, 0xA4, 0x19, 0x55, 0x29, 0xEC, 0x9C, 0x6A, 0xD7, 0xD7, 0x45, 0x3E,
        0x1D, 0x18, 0x52, 0xB5, 0x8A, 0xB1, 0x26, 0x43, 0x8C, 0x7A, 0x65, 0xF1,
        0x17, 0x5E, 0x75, 0x4A, 0x1A, 0x30, 0xD8, 0xEA, 0xC2, 0x19, 0x3C, 0x1E,
        0x2E, 0xFB, 0xDB, 0xC9, 0x1F, 0x73, 0xC9, 0x95, 0x53, 0x8C, 0x99, 0x5D,
        0x3B, 0x59, 0xFE, 0x8B, 0xA1, 0xD2, 0xBB, 0xBC, 0xBC, 0x4B, 0x78, 0x0A,
        0x6B, 0xD8, 0xBE, 0x85, 0x60, 0x08, 0x1E, 0x2F, 0x38, 0x77, 0xA3, 0xC4,
        0x46, 0xC1, 0x42, 0x39, 0xA6, 0x59, 0x98, 0x59, 0x21, 0x94, 0x1F, 0xDC,
        0xB3, 0xAE, 0xA3, 0xB6, 0xCC, 0x35, 0x82, 0x6F, 0x87, 0x58, 0x91, 0x70,
        0xEC, 0x3F, 0x05, 0x86, 0x5E, 0x04, 0x1E, 0xF4, 0xCD, 0xF5, 0xF9, 0x4C,
        0x0D, 0x65, 0x5E, 0x92, 0x1B, 0x67, 0xC5, 0xC5, 0xB6, 0x18, 0x3E, 0x7F,
        0xF7, 0x39, 0xCF, 0xA9, 0x32, 0x44, 0x2B, 0xB6, 0xCB, 0x52, 0x12, 0xAA
    },
    /* uint8_t
     * encrypted_user_rsa2048_private_key[R_TSIP_RSA2048_ND_KEY_BYTE_SIZE + 16]
     */
    {
        0x22, 0xEC, 0xE3, 0x79, 0xD1, 0x0C, 0xB4, 0xED, 0xE0, 0xA9, 0x0F, 0xBD,
        0xC7, 0x0B, 0xB4, 0x1E, 0x82, 0x27, 0x79, 0x20, 0x6A, 0x15, 0x56, 0xD6,
        0x0F, 0xFA, 0xE4, 0x61, 0x04, 0xDA, 0x81, 0x33, 0x42, 0xBA, 0x6D, 0xB9,
        0x34, 0x81, 0xFD, 0x67, 0xDF, 0x1D, 0xCB, 0x52, 0x64, 0x9A, 0x2E, 0x30,
        0x12, 0x60, 0xCB, 0x48, 0x27, 0xB6, 0x38, 0x0B, 0x54, 0x5A, 0x70, 0xCB,
        0x6F, 0x05, 0x53, 0x46, 0x8B, 0x08, 0x86, 0x24, 0xCB, 0xA2, 0x36, 0x54,
        0xCD, 0xC9, 0x61, 0x37, 0x43, 0xCB, 0xCC, 0x7C, 0x6B, 0x20, 0x07, 0xB6,
        0x01, 0x96, 0x32, 0xF5, 0x30, 0x0E, 0x96, 0x05, 0xC4, 0xF5, 0xF0, 0xB0,
        0xD3, 0x26, 0xAF, 0x6E, 0x39, 0x01, 0x2D, 0xAC, 0x7E, 0x39, 0xCF, 0x48,
        0x74, 0x7D, 0x98, 0x00, 0xA6, 0x95, 0xDA, 0x4C, 0xAC, 0x81, 0xC7, 0x9A,
        0x93, 0x69, 0x23, 0xD4, 0x39, 0xCD, 0xB3, 0x62, 0x4A, 0x4A, 0x26, 0x95,
        0x51, 0xE1, 0x06, 0x8F, 0xB2, 0x3C, 0x8F, 0xDE, 0xF1, 0xC3, 0x69, 0xFC,
        0xF5, 0xC3, 0xCE, 0x39, 0x3F, 0x7F, 0x2D, 0xA8, 0x8B, 0xB3, 0x2D, 0xE4,
        0xB8, 0xA4, 0x19, 0x55, 0x29, 0xEC, 0x9C, 0x6A, 0xD7, 0xD7, 0x45, 0x3E,
        0x1D, 0x18, 0x52, 0xB5, 0x8A, 0xB1, 0x26, 0x43, 0x8C, 0x7A, 0x65, 0xF1,
        0x17, 0x5E, 0x75, 0x4A, 0x1A, 0x30, 0xD8, 0xEA, 0xC2, 0x19, 0x3C, 0x1E,
        0x2E, 0xFB, 0xDB, 0xC9, 0x1F, 0x73, 0xC9, 0x95, 0x53, 0x8C, 0x99, 0x5D,
        0x3B, 0x59, 0xFE, 0x8B, 0xA1, 0xD2, 0xBB, 0xBC, 0xBC, 0x4B, 0x78, 0x0A,
        0x6B, 0xD8, 0xBE, 0x85, 0x60, 0x08, 0x1E, 0x2F, 0x38, 0x77, 0xA3, 0xC4,
        0x46, 0xC1, 0x42, 0x39, 0xA6, 0x59, 0x98, 0x59, 0x21, 0x94, 0x1F, 0xDC,
        0xB3, 0xAE, 0xA3, 0xB6, 0xCC, 0x35, 0x82, 0x6F, 0x87, 0x58, 0x91, 0x70,
        0xEC, 0x3F, 0x05, 0x86, 0x3C, 0xD1, 0x83, 0xB0, 0xBA, 0xE8, 0xD7, 0xF6,
        0xE5, 0x9E, 0x50, 0xA4, 0x04, 0xF3, 0x95, 0x9C, 0x41, 0xAF, 0xEB, 0x9D,
        0xB6, 0xA2, 0xF7, 0x80, 0x5A, 0xC4, 0x67, 0xD4, 0xFD, 0x0C, 0x2A, 0x7C,
        0x24, 0x03, 0xE5, 0x7F, 0x08, 0x69, 0x18, 0x4E, 0x33, 0x22, 0x7E, 0x03,
        0x8E, 0x13, 0xFB, 0xC6, 0x9B, 0xA3, 0x44, 0x5B, 0xEC, 0xF9, 0x5C, 0x93,
        0x3E, 0x9F, 0xBD, 0x72, 0x50, 0x52, 0xF3, 0xEA, 0x34, 0x2B, 0x8E, 0xEC,
        0x17, 0x42, 0x1A, 0xDF, 0x57, 0xA2, 0x66, 0xA7, 0x0B, 0x3B, 0xE5, 0xF7,
        0x64, 0x44, 0xA2, 0xA3, 0x5F, 0xB7, 0x5B, 0xF5, 0x99, 0x8C, 0xA0, 0x64,
        0x04, 0x46, 0x7C, 0x60, 0x75, 0xE4, 0xD9, 0xD5, 0xED, 0x21, 0x1D, 0x65,
        0x5D, 0x25, 0x21, 0x90, 0x7D, 0xA4, 0x95, 0x15, 0x54, 0x91, 0xD6, 0xB8,
        0xF9, 0xB7, 0x30, 0xD9, 0x7D, 0x2C, 0x7E, 0x85, 0x6A, 0xCF, 0x31, 0x1C,
        0xDC, 0x90, 0xA3, 0x75, 0xA4, 0xE4, 0x89, 0x5F, 0x2B, 0xED, 0x93, 0x4E,
        0xA8, 0x35, 0xD3, 0x20, 0x36, 0x7A, 0x02, 0x23, 0x70, 0xB7, 0xC2, 0x39,
        0xA4, 0x69, 0xB2, 0xBF, 0xA4, 0x3A, 0x6D, 0xB7, 0xB1, 0x32, 0x32, 0x9A,
        0x95, 0xEA, 0x02, 0xD2, 0x35, 0xE7, 0x8C, 0xD8, 0x22, 0xA9, 0xC3, 0x4C,
        0x68, 0x09, 0xA1, 0x2B, 0xB0, 0xE4, 0x55, 0x72, 0x7E, 0xE6, 0x00, 0xDD,
        0xC6, 0xD1, 0x83, 0x56, 0x08, 0x35, 0xC7, 0x9A, 0x98, 0x0C, 0x79, 0xBD,
        0x49, 0xBC, 0x3F, 0x45, 0x4A, 0x38, 0x1B, 0x55, 0x2D, 0x52, 0x57, 0x00,
        0xFC, 0xDB, 0xE7, 0xA0, 0x53, 0xB4, 0x63, 0xE4, 0x37, 0xEC, 0x9F, 0xF0,
        0xA4, 0x0F, 0xEF, 0x06, 0xF0, 0xC6, 0x5F, 0x0C, 0x4A, 0xB8, 0xC1, 0x96,
        0x1D, 0x0D, 0x9A, 0x43, 0x89, 0xA4, 0xF6, 0x55, 0x4A, 0x83, 0x54, 0x3F,
        0xCC, 0x09, 0x65, 0x34, 0xC0, 0x0F, 0x42, 0xF2, 0xE6, 0xDD, 0x2D, 0x8C,
        0x93, 0x42, 0x09, 0xF6, 0xE4, 0xDF, 0xB2, 0x64, 0x2A, 0x12, 0x7A, 0x59
    },
    /* uint8_t
     * encrypted_user_ecc256_public_key[R_TSIP_ECC_PUBLIC_KEY_BYTE_SIZE + 16];
     */
    {   
        0xD5, 0xC3, 0x55, 0x9A, 0xA4, 0xFB, 0xB0, 0x86, 0x0A, 0x75, 0x20, 0x6A,
        0xF9, 0x20, 0x63, 0xC6, 0xA0, 0xB8, 0xED, 0x91, 0xAF, 0x42, 0xD0, 0x23,
        0x99, 0xF8, 0x21, 0xF1, 0x22, 0x6C, 0xB5, 0x42, 0x6A, 0x29, 0xB4, 0xC4,
        0x96, 0xA4, 0x9C, 0xB2, 0xA2, 0x79, 0xC1, 0xAB, 0x55, 0xEE, 0xDE, 0xF6,
        0xFE, 0xA1, 0xD7, 0x99, 0x3A, 0x91, 0xF7, 0x4E, 0xC1, 0xB2, 0xE0, 0x3F,
        0x07, 0xD6, 0xA4, 0xFB, 0xF6, 0x61, 0x00, 0xAE, 0x52, 0x6B, 0x1A, 0xCC,
        0x70, 0x2F, 0xC4, 0xE2, 0x60, 0x36, 0x01, 0x17
    },
    /* uint8_t
     * encrypted_user_ecc256_private_key[R_TSIP_ECC_PRIVATE_KEY_BYTE_SIZE + 16];
     */
    {
        0x25, 0x7A, 0x0C, 0x8C, 0x9B, 0xF9, 0x3C, 0xF5, 0x49, 0x08, 0x86, 0x92,
        0x00, 0x7D, 0xAB, 0x21, 0x23, 0x38, 0xF8, 0xEA, 0xB0, 0x95, 0x59, 0x9B,
        0x04, 0xC8, 0xAA, 0xEA, 0x8C, 0x2D, 0x68, 0xF4, 0xD5, 0xD7, 0xA9, 0x6F,
        0x2C, 0x8C, 0x13, 0x93, 0xA3, 0x80, 0x11, 0xF9, 0x0F, 0x36, 0x14, 0xA8
    },
};

/* Public key type of CA root cert: 0: RSA-2048 2: ECDSA-P256*/
#if defined(USE_ECC_CERT)
const uint32_t              encrypted_user_key_type =
                                    R_TSIP_TLS_PUBLIC_KEY_TYPE_ECDSA_P256;
#else
const uint32_t              encrypted_user_key_type =
                                    R_TSIP_TLS_PUBLIC_KEY_TYPE_RSA2048;
#endif

const unsigned char ca_ecc_cert_der_sig[] =
{
        0xAD, 0x89, 0x0C, 0x68, 0x8E, 0x97, 0xE5, 0x23, 0xE4, 0x35,
        0x91, 0x2F, 0x1B, 0x2F, 0x48, 0xCC, 0x03, 0xFC, 0x18, 0xE1,
        0x64, 0x8C, 0x4D, 0x12, 0xBB, 0xC1, 0xDD, 0xFE, 0xDF, 0x3B,
        0x87, 0xB0, 0x5B, 0x84, 0x54, 0xE6, 0xAE, 0x6D, 0xE4, 0x08,
        0x91, 0xF0, 0xBD, 0x11, 0xCA, 0xC4, 0xF1, 0x44, 0x41, 0x4C,
        0x17, 0x65, 0xAD, 0xEC, 0xE5, 0x08, 0xD7, 0x9D, 0x3D, 0x95,
        0x2A, 0x2B, 0x85, 0x70, 0x75, 0xC7, 0xEB, 0x2F, 0xB2, 0x5C,
        0x07, 0xB8, 0x80, 0xBA, 0x6C, 0x5A, 0x78, 0x1C, 0xAC, 0xBC,
        0x00, 0x2C, 0x9A, 0x21, 0x4E, 0x2A, 0xBA, 0x8E, 0x7D, 0x27,
        0x82, 0xF8, 0xA9, 0x5A, 0xB3, 0x28, 0x82, 0x45, 0x1D, 0xF7,
        0x5C, 0x06, 0x6C, 0xFA, 0x00, 0xE4, 0x8D, 0x0C, 0xC7, 0xBC,
        0x16, 0x50, 0x84, 0xCE, 0x74, 0xAC, 0x67, 0x5E, 0xE0, 0x19,
        0xF3, 0xFC, 0xD2, 0x1D, 0x46, 0x00, 0x63, 0x5E, 0xF8, 0xAC,
        0x70, 0x82, 0x7C, 0x78, 0xD2, 0xD6, 0x42, 0xB0, 0xBC, 0x6E,
        0x41, 0xCC, 0x3E, 0x08, 0x39, 0x29, 0xF4, 0xA6, 0xF5, 0x3D,
        0x81, 0x0A, 0xF8, 0x12, 0xD8, 0xD1, 0x15, 0xA2, 0x4A, 0x4F,
        0x13, 0x07, 0x9A, 0x56, 0x92, 0x51, 0xA2, 0xD6, 0x6B, 0xD9,
        0xF9, 0x86, 0x8B, 0xBE, 0x05, 0xDE, 0x76, 0x66, 0x89, 0x73,
        0x02, 0x19, 0x5C, 0xAC, 0xDE, 0x1E, 0x52, 0x80, 0x65, 0x42,
        0x5D, 0xBB, 0xB4, 0xED, 0xCF, 0x1B, 0x5E, 0xED, 0xA1, 0xC2,
        0x24, 0xAB, 0xBD, 0x30, 0xB2, 0xAE, 0x65, 0x8D, 0xE1, 0xDC,
        0xA3, 0xC7, 0x43, 0xC0, 0xE4, 0xB9, 0x66, 0x91, 0x64, 0xFD,
        0x12, 0x42, 0x12, 0x18, 0x4D, 0x7D, 0xF4, 0x14, 0xE5, 0x9E,
        0x81, 0x38, 0xFB, 0x32, 0x3B, 0x54, 0xFA, 0x4A, 0x6F, 0x25,
        0xA7, 0x3F, 0x45, 0x5D, 0x99, 0xC5, 0x4A, 0xE1, 0xEF, 0x12,
        0x5E, 0x03, 0x30, 0xBC, 0x5C, 0x31
};
const int sizeof_ca_ecc_cert_sig = sizeof(ca_ecc_cert_der_sig);

/* ./ca-cert.der.sign,  */
const unsigned char ca_cert_der_sig[] =
{
        0x78, 0xA1, 0x30, 0x91, 0xC7, 0x12, 0xA0, 0x6B, 0x48, 0xFC,
        0x2B, 0x67, 0xF5, 0x00, 0x0D, 0x41, 0x64, 0x45, 0x20, 0xEF,
        0x14, 0xD4, 0x60, 0x5A, 0x0C, 0x7D, 0xBA, 0x16, 0x46, 0x6C,
        0x52, 0x3E, 0x8D, 0x15, 0x8C, 0xAB, 0x4D, 0x2F, 0x7E, 0x34,
        0xB9, 0x92, 0xFF, 0xFB, 0x6F, 0xCE, 0x7B, 0x15, 0xF0, 0xB7,
        0x1C, 0xFA, 0x6C, 0x06, 0x7A, 0x15, 0xC4, 0xAB, 0xA2, 0x8B,
        0xCB, 0x48, 0x6D, 0x25, 0x2F, 0xB3, 0xF0, 0xA1, 0xAB, 0xFD,
        0x53, 0xA9, 0x69, 0xC7, 0x33, 0xC3, 0x87, 0x48, 0xEE, 0x27,
        0x01, 0x22, 0xC0, 0x1B, 0x69, 0x96, 0x1B, 0x2D, 0xD2, 0x92,
        0x0B, 0xCC, 0x29, 0xD8, 0x17, 0x0E, 0x2C, 0x20, 0x95, 0xAC,
        0xE3, 0xE6, 0xF6, 0x9C, 0xE7, 0xBE, 0x0F, 0xF0, 0xD8, 0xBE,
        0xCF, 0x44, 0xBF, 0x34, 0x26, 0x7D, 0x30, 0xEA, 0x8D, 0xB9,
        0xB4, 0xB0, 0x18, 0xF1, 0x19, 0x1A, 0x19, 0xD9, 0xF0, 0x9D,
        0x72, 0xA6, 0x33, 0x9A, 0xA6, 0xC6, 0x74, 0xA9, 0x01, 0xE3,
        0xFF, 0x60, 0xFC, 0x6D, 0x0B, 0x4C, 0x5D, 0x52, 0x4D, 0xED,
        0x6C, 0xCC, 0xB9, 0x8D, 0x7B, 0x44, 0x3A, 0x1A, 0xD5, 0x8F,
        0x75, 0xAA, 0x6B, 0xEC, 0xBB, 0x94, 0x5D, 0xA3, 0x9D, 0x33,
        0x50, 0x1B, 0xBD, 0x04, 0x23, 0x05, 0x65, 0xA4, 0x5F, 0x21,
        0xDD, 0x27, 0x3A, 0xB7, 0xE6, 0x21, 0x54, 0xA1, 0x75, 0x3C,
        0x3D, 0x0E, 0x2F, 0xF5, 0x21, 0x7F, 0x02, 0x53, 0xB7, 0x14,
        0x41, 0xEE, 0x0D, 0xCE, 0xB7, 0x48, 0xE6, 0x9A, 0x2E, 0x77,
        0x9F, 0x94, 0x94, 0x00, 0x69, 0x28, 0xB4, 0xE9, 0xB1, 0x26,
        0x2B, 0x90, 0xB9, 0xCD, 0x21, 0x05, 0xB5, 0x01, 0x37, 0x45,
        0x32, 0x96, 0x80, 0xC3, 0x5A, 0xF1, 0x60, 0x9B, 0x97, 0x0D,
        0x58, 0x63, 0x84, 0xB0, 0xF9, 0xCA, 0xBB, 0x97, 0x53, 0xA4,
        0xC6, 0xE5, 0x6F, 0x59, 0x37, 0x81
};
const int sizeof_ca_cert_sig = sizeof(ca_cert_der_sig);
/* ./client-cert.der.sign,  */
const unsigned char client_cert_der_sign[] =
{
		0x81, 0x89, 0xC5, 0xC6, 0x25, 0xE3, 0xD5, 0x3D, 0xEE, 0xE0,
		0xBC, 0xDF, 0xF0, 0xA4, 0xCE, 0xAC, 0xF8, 0x26, 0xB1, 0x41,
		0xE3, 0x8C, 0x50, 0xE8, 0xCA, 0x4A, 0xA7, 0xDB, 0x5F, 0xED,
		0x61, 0x31, 0xFD, 0x13, 0xC7, 0x04, 0x25, 0x4A, 0x2D, 0x77,
		0xE8, 0xA0, 0xB3, 0xA5, 0x5D, 0x54, 0x70, 0xF9, 0x76, 0xC9,
		0x26, 0x32, 0x84, 0x04, 0xEC, 0xEF, 0x39, 0x48, 0x8D, 0xB1,
		0xDC, 0xA7, 0x71, 0xC2, 0x69, 0xC6, 0x99, 0x16, 0xB2, 0x06,
		0xBD, 0xA7, 0x7C, 0x66, 0x35, 0x2D, 0x9A, 0xFB, 0xDA, 0xAF,
		0xAA, 0xF7, 0x5A, 0x2E, 0x7C, 0x74, 0x3C, 0x53, 0xBC, 0x59,
		0x5A, 0xF6, 0x1A, 0x0E, 0x2F, 0x9A, 0xA6, 0x9B, 0x3C, 0x06,
		0x88, 0x77, 0x38, 0x7A, 0x02, 0xC9, 0x89, 0x03, 0x5B, 0xF9,
		0xE7, 0xF2, 0xFD, 0x2B, 0x63, 0x94, 0x92, 0x8D, 0xBB, 0x9D,
		0x71, 0x17, 0xB6, 0xBF, 0xA4, 0x68, 0x51, 0xF4, 0x98, 0xAC,
		0xD2, 0x57, 0x6D, 0xC0, 0xBD, 0xE9, 0xC1, 0xE5, 0x4D, 0xD6,
		0xFF, 0xC8, 0xDF, 0x7A, 0x4F, 0x97, 0x5D, 0x46, 0x3A, 0x0A,
		0x38, 0xE8, 0x0C, 0x99, 0xE7, 0x97, 0xE7, 0x3F, 0xFE, 0xC8,
		0x6A, 0x93, 0x95, 0xD2, 0x32, 0xB1, 0x01, 0x00, 0x1C, 0x9A,
		0xCE, 0x5F, 0x2B, 0xA8, 0xB1, 0xC7, 0xDC, 0x1B, 0x04, 0x9F,
		0x58, 0x03, 0x57, 0x19, 0x9A, 0xDB, 0x58, 0x33, 0xBD, 0x9D,
		0x3E, 0xA0, 0x3D, 0x9A, 0x00, 0xA6, 0xE9, 0x2E, 0xCD, 0x45,
		0x97, 0xC1, 0xDF, 0xCF, 0xAF, 0x8A, 0x93, 0x52, 0xAA, 0x65,
		0x1C, 0xC2, 0x3C, 0xDD, 0xE1, 0xED, 0x4B, 0x8A, 0x05, 0x5A,
		0xBE, 0x84, 0xEE, 0xDF, 0xC0, 0x96, 0xD2, 0x5A, 0x60, 0x32,
		0xDF, 0xC9, 0x01, 0x7C, 0x83, 0x27, 0x2B, 0x4B, 0x18, 0x18,
		0x9F, 0x58, 0xE4, 0xF0, 0x0C, 0x36, 0xC1, 0xB4, 0x08, 0x70,
		0xFB, 0xDC, 0xCB, 0x70, 0x61, 0xAC
};
const int sizeof_client_cert_der_sign = sizeof(client_cert_der_sign);

uint32_t s_inst1[R_TSIP_SINST_WORD_SIZE] = { 0 };
uint32_t s_inst2[R_TSIP_SINST2_WORD_SIZE]= { 0 };
#endif
