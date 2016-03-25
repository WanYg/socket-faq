/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD4 Message-Digest Algorithm (RFC 1320).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md4
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md4.c for more information.
 */

#ifdef HAVE_OPENSSL
#include <openssl/md4.h>
#elif !defined(_MD4_H)
#define _MD4_H

#include <stdint.h>
#include <sys/types.h>

typedef struct {
	uint32_t lo, hi;
	uint32_t a, b, c, d;
	u_char buffer[64];
	uint32_t block[16];
} md4_ctx_t;

void md4_init(md4_ctx_t *ctx);
void md4_update(md4_ctx_t *ctx, const void *data, size_t size);
void md4_final(u_char *result, md4_ctx_t *ctx);

#endif
