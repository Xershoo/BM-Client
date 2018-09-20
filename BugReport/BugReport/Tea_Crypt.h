/*
OIcqCrypt.h
OICQ����
hyj@oicq.com
1999/12/24
  ʵ�������㷨:

  Hash�㷨: MD5,��ʵ��

  �Գ��㷨: DES,δʵ��

  �ǶԳ��㷨: RSA,δʵ��
*/

#ifndef _INCLUDED_OICQCRYPT_H_
#define _INCLUDED_OICQCRYPT_H_

extern "C" {

//typedef char BYTE;

//typedef char BOOL;

#define TRUE 1

#define FALSE 0

#define MD5_DIGEST_LENGTH	16

BOOL Md5Test(); /*����MD5�����Ƿ���rfc1321����*/

/*

	����const BYTE *inBuffer��int length

	���BYTE *outBuffer

	����length��Ϊ0,outBuffer�ĳ���ΪMD5_DIGEST_LENGTH(16byte)

*/

void Md5HashBuffer( BYTE *outBuffer, const BYTE *inBuffer, int length);

/*

	����const BYTE *inBuffer��int length

	���BYTE *outBuffer

	����length��Ϊ0,outBuffer�ĳ���ΪMD5_DIGEST_LENGTH(16byte)

*/
//pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte

void TeaEncryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf);
void TeaDecryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf);

/*pKeyΪ16byte*/

/*

	����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;

	���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���,����ӦԤ��nInBufLen+17;

*/

/*TEA�����㷨,CBCģʽ*/

/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/

void oi_symmetry_encrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);

/*pKeyΪ16byte*/
/*

	����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���; *pOutBufLenΪ���ջ������ĳ���

		�ر�ע��*pOutBufLenӦԤ�ý��ջ������ĳ���!

	���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���,����ӦԤ��nInBufLen-10;

	����ֵ:�����ʽ��ȷ����TRUE;

*/

/*TEA�����㷨,CBCģʽ*/

/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/

BOOL oi_symmetry_decrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);

/*pKeyΪ16byte*/
/*
	����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;
	���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���,����ӦԤ��nInBufLen+17;
*/

/*TEA�����㷨,CBCģʽ*/

/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/

void oi_symmetry_encrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);

/*pKeyΪ16byte*/
/*
	����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���; *pOutBufLenΪ���ջ������ĳ���

		�ر�ע��*pOutBufLenӦԤ�ý��ջ������ĳ���!

	���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���,����ӦԤ��nInBufLen-10;

	����ֵ:�����ʽ��ȷ����TRUE;

*/

/*TEA�����㷨,CBCģʽ*/
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/

BOOL oi_symmetry_decrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen);

} /* extern "C" */


#endif

