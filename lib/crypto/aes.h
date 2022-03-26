/*
 * Copyright (c) 2003-2004 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* $Id$ */

#ifndef LIB_CRYPTO_AES_H
#define LIB_CRYPTO_AES_H 1

#define SAMBA_RIJNDAEL 1
#define SAMBA_AES_CBC_ENCRYPT 1
#define SAMBA_AES_CFB8_ENCRYPT 1
#define SAMBA_AES_BLOCK_XOR 1

/* symbol renaming */
#define AES_set_encrypt_key samba_AES_set_encrypt_key
#define AES_set_decrypt_key samba_AES_decrypt_key
#define AES_encrypt samba_AES_encrypt
#define AES_decrypt samba_AES_decrypt
#define AES_cbc_encrypt samba_AES_cbc_encrypt
#define AES_cfb8_encrypt samba_AES_cfb8_encrypt

/*
 *
 */

#define AES_BLOCK_SIZE 16
#define AES_MAXNR 14

#define AES_ENCRYPT 1
#define AES_DECRYPT 0

typedef struct aes_key {
    uint32_t key[(AES_MAXNR+1)*4];
    int rounds;
} AES_KEY;

#ifdef __cplusplus
extern "C" {
#endif

int AES_set_encrypt_key(const unsigned char *, const int, AES_KEY *);
int AES_set_decrypt_key(const unsigned char *, const int, AES_KEY *);

void AES_encrypt(const unsigned char *, unsigned char *, const AES_KEY *);
void AES_decrypt(const unsigned char *, unsigned char *, const AES_KEY *);

void AES_cbc_encrypt(const unsigned char *, unsigned char *,
		     const unsigned long, const AES_KEY *,
		     unsigned char *, int);

void AES_cfb8_encrypt(const unsigned char *in, unsigned char *out,
		      unsigned long size, const AES_KEY *key,
		      unsigned char *iv, int forward_encrypt);

#define aes_cfb8_encrypt(in, out, size, key, iv, forward_encrypt) \
	AES_cfb8_encrypt(in, out, size, key, iv, forward_encrypt)

#ifdef  __cplusplus
}
#endif

#ifdef SAMBA_AES_BLOCK_XOR
static inline void aes_block_xor(const uint8_t in1[AES_BLOCK_SIZE],
				 const uint8_t in2[AES_BLOCK_SIZE],
				 uint8_t out[AES_BLOCK_SIZE])
{
#define __IS_ALIGN8(p) ((((uintptr_t)(p)) & 0x7) == 0)
#define __IS_ALIGNED(a,b,c) __IS_ALIGN8(\
		((uintptr_t)(a)) | \
		((uintptr_t)(b)) | \
		((uintptr_t)(c)))
	/* If everything is aligned we can optimize */
	if (likely(__IS_ALIGNED(in1, in2, out))) {
#define __RO64(p) ((const uint64_t *)(p))
#define __RW64(p) ((uint64_t *)(p))
		__RW64(out)[0] = __RO64(in1)[0] ^ __RO64(in2)[0];
		__RW64(out)[1] = __RO64(in1)[1] ^ __RO64(in2)[1];
	} else {
		uint64_t i1[2];
		uint64_t i2[2];
		uint64_t o[2];

		memcpy(i1, in1, AES_BLOCK_SIZE);
		memcpy(i2, in2, AES_BLOCK_SIZE);
		o[0] = i1[0] ^ i2[0];
		o[1] = i1[1] ^ i2[1];
		memcpy(out, o, AES_BLOCK_SIZE);
	}
}
#endif /* SAMBA_AES_BLOCK_XOR */

