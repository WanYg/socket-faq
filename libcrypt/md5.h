#ifndef __MD5_H_
#define __MD5_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef OPENSSL_MD5
#include <openssl/md5.h>

#define md5_init    MD5_Init
#define md5_update  MD5_Update
#define md5_final   MD5_Final

#else //!HAVE_MD5

typedef struct {
	uint64_t bytes;
	uint32_t a, b, c, d;
	u_char buffer[64];
} md5_ctx_t;

void md5_init(md5_ctx_t *ctx);
void md5_update(md5_ctx_t *ctx, const void *data, size_t size);
void md5_final(u_char result[16], md5_ctx_t *ctx);

#endif //HAVE_OPENSSL_MD5

#endif //__MD5_H_
