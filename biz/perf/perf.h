#ifndef __PERF_H
#define __PERF_H

#if ( defined __x86_64__ || defined __i386__ )
typedef unsigned char RC4_CONTEXT[264];
typedef unsigned char BF_CONTEXT[4168];
typedef unsigned char BFCBC_CONTEXT[4176];
typedef unsigned char MD5_CONTEXT[88];
typedef unsigned char SHA1_CONTEXT[92];
#if defined __x86_64__
typedef unsigned char AES_CONTEXT[256];
typedef unsigned char AESCBC_CONTEXT[272];
#else
typedef unsigned char AES_CONTEXT[248];
typedef unsigned char AESCBC_CONTEXT[264];
#endif

#if defined __cplusplus
extern "C" {
#endif

void rc4_init_context ( RC4_CONTEXT context, const void *key, unsigned int keylen );
void rc4_update ( RC4_CONTEXT context, void *inout, unsigned int inputlen );

#define PERF_ENCRYPT	0x00000000
#define PERF_DECRYPT	0x80000000
#define AES128		128
#define AES192		192
#define AES256		256
void aes_init_context(AES_CONTEXT context, const void *key, unsigned int keybits);
void aes_encrypt(AES_CONTEXT context, void *dst, const void *src);	// 16 byte block
void aes_decrypt(AES_CONTEXT context, void *dst, const void *src);
void aes_cbc_init_context(AESCBC_CONTEXT context, const void *key, unsigned int keybits, const void *iv);	// 16 byte iv
void aes_cbc_encrypt(AESCBC_CONTEXT context, void *dst, const void *src, unsigned int nblocks);
void aes_cbc_decrypt(AESCBC_CONTEXT context, void *dst, const void *src, unsigned int nblocks);

void bf_init_context(BF_CONTEXT context, const void* key, size_t keybits);	// keybits: 32, 64, 96, 128, ..., 448
void bf_encrypt(BF_CONTEXT context, void *dst, const void *src);	// 8 byte block
void bf_decrypt(BF_CONTEXT context, void *dst, const void *src);
void bf_cbc_init_context(BFCBC_CONTEXT, const void *key, unsigned int keybits, const void *iv);	// 8 byte iv
void bf_cbc_encrypt(BFCBC_CONTEXT context, void *dst, const void *src, unsigned int nblocks);
void bf_cbc_decrypt(BFCBC_CONTEXT context,  void *dst, const void *src, unsigned int nblocks);

void md5_init_context( MD5_CONTEXT context );
void md5_update( MD5_CONTEXT context, const void *input, unsigned int inputlen );
void md5_final( MD5_CONTEXT context );
void md5_digest( unsigned char digest[16], const void *input, unsigned int inputlen );

void sha1_init_context( SHA1_CONTEXT context );
void sha1_update( SHA1_CONTEXT context, const void *input, unsigned int inputlen );
void sha1_final( SHA1_CONTEXT context );
void sha1_digest( unsigned char digest[20], const void *input, unsigned int inputlen );

void base64_encode(void *output, const void *input, unsigned long len);
unsigned long base64_decode(void *output, const void *input, unsigned long len);

unsigned char *mppc256_transform( unsigned char *obuf, size_t isize, unsigned char **histptr, unsigned char *hash[256]);

unsigned long crc32( const unsigned char *s, unsigned long len );

#if defined __cplusplus
};
#endif

#endif

#endif
