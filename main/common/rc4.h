#ifndef _ENCRYPT_RC4_
#define _ENCRYPT_RC4_

#include <string.h>

#define BOX_LEN 256
#define ARC4_KEY "!@#$%^&*()_+|UDPARC4KEYFORHARDKORE1.0.0PACKET1234567890*Talkwithfriends,family,orteammatesoverafastandclearconnection*Abilitytousetextchatforanyonewithoutamicrophone*Nativeclientandserversupportforall3majorplatforms*AutomaticmicrophonevolumenormalizationAd"

int GetKey(const unsigned char* pass, int pass_len, unsigned char *out);
int ARC4(const unsigned char * data, size_t data_len, unsigned char * out, int * out_len);
int RC4(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len, unsigned char* out, int* out_len); 
static void swap_byte(unsigned char* a, unsigned char* b);

char* Encrypt(const char* szSource, const char* szPassWord); // ���ܣ����ؼ��ܽ��
char* Decrypt(const char* szSource, const char* szPassWord); // ���ܣ����ؽ��ܽ��

char* ByteToHex(const unsigned char* vByte, const int vLen); // ���ֽ���pbBufferתΪʮ�������ַ��������㴫��
unsigned char* HexToByte(const char* szHex); // ��ʮ�������ַ���תΪ�ֽ���pbBuffer������

#endif // #ifndef _ENCRYPT_RC4_