static inline void aes_block_lshift(const uint8_t in[AES_BLOCK_SIZE],
				    uint8_t out[AES_BLOCK_SIZE])
{
	static const struct aes_block_lshift_entry {
		uint8_t lshift;
		uint8_t overflow;
	} aes_block_lshift_table[UINT8_MAX+1] = {
		[0x00] = { .lshift = 0x00, .overflow = 0x00 },
		[0x01] = { .lshift = 0x02, .overflow = 0x00 },
		[0x02] = { .lshift = 0x04, .overflow = 0x00 },
		[0x03] = { .lshift = 0x06, .overflow = 0x00 },
		[0x04] = { .lshift = 0x08, .overflow = 0x00 },
		[0x05] = { .lshift = 0x0a, .overflow = 0x00 },
		[0x06] = { .lshift = 0x0c, .overflow = 0x00 },
		[0x07] = { .lshift = 0x0e, .overflow = 0x00 },
		[0x08] = { .lshift = 0x10, .overflow = 0x00 },
		[0x09] = { .lshift = 0x12, .overflow = 0x00 },
		[0x0a] = { .lshift = 0x14, .overflow = 0x00 },
		[0x0b] = { .lshift = 0x16, .overflow = 0x00 },
		[0x0c] = { .lshift = 0x18, .overflow = 0x00 },
		[0x0d] = { .lshift = 0x1a, .overflow = 0x00 },
		[0x0e] = { .lshift = 0x1c, .overflow = 0x00 },
		[0x0f] = { .lshift = 0x1e, .overflow = 0x00 },
		[0x10] = { .lshift = 0x20, .overflow = 0x00 },
		[0x11] = { .lshift = 0x22, .overflow = 0x00 },
		[0x12] = { .lshift = 0x24, .overflow = 0x00 },
		[0x13] = { .lshift = 0x26, .overflow = 0x00 },
		[0x14] = { .lshift = 0x28, .overflow = 0x00 },
		[0x15] = { .lshift = 0x2a, .overflow = 0x00 },
		[0x16] = { .lshift = 0x2c, .overflow = 0x00 },
		[0x17] = { .lshift = 0x2e, .overflow = 0x00 },
		[0x18] = { .lshift = 0x30, .overflow = 0x00 },
		[0x19] = { .lshift = 0x32, .overflow = 0x00 },
		[0x1a] = { .lshift = 0x34, .overflow = 0x00 },
		[0x1b] = { .lshift = 0x36, .overflow = 0x00 },
		[0x1c] = { .lshift = 0x38, .overflow = 0x00 },
		[0x1d] = { .lshift = 0x3a, .overflow = 0x00 },
		[0x1e] = { .lshift = 0x3c, .overflow = 0x00 },
		[0x1f] = { .lshift = 0x3e, .overflow = 0x00 },
		[0x20] = { .lshift = 0x40, .overflow = 0x00 },
		[0x21] = { .lshift = 0x42, .overflow = 0x00 },
		[0x22] = { .lshift = 0x44, .overflow = 0x00 },
		[0x23] = { .lshift = 0x46, .overflow = 0x00 },
		[0x24] = { .lshift = 0x48, .overflow = 0x00 },
		[0x25] = { .lshift = 0x4a, .overflow = 0x00 },
		[0x26] = { .lshift = 0x4c, .overflow = 0x00 },
		[0x27] = { .lshift = 0x4e, .overflow = 0x00 },
		[0x28] = { .lshift = 0x50, .overflow = 0x00 },
		[0x29] = { .lshift = 0x52, .overflow = 0x00 },
		[0x2a] = { .lshift = 0x54, .overflow = 0x00 },
		[0x2b] = { .lshift = 0x56, .overflow = 0x00 },
		[0x2c] = { .lshift = 0x58, .overflow = 0x00 },
		[0x2d] = { .lshift = 0x5a, .overflow = 0x00 },
		[0x2e] = { .lshift = 0x5c, .overflow = 0x00 },
		[0x2f] = { .lshift = 0x5e, .overflow = 0x00 },
		[0x30] = { .lshift = 0x60, .overflow = 0x00 },
		[0x31] = { .lshift = 0x62, .overflow = 0x00 },
		[0x32] = { .lshift = 0x64, .overflow = 0x00 },
		[0x33] = { .lshift = 0x66, .overflow = 0x00 },
		[0x34] = { .lshift = 0x68, .overflow = 0x00 },
		[0x35] = { .lshift = 0x6a, .overflow = 0x00 },
		[0x36] = { .lshift = 0x6c, .overflow = 0x00 },
		[0x37] = { .lshift = 0x6e, .overflow = 0x00 },
		[0x38] = { .lshift = 0x70, .overflow = 0x00 },
		[0x39] = { .lshift = 0x72, .overflow = 0x00 },
		[0x3a] = { .lshift = 0x74, .overflow = 0x00 },
		[0x3b] = { .lshift = 0x76, .overflow = 0x00 },
		[0x3c] = { .lshift = 0x78, .overflow = 0x00 },
		[0x3d] = { .lshift = 0x7a, .overflow = 0x00 },
		[0x3e] = { .lshift = 0x7c, .overflow = 0x00 },
		[0x3f] = { .lshift = 0x7e, .overflow = 0x00 },
		[0x40] = { .lshift = 0x80, .overflow = 0x00 },
		[0x41] = { .lshift = 0x82, .overflow = 0x00 },
		[0x42] = { .lshift = 0x84, .overflow = 0x00 },
		[0x43] = { .lshift = 0x86, .overflow = 0x00 },
		[0x44] = { .lshift = 0x88, .overflow = 0x00 },
		[0x45] = { .lshift = 0x8a, .overflow = 0x00 },
		[0x46] = { .lshift = 0x8c, .overflow = 0x00 },
		[0x47] = { .lshift = 0x8e, .overflow = 0x00 },
		[0x48] = { .lshift = 0x90, .overflow = 0x00 },
		[0x49] = { .lshift = 0x92, .overflow = 0x00 },
		[0x4a] = { .lshift = 0x94, .overflow = 0x00 },
		[0x4b] = { .lshift = 0x96, .overflow = 0x00 },
		[0x4c] = { .lshift = 0x98, .overflow = 0x00 },
		[0x4d] = { .lshift = 0x9a, .overflow = 0x00 },
		[0x4e] = { .lshift = 0x9c, .overflow = 0x00 },
		[0x4f] = { .lshift = 0x9e, .overflow = 0x00 },
		[0x50] = { .lshift = 0xa0, .overflow = 0x00 },
		[0x51] = { .lshift = 0xa2, .overflow = 0x00 },
		[0x52] = { .lshift = 0xa4, .overflow = 0x00 },
		[0x53] = { .lshift = 0xa6, .overflow = 0x00 },
		[0x54] = { .lshift = 0xa8, .overflow = 0x00 },
		[0x55] = { .lshift = 0xaa, .overflow = 0x00 },
		[0x56] = { .lshift = 0xac, .overflow = 0x00 },
		[0x57] = { .lshift = 0xae, .overflow = 0x00 },
		[0x58] = { .lshift = 0xb0, .overflow = 0x00 },
		[0x59] = { .lshift = 0xb2, .overflow = 0x00 },
		[0x5a] = { .lshift = 0xb4, .overflow = 0x00 },
		[0x5b] = { .lshift = 0xb6, .overflow = 0x00 },
		[0x5c] = { .lshift = 0xb8, .overflow = 0x00 },
		[0x5d] = { .lshift = 0xba, .overflow = 0x00 },
		[0x5e] = { .lshift = 0xbc, .overflow = 0x00 },
		[0x5f] = { .lshift = 0xbe, .overflow = 0x00 },
		[0x60] = { .lshift = 0xc0, .overflow = 0x00 },
		[0x61] = { .lshift = 0xc2, .overflow = 0x00 },
		[0x62] = { .lshift = 0xc4, .overflow = 0x00 },
		[0x63] = { .lshift = 0xc6, .overflow = 0x00 },
		[0x64] = { .lshift = 0xc8, .overflow = 0x00 },
		[0x65] = { .lshift = 0xca, .overflow = 0x00 },
		[0x66] = { .lshift = 0xcc, .overflow = 0x00 },
		[0x67] = { .lshift = 0xce, .overflow = 0x00 },
		[0x68] = { .lshift = 0xd0, .overflow = 0x00 },
		[0x69] = { .lshift = 0xd2, .overflow = 0x00 },
		[0x6a] = { .lshift = 0xd4, .overflow = 0x00 },
		[0x6b] = { .lshift = 0xd6, .overflow = 0x00 },
		[0x6c] = { .lshift = 0xd8, .overflow = 0x00 },
		[0x6d] = { .lshift = 0xda, .overflow = 0x00 },
		[0x6e] = { .lshift = 0xdc, .overflow = 0x00 },
		[0x6f] = { .lshift = 0xde, .overflow = 0x00 },
		[0x70] = { .lshift = 0xe0, .overflow = 0x00 },
		[0x71] = { .lshift = 0xe2, .overflow = 0x00 },
		[0x72] = { .lshift = 0xe4, .overflow = 0x00 },
		[0x73] = { .lshift = 0xe6, .overflow = 0x00 },
		[0x74] = { .lshift = 0xe8, .overflow = 0x00 },
		[0x75] = { .lshift = 0xea, .overflow = 0x00 },
		[0x76] = { .lshift = 0xec, .overflow = 0x00 },
		[0x77] = { .lshift = 0xee, .overflow = 0x00 },
		[0x78] = { .lshift = 0xf0, .overflow = 0x00 },
		[0x79] = { .lshift = 0xf2, .overflow = 0x00 },
		[0x7a] = { .lshift = 0xf4, .overflow = 0x00 },
		[0x7b] = { .lshift = 0xf6, .overflow = 0x00 },
		[0x7c] = { .lshift = 0xf8, .overflow = 0x00 },
		[0x7d] = { .lshift = 0xfa, .overflow = 0x00 },
		[0x7e] = { .lshift = 0xfc, .overflow = 0x00 },
		[0x7f] = { .lshift = 0xfe, .overflow = 0x00 },
		[0x80] = { .lshift = 0x00, .overflow = 0x01 },
		[0x81] = { .lshift = 0x02, .overflow = 0x01 },
		[0x82] = { .lshift = 0x04, .overflow = 0x01 },
		[0x83] = { .lshift = 0x06, .overflow = 0x01 },
		[0x84] = { .lshift = 0x08, .overflow = 0x01 },
		[0x85] = { .lshift = 0x0a, .overflow = 0x01 },
		[0x86] = { .lshift = 0x0c, .overflow = 0x01 },
		[0x87] = { .lshift = 0x0e, .overflow = 0x01 },
		[0x88] = { .lshift = 0x10, .overflow = 0x01 },
		[0x89] = { .lshift = 0x12, .overflow = 0x01 },
		[0x8a] = { .lshift = 0x14, .overflow = 0x01 },
		[0x8b] = { .lshift = 0x16, .overflow = 0x01 },
		[0x8c] = { .lshift = 0x18, .overflow = 0x01 },
		[0x8d] = { .lshift = 0x1a, .overflow = 0x01 },
		[0x8e] = { .lshift = 0x1c, .overflow = 0x01 },
		[0x8f] = { .lshift = 0x1e, .overflow = 0x01 },
		[0x90] = { .lshift = 0x20, .overflow = 0x01 },
		[0x91] = { .lshift = 0x22, .overflow = 0x01 },
		[0x92] = { .lshift = 0x24, .overflow = 0x01 },
		[0x93] = { .lshift = 0x26, .overflow = 0x01 },
		[0x94] = { .lshift = 0x28, .overflow = 0x01 },
		[0x95] = { .lshift = 0x2a, .overflow = 0x01 },
		[0x96] = { .lshift = 0x2c, .overflow = 0x01 },
		[0x97] = { .lshift = 0x2e, .overflow = 0x01 },
		[0x98] = { .lshift = 0x30, .overflow = 0x01 },
		[0x99] = { .lshift = 0x32, .overflow = 0x01 },
		[0x9a] = { .lshift = 0x34, .overflow = 0x01 },
		[0x9b] = { .lshift = 0x36, .overflow = 0x01 },
		[0x9c] = { .lshift = 0x38, .overflow = 0x01 },
		[0x9d] = { .lshift = 0x3a, .overflow = 0x01 },
		[0x9e] = { .lshift = 0x3c, .overflow = 0x01 },
		[0x9f] = { .lshift = 0x3e, .overflow = 0x01 },
		[0xa0] = { .lshift = 0x40, .overflow = 0x01 },
		[0xa1] = { .lshift = 0x42, .overflow = 0x01 },
		[0xa2] = { .lshift = 0x44, .overflow = 0x01 },
		[0xa3] = { .lshift = 0x46, .overflow = 0x01 },
		[0xa4] = { .lshift = 0x48, .overflow = 0x01 },
		[0xa5] = { .lshift = 0x4a, .overflow = 0x01 },
		[0xa6] = { .lshift = 0x4c, .overflow = 0x01 },
		[0xa7] = { .lshift = 0x4e, .overflow = 0x01 },
		[0xa8] = { .lshift = 0x50, .overflow = 0x01 },
		[0xa9] = { .lshift = 0x52, .overflow = 0x01 },
		[0xaa] = { .lshift = 0x54, .overflow = 0x01 },
		[0xab] = { .lshift = 0x56, .overflow = 0x01 },
		[0xac] = { .lshift = 0x58, .overflow = 0x01 },
		[0xad] = { .lshift = 0x5a, .overflow = 0x01 },
		[0xae] = { .lshift = 0x5c, .overflow = 0x01 },
		[0xaf] = { .lshift = 0x5e, .overflow = 0x01 },
		[0xb0] = { .lshift = 0x60, .overflow = 0x01 },
		[0xb1] = { .lshift = 0x62, .overflow = 0x01 },
		[0xb2] = { .lshift = 0x64, .overflow = 0x01 },
		[0xb3] = { .lshift = 0x66, .overflow = 0x01 },
		[0xb4] = { .lshift = 0x68, .overflow = 0x01 },
		[0xb5] = { .lshift = 0x6a, .overflow = 0x01 },
		[0xb6] = { .lshift = 0x6c, .overflow = 0x01 },
		[0xb7] = { .lshift = 0x6e, .overflow = 0x01 },
		[0xb8] = { .lshift = 0x70, .overflow = 0x01 },
		[0xb9] = { .lshift = 0x72, .overflow = 0x01 },
		[0xba] = { .lshift = 0x74, .overflow = 0x01 },
		[0xbb] = { .lshift = 0x76, .overflow = 0x01 },
		[0xbc] = { .lshift = 0x78, .overflow = 0x01 },
		[0xbd] = { .lshift = 0x7a, .overflow = 0x01 },
		[0xbe] = { .lshift = 0x7c, .overflow = 0x01 },
		[0xbf] = { .lshift = 0x7e, .overflow = 0x01 },
		[0xc0] = { .lshift = 0x80, .overflow = 0x01 },
		[0xc1] = { .lshift = 0x82, .overflow = 0x01 },
		[0xc2] = { .lshift = 0x84, .overflow = 0x01 },
		[0xc3] = { .lshift = 0x86, .overflow = 0x01 },
		[0xc4] = { .lshift = 0x88, .overflow = 0x01 },
		[0xc5] = { .lshift = 0x8a, .overflow = 0x01 },
		[0xc6] = { .lshift = 0x8c, .overflow = 0x01 },
		[0xc7] = { .lshift = 0x8e, .overflow = 0x01 },
		[0xc8] = { .lshift = 0x90, .overflow = 0x01 },
		[0xc9] = { .lshift = 0x92, .overflow = 0x01 },
		[0xca] = { .lshift = 0x94, .overflow = 0x01 },
		[0xcb] = { .lshift = 0x96, .overflow = 0x01 },
		[0xcc] = { .lshift = 0x98, .overflow = 0x01 },
		[0xcd] = { .lshift = 0x9a, .overflow = 0x01 },
		[0xce] = { .lshift = 0x9c, .overflow = 0x01 },
		[0xcf] = { .lshift = 0x9e, .overflow = 0x01 },
		[0xd0] = { .lshift = 0xa0, .overflow = 0x01 },
		[0xd1] = { .lshift = 0xa2, .overflow = 0x01 },
		[0xd2] = { .lshift = 0xa4, .overflow = 0x01 },
		[0xd3] = { .lshift = 0xa6, .overflow = 0x01 },
		[0xd4] = { .lshift = 0xa8, .overflow = 0x01 },
		[0xd5] = { .lshift = 0xaa, .overflow = 0x01 },
		[0xd6] = { .lshift = 0xac, .overflow = 0x01 },
		[0xd7] = { .lshift = 0xae, .overflow = 0x01 },
		[0xd8] = { .lshift = 0xb0, .overflow = 0x01 },
		[0xd9] = { .lshift = 0xb2, .overflow = 0x01 },
		[0xda] = { .lshift = 0xb4, .overflow = 0x01 },
		[0xdb] = { .lshift = 0xb6, .overflow = 0x01 },
		[0xdc] = { .lshift = 0xb8, .overflow = 0x01 },
		[0xdd] = { .lshift = 0xba, .overflow = 0x01 },
		[0xde] = { .lshift = 0xbc, .overflow = 0x01 },
		[0xdf] = { .lshift = 0xbe, .overflow = 0x01 },
		[0xe0] = { .lshift = 0xc0, .overflow = 0x01 },
		[0xe1] = { .lshift = 0xc2, .overflow = 0x01 },
		[0xe2] = { .lshift = 0xc4, .overflow = 0x01 },
		[0xe3] = { .lshift = 0xc6, .overflow = 0x01 },
		[0xe4] = { .lshift = 0xc8, .overflow = 0x01 },
		[0xe5] = { .lshift = 0xca, .overflow = 0x01 },
		[0xe6] = { .lshift = 0xcc, .overflow = 0x01 },
		[0xe7] = { .lshift = 0xce, .overflow = 0x01 },
		[0xe8] = { .lshift = 0xd0, .overflow = 0x01 },
		[0xe9] = { .lshift = 0xd2, .overflow = 0x01 },
		[0xea] = { .lshift = 0xd4, .overflow = 0x01 },
		[0xeb] = { .lshift = 0xd6, .overflow = 0x01 },
		[0xec] = { .lshift = 0xd8, .overflow = 0x01 },
		[0xed] = { .lshift = 0xda, .overflow = 0x01 },
		[0xee] = { .lshift = 0xdc, .overflow = 0x01 },
		[0xef] = { .lshift = 0xde, .overflow = 0x01 },
		[0xf0] = { .lshift = 0xe0, .overflow = 0x01 },
		[0xf1] = { .lshift = 0xe2, .overflow = 0x01 },
		[0xf2] = { .lshift = 0xe4, .overflow = 0x01 },
		[0xf3] = { .lshift = 0xe6, .overflow = 0x01 },
		[0xf4] = { .lshift = 0xe8, .overflow = 0x01 },
		[0xf5] = { .lshift = 0xea, .overflow = 0x01 },
		[0xf6] = { .lshift = 0xec, .overflow = 0x01 },
		[0xf7] = { .lshift = 0xee, .overflow = 0x01 },
		[0xf8] = { .lshift = 0xf0, .overflow = 0x01 },
		[0xf9] = { .lshift = 0xf2, .overflow = 0x01 },
		[0xfa] = { .lshift = 0xf4, .overflow = 0x01 },
		[0xfb] = { .lshift = 0xf6, .overflow = 0x01 },
		[0xfc] = { .lshift = 0xf8, .overflow = 0x01 },
		[0xfd] = { .lshift = 0xfa, .overflow = 0x01 },
		[0xfe] = { .lshift = 0xfc, .overflow = 0x01 },
		[0xff] = { .lshift = 0xfe, .overflow = 0x01 },
	};
	int8_t i;
	uint8_t overflow = 0;

	for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
		const struct aes_block_lshift_entry *e = &aes_block_lshift_table[in[i]];
		out[i] = e->lshift | overflow;
		overflow = e->overflow;
	}
}

static inline void aes_block_rshift(const uint8_t in[AES_BLOCK_SIZE],
				    uint8_t out[AES_BLOCK_SIZE])
{
	static const struct aes_block_rshift_entry {
		uint8_t rshift;
		uint8_t overflow;
	} aes_block_rshift_table[UINT8_MAX+1] = {
		[0x00] = { .rshift = 0x00, .overflow = 0x00 },
		[0x01] = { .rshift = 0x00, .overflow = 0x80 },
		[0x02] = { .rshift = 0x01, .overflow = 0x00 },
		[0x03] = { .rshift = 0x01, .overflow = 0x80 },
		[0x04] = { .rshift = 0x02, .overflow = 0x00 },
		[0x05] = { .rshift = 0x02, .overflow = 0x80 },
		[0x06] = { .rshift = 0x03, .overflow = 0x00 },
		[0x07] = { .rshift = 0x03, .overflow = 0x80 },
		[0x08] = { .rshift = 0x04, .overflow = 0x00 },
		[0x09] = { .rshift = 0x04, .overflow = 0x80 },
		[0x0a] = { .rshift = 0x05, .overflow = 0x00 },
		[0x0b] = { .rshift = 0x05, .overflow = 0x80 },
		[0x0c] = { .rshift = 0x06, .overflow = 0x00 },
		[0x0d] = { .rshift = 0x06, .overflow = 0x80 },
		[0x0e] = { .rshift = 0x07, .overflow = 0x00 },
		[0x0f] = { .rshift = 0x07, .overflow = 0x80 },
		[0x10] = { .rshift = 0x08, .overflow = 0x00 },
		[0x11] = { .rshift = 0x08, .overflow = 0x80 },
		[0x12] = { .rshift = 0x09, .overflow = 0x00 },
		[0x13] = { .rshift = 0x09, .overflow = 0x80 },
		[0x14] = { .rshift = 0x0a, .overflow = 0x00 },
		[0x15] = { .rshift = 0x0a, .overflow = 0x80 },
		[0x16] = { .rshift = 0x0b, .overflow = 0x00 },
		[0x17] = { .rshift = 0x0b, .overflow = 0x80 },
		[0x18] = { .rshift = 0x0c, .overflow = 0x00 },
		[0x19] = { .rshift = 0x0c, .overflow = 0x80 },
		[0x1a] = { .rshift = 0x0d, .overflow = 0x00 },
		[0x1b] = { .rshift = 0x0d, .overflow = 0x80 },
		[0x1c] = { .rshift = 0x0e, .overflow = 0x00 },
		[0x1d] = { .rshift = 0x0e, .overflow = 0x80 },
		[0x1e] = { .rshift = 0x0f, .overflow = 0x00 },
		[0x1f] = { .rshift = 0x0f, .overflow = 0x80 },
		[0x20] = { .rshift = 0x10, .overflow = 0x00 },
		[0x21] = { .rshift = 0x10, .overflow = 0x80 },
		[0x22] = { .rshift = 0x11, .overflow = 0x00 },
		[0x23] = { .rshift = 0x11, .overflow = 0x80 },
		[0x24] = { .rshift = 0x12, .overflow = 0x00 },
		[0x25] = { .rshift = 0x12, .overflow = 0x80 },
		[0x26] = { .rshift = 0x13, .overflow = 0x00 },
		[0x27] = { .rshift = 0x13, .overflow = 0x80 },
		[0x28] = { .rshift = 0x14, .overflow = 0x00 },
		[0x29] = { .rshift = 0x14, .overflow = 0x80 },
		[0x2a] = { .rshift = 0x15, .overflow = 0x00 },
		[0x2b] = { .rshift = 0x15, .overflow = 0x80 },
		[0x2c] = { .rshift = 0x16, .overflow = 0x00 },
		[0x2d] = { .rshift = 0x16, .overflow = 0x80 },
		[0x2e] = { .rshift = 0x17, .overflow = 0x00 },
		[0x2f] = { .rshift = 0x17, .overflow = 0x80 },
		[0x30] = { .rshift = 0x18, .overflow = 0x00 },
		[0x31] = { .rshift = 0x18, .overflow = 0x80 },
		[0x32] = { .rshift = 0x19, .overflow = 0x00 },
		[0x33] = { .rshift = 0x19, .overflow = 0x80 },
		[0x34] = { .rshift = 0x1a, .overflow = 0x00 },
		[0x35] = { .rshift = 0x1a, .overflow = 0x80 },
		[0x36] = { .rshift = 0x1b, .overflow = 0x00 },
		[0x37] = { .rshift = 0x1b, .overflow = 0x80 },
		[0x38] = { .rshift = 0x1c, .overflow = 0x00 },
		[0x39] = { .rshift = 0x1c, .overflow = 0x80 },
		[0x3a] = { .rshift = 0x1d, .overflow = 0x00 },
		[0x3b] = { .rshift = 0x1d, .overflow = 0x80 },
		[0x3c] = { .rshift = 0x1e, .overflow = 0x00 },
		[0x3d] = { .rshift = 0x1e, .overflow = 0x80 },
		[0x3e] = { .rshift = 0x1f, .overflow = 0x00 },
		[0x3f] = { .rshift = 0x1f, .overflow = 0x80 },
		[0x40] = { .rshift = 0x20, .overflow = 0x00 },
		[0x41] = { .rshift = 0x20, .overflow = 0x80 },
		[0x42] = { .rshift = 0x21, .overflow = 0x00 },
		[0x43] = { .rshift = 0x21, .overflow = 0x80 },
		[0x44] = { .rshift = 0x22, .overflow = 0x00 },
		[0x45] = { .rshift = 0x22, .overflow = 0x80 },
		[0x46] = { .rshift = 0x23, .overflow = 0x00 },
		[0x47] = { .rshift = 0x23, .overflow = 0x80 },
		[0x48] = { .rshift = 0x24, .overflow = 0x00 },
		[0x49] = { .rshift = 0x24, .overflow = 0x80 },
		[0x4a] = { .rshift = 0x25, .overflow = 0x00 },
		[0x4b] = { .rshift = 0x25, .overflow = 0x80 },
		[0x4c] = { .rshift = 0x26, .overflow = 0x00 },
		[0x4d] = { .rshift = 0x26, .overflow = 0x80 },
		[0x4e] = { .rshift = 0x27, .overflow = 0x00 },
		[0x4f] = { .rshift = 0x27, .overflow = 0x80 },
		[0x50] = { .rshift = 0x28, .overflow = 0x00 },
		[0x51] = { .rshift = 0x28, .overflow = 0x80 },
		[0x52] = { .rshift = 0x29, .overflow = 0x00 },
		[0x53] = { .rshift = 0x29, .overflow = 0x80 },
		[0x54] = { .rshift = 0x2a, .overflow = 0x00 },
		[0x55] = { .rshift = 0x2a, .overflow = 0x80 },
		[0x56] = { .rshift = 0x2b, .overflow = 0x00 },
		[0x57] = { .rshift = 0x2b, .overflow = 0x80 },
		[0x58] = { .rshift = 0x2c, .overflow = 0x00 },
		[0x59] = { .rshift = 0x2c, .overflow = 0x80 },
		[0x5a] = { .rshift = 0x2d, .overflow = 0x00 },
		[0x5b] = { .rshift = 0x2d, .overflow = 0x80 },
		[0x5c] = { .rshift = 0x2e, .overflow = 0x00 },
		[0x5d] = { .rshift = 0x2e, .overflow = 0x80 },
		[0x5e] = { .rshift = 0x2f, .overflow = 0x00 },
		[0x5f] = { .rshift = 0x2f, .overflow = 0x80 },
		[0x60] = { .rshift = 0x30, .overflow = 0x00 },
		[0x61] = { .rshift = 0x30, .overflow = 0x80 },
		[0x62] = { .rshift = 0x31, .overflow = 0x00 },
		[0x63] = { .rshift = 0x31, .overflow = 0x80 },
		[0x64] = { .rshift = 0x32, .overflow = 0x00 },
		[0x65] = { .rshift = 0x32, .overflow = 0x80 },
		[0x66] = { .rshift = 0x33, .overflow = 0x00 },
		[0x67] = { .rshift = 0x33, .overflow = 0x80 },
		[0x68] = { .rshift = 0x34, .overflow = 0x00 },
		[0x69] = { .rshift = 0x34, .overflow = 0x80 },
		[0x6a] = { .rshift = 0x35, .overflow = 0x00 },
		[0x6b] = { .rshift = 0x35, .overflow = 0x80 },
		[0x6c] = { .rshift = 0x36, .overflow = 0x00 },
		[0x6d] = { .rshift = 0x36, .overflow = 0x80 },
		[0x6e] = { .rshift = 0x37, .overflow = 0x00 },
		[0x6f] = { .rshift = 0x37, .overflow = 0x80 },
		[0x70] = { .rshift = 0x38, .overflow = 0x00 },
		[0x71] = { .rshift = 0x38, .overflow = 0x80 },
		[0x72] = { .rshift = 0x39, .overflow = 0x00 },
		[0x73] = { .rshift = 0x39, .overflow = 0x80 },
		[0x74] = { .rshift = 0x3a, .overflow = 0x00 },
		[0x75] = { .rshift = 0x3a, .overflow = 0x80 },
		[0x76] = { .rshift = 0x3b, .overflow = 0x00 },
		[0x77] = { .rshift = 0x3b, .overflow = 0x80 },
		[0x78] = { .rshift = 0x3c, .overflow = 0x00 },
		[0x79] = { .rshift = 0x3c, .overflow = 0x80 },
		[0x7a] = { .rshift = 0x3d, .overflow = 0x00 },
		[0x7b] = { .rshift = 0x3d, .overflow = 0x80 },
		[0x7c] = { .rshift = 0x3e, .overflow = 0x00 },
		[0x7d] = { .rshift = 0x3e, .overflow = 0x80 },
		[0x7e] = { .rshift = 0x3f, .overflow = 0x00 },
		[0x7f] = { .rshift = 0x3f, .overflow = 0x80 },
		[0x80] = { .rshift = 0x40, .overflow = 0x00 },
		[0x81] = { .rshift = 0x40, .overflow = 0x80 },
		[0x82] = { .rshift = 0x41, .overflow = 0x00 },
		[0x83] = { .rshift = 0x41, .overflow = 0x80 },
		[0x84] = { .rshift = 0x42, .overflow = 0x00 },
		[0x85] = { .rshift = 0x42, .overflow = 0x80 },
		[0x86] = { .rshift = 0x43, .overflow = 0x00 },
		[0x87] = { .rshift = 0x43, .overflow = 0x80 },
		[0x88] = { .rshift = 0x44, .overflow = 0x00 },
		[0x89] = { .rshift = 0x44, .overflow = 0x80 },
		[0x8a] = { .rshift = 0x45, .overflow = 0x00 },
		[0x8b] = { .rshift = 0x45, .overflow = 0x80 },
		[0x8c] = { .rshift = 0x46, .overflow = 0x00 },
		[0x8d] = { .rshift = 0x46, .overflow = 0x80 },
		[0x8e] = { .rshift = 0x47, .overflow = 0x00 },
		[0x8f] = { .rshift = 0x47, .overflow = 0x80 },
		[0x90] = { .rshift = 0x48, .overflow = 0x00 },
		[0x91] = { .rshift = 0x48, .overflow = 0x80 },
		[0x92] = { .rshift = 0x49, .overflow = 0x00 },
		[0x93] = { .rshift = 0x49, .overflow = 0x80 },
		[0x94] = { .rshift = 0x4a, .overflow = 0x00 },
		[0x95] = { .rshift = 0x4a, .overflow = 0x80 },
		[0x96] = { .rshift = 0x4b, .overflow = 0x00 },
		[0x97] = { .rshift = 0x4b, .overflow = 0x80 },
		[0x98] = { .rshift = 0x4c, .overflow = 0x00 },
		[0x99] = { .rshift = 0x4c, .overflow = 0x80 },
		[0x9a] = { .rshift = 0x4d, .overflow = 0x00 },
		[0x9b] = { .rshift = 0x4d, .overflow = 0x80 },
		[0x9c] = { .rshift = 0x4e, .overflow = 0x00 },
		[0x9d] = { .rshift = 0x4e, .overflow = 0x80 },
		[0x9e] = { .rshift = 0x4f, .overflow = 0x00 },
		[0x9f] = { .rshift = 0x4f, .overflow = 0x80 },
		[0xa0] = { .rshift = 0x50, .overflow = 0x00 },
		[0xa1] = { .rshift = 0x50, .overflow = 0x80 },
		[0xa2] = { .rshift = 0x51, .overflow = 0x00 },
		[0xa3] = { .rshift = 0x51, .overflow = 0x80 },
		[0xa4] = { .rshift = 0x52, .overflow = 0x00 },
		[0xa5] = { .rshift = 0x52, .overflow = 0x80 },
		[0xa6] = { .rshift = 0x53, .overflow = 0x00 },
		[0xa7] = { .rshift = 0x53, .overflow = 0x80 },
		[0xa8] = { .rshift = 0x54, .overflow = 0x00 },
		[0xa9] = { .rshift = 0x54, .overflow = 0x80 },
		[0xaa] = { .rshift = 0x55, .overflow = 0x00 },
		[0xab] = { .rshift = 0x55, .overflow = 0x80 },
		[0xac] = { .rshift = 0x56, .overflow = 0x00 },
		[0xad] = { .rshift = 0x56, .overflow = 0x80 },
		[0xae] = { .rshift = 0x57, .overflow = 0x00 },
		[0xaf] = { .rshift = 0x57, .overflow = 0x80 },
		[0xb0] = { .rshift = 0x58, .overflow = 0x00 },
		[0xb1] = { .rshift = 0x58, .overflow = 0x80 },
		[0xb2] = { .rshift = 0x59, .overflow = 0x00 },
		[0xb3] = { .rshift = 0x59, .overflow = 0x80 },
		[0xb4] = { .rshift = 0x5a, .overflow = 0x00 },
		[0xb5] = { .rshift = 0x5a, .overflow = 0x80 },
		[0xb6] = { .rshift = 0x5b, .overflow = 0x00 },
		[0xb7] = { .rshift = 0x5b, .overflow = 0x80 },
		[0xb8] = { .rshift = 0x5c, .overflow = 0x00 },
		[0xb9] = { .rshift = 0x5c, .overflow = 0x80 },
		[0xba] = { .rshift = 0x5d, .overflow = 0x00 },
		[0xbb] = { .rshift = 0x5d, .overflow = 0x80 },
		[0xbc] = { .rshift = 0x5e, .overflow = 0x00 },
		[0xbd] = { .rshift = 0x5e, .overflow = 0x80 },
		[0xbe] = { .rshift = 0x5f, .overflow = 0x00 },
		[0xbf] = { .rshift = 0x5f, .overflow = 0x80 },
		[0xc0] = { .rshift = 0x60, .overflow = 0x00 },
		[0xc1] = { .rshift = 0x60, .overflow = 0x80 },
		[0xc2] = { .rshift = 0x61, .overflow = 0x00 },
		[0xc3] = { .rshift = 0x61, .overflow = 0x80 },
		[0xc4] = { .rshift = 0x62, .overflow = 0x00 },
		[0xc5] = { .rshift = 0x62, .overflow = 0x80 },
		[0xc6] = { .rshift = 0x63, .overflow = 0x00 },
		[0xc7] = { .rshift = 0x63, .overflow = 0x80 },
		[0xc8] = { .rshift = 0x64, .overflow = 0x00 },
		[0xc9] = { .rshift = 0x64, .overflow = 0x80 },
		[0xca] = { .rshift = 0x65, .overflow = 0x00 },
		[0xcb] = { .rshift = 0x65, .overflow = 0x80 },
		[0xcc] = { .rshift = 0x66, .overflow = 0x00 },
		[0xcd] = { .rshift = 0x66, .overflow = 0x80 },
		[0xce] = { .rshift = 0x67, .overflow = 0x00 },
		[0xcf] = { .rshift = 0x67, .overflow = 0x80 },
		[0xd0] = { .rshift = 0x68, .overflow = 0x00 },
		[0xd1] = { .rshift = 0x68, .overflow = 0x80 },
		[0xd2] = { .rshift = 0x69, .overflow = 0x00 },
		[0xd3] = { .rshift = 0x69, .overflow = 0x80 },
		[0xd4] = { .rshift = 0x6a, .overflow = 0x00 },
		[0xd5] = { .rshift = 0x6a, .overflow = 0x80 },
		[0xd6] = { .rshift = 0x6b, .overflow = 0x00 },
		[0xd7] = { .rshift = 0x6b, .overflow = 0x80 },
		[0xd8] = { .rshift = 0x6c, .overflow = 0x00 },
		[0xd9] = { .rshift = 0x6c, .overflow = 0x80 },
		[0xda] = { .rshift = 0x6d, .overflow = 0x00 },
		[0xdb] = { .rshift = 0x6d, .overflow = 0x80 },
		[0xdc] = { .rshift = 0x6e, .overflow = 0x00 },
		[0xdd] = { .rshift = 0x6e, .overflow = 0x80 },
		[0xde] = { .rshift = 0x6f, .overflow = 0x00 },
		[0xdf] = { .rshift = 0x6f, .overflow = 0x80 },
		[0xe0] = { .rshift = 0x70, .overflow = 0x00 },
		[0xe1] = { .rshift = 0x70, .overflow = 0x80 },
		[0xe2] = { .rshift = 0x71, .overflow = 0x00 },
		[0xe3] = { .rshift = 0x71, .overflow = 0x80 },
		[0xe4] = { .rshift = 0x72, .overflow = 0x00 },
		[0xe5] = { .rshift = 0x72, .overflow = 0x80 },
		[0xe6] = { .rshift = 0x73, .overflow = 0x00 },
		[0xe7] = { .rshift = 0x73, .overflow = 0x80 },
		[0xe8] = { .rshift = 0x74, .overflow = 0x00 },
		[0xe9] = { .rshift = 0x74, .overflow = 0x80 },
		[0xea] = { .rshift = 0x75, .overflow = 0x00 },
		[0xeb] = { .rshift = 0x75, .overflow = 0x80 },
		[0xec] = { .rshift = 0x76, .overflow = 0x00 },
		[0xed] = { .rshift = 0x76, .overflow = 0x80 },
		[0xee] = { .rshift = 0x77, .overflow = 0x00 },
		[0xef] = { .rshift = 0x77, .overflow = 0x80 },
		[0xf0] = { .rshift = 0x78, .overflow = 0x00 },
		[0xf1] = { .rshift = 0x78, .overflow = 0x80 },
		[0xf2] = { .rshift = 0x79, .overflow = 0x00 },
		[0xf3] = { .rshift = 0x79, .overflow = 0x80 },
		[0xf4] = { .rshift = 0x7a, .overflow = 0x00 },
		[0xf5] = { .rshift = 0x7a, .overflow = 0x80 },
		[0xf6] = { .rshift = 0x7b, .overflow = 0x00 },
		[0xf7] = { .rshift = 0x7b, .overflow = 0x80 },
		[0xf8] = { .rshift = 0x7c, .overflow = 0x00 },
		[0xf9] = { .rshift = 0x7c, .overflow = 0x80 },
		[0xfa] = { .rshift = 0x7d, .overflow = 0x00 },
		[0xfb] = { .rshift = 0x7d, .overflow = 0x80 },
		[0xfc] = { .rshift = 0x7e, .overflow = 0x00 },
		[0xfd] = { .rshift = 0x7e, .overflow = 0x80 },
		[0xfe] = { .rshift = 0x7f, .overflow = 0x00 },
		[0xff] = { .rshift = 0x7f, .overflow = 0x80 },
	};
	uint8_t i;
	uint8_t overflow = 0;

	for (i = 0; i < AES_BLOCK_SIZE; i++) {
		const struct aes_block_rshift_entry *e = &aes_block_rshift_table[in[i]];
		out[i] = e->rshift | overflow;
		overflow = e->overflow;
	}
}
#endif /* LIB_CRYPTO_AES_H */
