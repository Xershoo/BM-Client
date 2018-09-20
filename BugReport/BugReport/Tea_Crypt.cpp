//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <string.h>
//#include <math.h>

//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "Tea_Crypt.h"
#include "stdafx.h"

typedef unsigned long WORD32;
const WORD32 DELTA = 0x9e3779b9;
	
#define ROUNDS 16
	
#define LOG_ROUNDS 4
	
	
/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
	
void TeaEncryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf)
{
	
	WORD32 y, z;
	WORD32 sum;
	WORD32 k[4];
	int i;
	y = ntohl(*((WORD32*)pInBuf));
	z = ntohl(*((WORD32*)(pInBuf+4)));
	for ( i = 0; i<4; i++)
	{
		k[i] = ntohl(*((WORD32*)(pKey+i*4)));	
	}

	sum = 0;	
	for (i=0; i<ROUNDS; i++)
	{
		
		sum += DELTA;
		y += (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
		z += (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3];
	}
	*((WORD32*)pOutBuf) = htonl(y);
	*((WORD32*)(pOutBuf+4)) = htonl(z);
}
	
/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/	
void TeaDecryptECB(const BYTE *pInBuf, const BYTE *pKey, BYTE *pOutBuf)
{
	WORD32 y, z, sum;
	WORD32 k[4];
	int i;

	y = ntohl(*((WORD32*)pInBuf));	
	z = ntohl(*((WORD32*)(pInBuf+4)));
	for ( i=0; i<4; i++)
	{
		k[i] = ntohl(*((WORD32*)(pKey+i*4)));	
	}
	sum = DELTA << LOG_ROUNDS;
	for (i=0; i<ROUNDS; i++)
	{
		z -= (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3];	
		y -= (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
		sum -= DELTA;
	}

	*((WORD32*)pOutBuf) = htonl(y);	
	*((WORD32*)(pOutBuf+4)) = htonl(z);
}
	
#define SALT_LEN 2
#define ZERO_LEN 7
	
/*pKeyΪ16byte*/
/*
  ����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;
	���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���;	
	*/
/*TEA�����㷨,CBCģʽ*/
	
/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
	
void oi_symmetry_encrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
{
	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;
	int nPadlen = 0;
	BYTE src_buf[8], zero_iv[8], *iv_buf;
	int src_i, i, j;
	/*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
	nPadSaltBodyZeroLen = nInBufLen/*Body����*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(8byte)*/;

	if ((nPadlen=(nPadSaltBodyZeroLen%8))!=0)
	{
		/*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/	
		nPadlen=8-nPadlen;	
	}
	/*srand( (unsigned)time( NULL ) ); ��ʼ�������*/
	
	/*���ܵ�һ������(8byte),ȡǰ��10byte*/
	src_buf[0] = ((BYTE)rand()) & 0x0f8/*�����λ��PadLen,����*/ | (BYTE)nPadlen;
	src_i = 1; /*src_iָ��src_buf��һ��λ��*/

	while(nPadlen--)	
		src_buf[src_i++]=(BYTE)rand(); /*Padding*/
	/*come here, i must <= 8*/
	memset(zero_iv, 0, 8);
	
	iv_buf = zero_iv; /*make iv*/
	*pOutBufLen = 0; /*init OutBufLen*/
	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i<8)		
		{
			src_buf[src_i++]=(BYTE)rand();	
			i++; /*i inc in here*/
		}
		if (src_i==8)	
		{
			/*src_i==8*/	
			for (j=0;j<8;j++) /*CBC XOR*/
				src_buf[j]^=iv_buf[j];
			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
	/*src_iָ��src_buf��һ��λ��*/
	while(nInBufLen)
	{
		if (src_i<8)		
		{
			src_buf[src_i++]=*(pInBuf++);	
			nInBufLen--;
		}
		if (src_i==8)	
		{
			/*src_i==8*/	
			for (i=0;i<8;i++) /*CBC XOR*/
				src_buf[i]^=iv_buf[i];
			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
	/*src_iָ��src_buf��һ��λ��*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (src_i<8)		
		{
			src_buf[src_i++]=0;	
			i++; /*i inc in here*/
		}
		if (src_i==8)	
		{		
			/*src_i==8*/	
			for (j=0;j<8;j++) /*CBC XOR*/
				
				src_buf[j]^=iv_buf[j];
			
			/*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
}

/*pKeyΪ16byte*/

/*

  ����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���;
  
	���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���;
	
	  ����ֵ:�����ʽ��ȷ����TRUE;
	  
		*/
		
		/*TEA�����㷨,CBCģʽ*/
		
		/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
BOOL oi_symmetry_decrypt(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)			
{
	
	
	
	int nPadLen, nPlainLen;
	
	BYTE dest_buf[8];
	
	const BYTE *iv_buf;
	
	int dest_i, i, j;
	
	
	
	//if (nInBufLen%8) return FALSE; BUG!
	
	if ((nInBufLen%8) || (nInBufLen<16)) return FALSE;
	
	
	
	
	
	TeaDecryptECB(pInBuf, pKey, dest_buf);
	
	
	
	nPadLen = dest_buf[0] & 0x7/*ֻҪ�����λ*/;
	
	
	
	/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
	
	i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*���ĳ���*/
	
	if (*pOutBufLen<i) return FALSE;
	
	*pOutBufLen = i;
	
	if (*pOutBufLen < 0) return FALSE;
	
	
	
	
	
	iv_buf = pInBuf; /*init iv*/
	
	nInBufLen -= 8;
	
	pInBuf += 8;
	
	
	
	dest_i=1; /*dest_iָ��dest_buf��һ��λ��*/
	
	
	
	
	
	/*��Padding�˵�*/
	
	dest_i+=nPadLen;
	
	
	
	/*dest_i must <=8*/
	
	
	
	/*��Salt�˵�*/
	
	for (i=1; i<=SALT_LEN;)
		
	{
		
		if (dest_i<8)
			
		{
			
			dest_i++;
			
			i++;
			
		}
		
		else if (dest_i==8)
			
		{
			
			/*dest_i==8*/
			
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			
			for (j=0; j<8; j++)
				
				dest_buf[j]^=iv_buf[j];
			
			
			
			iv_buf = pInBuf;
			
			nInBufLen -= 8;
			
			pInBuf += 8;
			
			
			
			dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
			
		}
		
	}
	
	
	
	/*��ԭ����*/
	
	
	
	nPlainLen=*pOutBufLen;
	
	while (nPlainLen)
		
	{
		
		if (dest_i<8)
			
		{
			
			*(pOutBuf++)=dest_buf[dest_i++];
			
			nPlainLen--;
			
		}
		
		else if (dest_i==8)
			
		{
			
			/*dest_i==8*/
			
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			
			for (i=0; i<8; i++)
				
				dest_buf[i]^=iv_buf[i];
			
			
			
			iv_buf = pInBuf;
			
			nInBufLen -= 8;
			
			pInBuf += 8;
			
			
			
			dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
			
		}
		
	}
	
	
	
	/*У��Zero*/
	
	for (i=1;i<=ZERO_LEN;)
		
	{
		
		if (dest_i<8)
			
		{
			
			if(dest_buf[dest_i++]) return FALSE;
			
			i++;
			
		}
		
		else if (dest_i==8)
			
		{
			
			/*dest_i==8*/
			
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			
			for (j=0; j<8; j++)
				
				dest_buf[j]^=iv_buf[j];
			
			
			
			iv_buf = pInBuf;
			
			nInBufLen -= 8;
			
			pInBuf += 8;
			
			
			
			dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
			
		}
		
		
		
	}
	return TRUE;
	
}



///////////////////////////////////////////////////////////////////////////////////////////////



/*pKeyΪ16byte*/

/*

  ����:pInBufΪ����ܵ����Ĳ���(Body),nInBufLenΪpInBuf����;
  
	���:pOutBufΪ���ĸ�ʽ,pOutBufLenΪpOutBuf�ĳ�����8byte�ı���;
	
	  */
	  
	  /*TEA�����㷨,CBCģʽ*/
	  
	  /*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
	  
void oi_symmetry_encrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)		  
{		  
	int nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero�ĳ���*/;		  
	int nPadlen;
	BYTE src_buf[8], iv_plain[8], *iv_crypt;
	int src_i, i, j;
		  
	/*����Body���ȼ���PadLen,��С���賤�ȱ���Ϊ8byte��������*/
		  
	nPadSaltBodyZeroLen = nInBufLen/*Body����*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(8byte)*/;
	if ((nPadlen=(nPadSaltBodyZeroLen%8)) != 0)
	{
	  /*ģ8��0�貹0,��1��7,��2��6,...,��7��1*/
	  nPadlen=8-nPadlen;
	}
	/*srand( (unsigned)time( NULL ) ); ��ʼ�������*/
	
	/*���ܵ�һ������(8byte),ȡǰ��10byte*/
	src_buf[0] = ((BYTE)rand()) & 0x0f8/*�����λ��PadLen,����*/ | (BYTE)nPadlen;
	src_i = 1; /*src_iָ��src_buf��һ��λ��*/
	while(nPadlen--)
		src_buf[src_i++]=(BYTE)rand(); /*Padding*/
	/*come here, src_i must <= 8*/
	for ( i=0; i<8; i++)
	  iv_plain[i] = 0;
		  
	iv_crypt = iv_plain; /*make zero iv*/
		  
	*pOutBufLen = 0; /*init OutBufLen*/
		  
	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
			  
			  if (src_i<8)
				  
			  {
				  
				  src_buf[src_i++]=(BYTE)rand();
				  
				  i++; /*i inc in here*/
				  
			  }
			  
			  
			  
			  if (src_i==8)
				  
			  {
				  
				  /*src_i==8*/
				  
				  
				  
				  for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
					  
					  src_buf[j]^=iv_crypt[j];
				  
				  
				  
				  /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
				  
				  /*����*/
				  
				  TeaEncryptECB(src_buf, pKey, pOutBuf);
				  
				  
				  
				  for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
					  
					  pOutBuf[j]^=iv_plain[j];
				  
				  
				  
				  /*���浱ǰ��iv_plain*/
				  
				  for (j=0;j<8;j++)
					  
					  iv_plain[j]=src_buf[j];
				  
				  
				  
				  /*����iv_crypt*/
				  
				  src_i=0;
				  
				  iv_crypt=pOutBuf;
				  
				  *pOutBufLen+=8;
				  
				  pOutBuf+=8;
				  
			  }
			  
		  }
		  
		  
		  
		  /*src_iָ��src_buf��һ��λ��*/
		  
		  
		  
		  while(nInBufLen)
			  
		  {
			  
			  if (src_i<8)
				  
			  {
				  
				  src_buf[src_i++]=*(pInBuf++);
				  
				  nInBufLen--;
				  
			  }
			  
			  
			  
			  if (src_i==8)
				  
			  {
				  
				  /*src_i==8*/
				  
				  
				  
				  for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
					  
					  src_buf[j]^=iv_crypt[j];
				  
				  /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
				  
				  TeaEncryptECB(src_buf, pKey, pOutBuf);
				  
				  
				  
				  for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
					  
					  pOutBuf[j]^=iv_plain[j];
				  
				  
				  
				  /*���浱ǰ��iv_plain*/
				  
				  for (j=0;j<8;j++)
					  
					  iv_plain[j]=src_buf[j];
				  
				  
				  
				  src_i=0;
				  
				  iv_crypt=pOutBuf;
				  
				  *pOutBufLen+=8;
				  
				  pOutBuf+=8;
				  
			  }
			  
		  }
		  
		  
		  
		  /*src_iָ��src_buf��һ��λ��*/
		  
		  
		  
		  for (i=1;i<=ZERO_LEN;)
			  
		  {
			  
			  if (src_i<8)
				  
			  {
				  
				  src_buf[src_i++]=0;
				  
				  i++; /*i inc in here*/
				  
			  }
			  
			  
			  
			  if (src_i==8)
				  
			  {
				  
				  /*src_i==8*/
				  
				  
				  
				  for (j=0;j<8;j++) /*����ǰ���ǰ8��byte������(iv_cryptָ���)*/
					  
					  src_buf[j]^=iv_crypt[j];
				  
				  /*pOutBuffer��pInBuffer��Ϊ8byte, pKeyΪ16byte*/
				  
				  TeaEncryptECB(src_buf, pKey, pOutBuf);
				  
				  
				  
				  for (j=0;j<8;j++) /*���ܺ����ǰ8��byte������(iv_plainָ���)*/
					  
					  pOutBuf[j]^=iv_plain[j];
				  
				  
				  
				  /*���浱ǰ��iv_plain*/
				  
				  for (j=0;j<8;j++)
					  
					  iv_plain[j]=src_buf[j];
				  
				  
				  
				  src_i=0;
				  
				  iv_crypt=pOutBuf;
				  
				  *pOutBufLen+=8;
				  
				  pOutBuf+=8;
				  
			  }
			  
		  }
}



/*pKeyΪ16byte*/

/*

  ����:pInBufΪ���ĸ�ʽ,nInBufLenΪpInBuf�ĳ�����8byte�ı���;
  
	���:pOutBufΪ����(Body),pOutBufLenΪpOutBuf�ĳ���;
	
	  ����ֵ:�����ʽ��ȷ����TRUE;
	  
		*/
		
		/*TEA�����㷨,CBCģʽ*/
		
		/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
		
		BOOL oi_symmetry_decrypt2(const BYTE* pInBuf, int nInBufLen, const BYTE* pKey, BYTE* pOutBuf, int *pOutBufLen)
			
		{
			
			
			
			int nPadLen, nPlainLen;
			
			BYTE dest_buf[8], zero_buf[8];
			
			const BYTE *iv_pre_crypt, *iv_cur_crypt;
			
			int dest_i, i, j;
			
			
			
			//if (nInBufLen%8) return FALSE; BUG!
			
			if ((nInBufLen%8) || (nInBufLen<16)) return FALSE;
			
			
			
			
			
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			
			
			
			nPadLen = dest_buf[0] & 0x7/*ֻҪ�����λ*/;
			
			
			
			/*���ĸ�ʽ:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(8byte)*/
			
			i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*���ĳ���*/
			
			if (*pOutBufLen<i) return FALSE;
			
			*pOutBufLen = i;
			
			if (*pOutBufLen < 0) return FALSE;
			
			
			
			for ( i=0; i<8; i++)
				
				zero_buf[i] = 0;
			
			
			
			iv_pre_crypt = zero_buf;
			
			iv_cur_crypt = pInBuf; /*init iv*/
			
			nInBufLen -= 8;
			
			pInBuf += 8;
			
			
			
			dest_i=1; /*dest_iָ��dest_buf��һ��λ��*/
			
			
			
			
			
			/*��Padding�˵�*/
			
			dest_i+=nPadLen;
			
			
			
			/*dest_i must <=8*/
			
			
			
			/*��Salt�˵�*/
			
			for (i=1; i<=SALT_LEN;)
				
			{
				
				if (dest_i<8)
					
				{
					
					dest_i++;
					
					i++;
					
				}
				
				else if (dest_i==8)
					
				{
					
					/*�⿪һ���µļ��ܿ�*/
					
					
					
					/*�ı�ǰһ�����ܿ��ָ��*/
					
					iv_pre_crypt = iv_cur_crypt;
					
					iv_cur_crypt = pInBuf;
					
					
					
					/*���ǰһ������(��dest_buf[]��)*/
					
					for (j=0; j<8; j++)
						
						dest_buf[j]^=pInBuf[j];
					
					
					
					/*dest_i==8*/
					
					TeaDecryptECB(dest_buf, pKey, dest_buf);
					
					
					
					/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/
					
					
					
					nInBufLen -= 8;
					
					pInBuf += 8;
					
					
					
					dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
					
				}
				
			}
			
			
			
			/*��ԭ����*/
			
			
			
			nPlainLen=*pOutBufLen;
			
			while (nPlainLen)
				
			{
				
				if (dest_i<8)
					
				{
					
					*(pOutBuf++)=dest_buf[dest_i]^iv_pre_crypt[dest_i];
					
					dest_i++;
					
					nPlainLen--;
					
				}
				
				else if (dest_i==8)
					
				{
					
					/*dest_i==8*/
					
					
					
					/*�ı�ǰһ�����ܿ��ָ��*/
					
					iv_pre_crypt = iv_cur_crypt;
					
					iv_cur_crypt = pInBuf;
					
					
					
					/*�⿪һ���µļ��ܿ�*/
					
					
					
					/*���ǰһ������(��dest_buf[]��)*/
					
					for (j=0; j<8; j++)
						
						dest_buf[j]^=pInBuf[j];
					
					
					
					TeaDecryptECB(dest_buf, pKey, dest_buf);
					
					
					
					/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/
					
					
					
					nInBufLen -= 8;
					
					pInBuf += 8;
					
					
					
					dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
					
				}
				
			}
			
			
			
			/*У��Zero*/
			
			for (i=1;i<=ZERO_LEN;)
				
			{
				
				if (dest_i<8)
					
				{
					
					if(dest_buf[dest_i]^iv_pre_crypt[dest_i]) return FALSE;
					
					dest_i++;
					
					i++;
					
				}
				
				else if (dest_i==8)
					
				{
					
					/*�ı�ǰһ�����ܿ��ָ��*/
					
					iv_pre_crypt = iv_cur_crypt;
					
					iv_cur_crypt = pInBuf;
					
					
					
					/*�⿪һ���µļ��ܿ�*/
					
					
					
					/*���ǰһ������(��dest_buf[]��)*/
					
					for (j=0; j<8; j++)
						
						dest_buf[j]^=pInBuf[j];
					
					
					
					TeaDecryptECB(dest_buf, pKey, dest_buf);
					
					
					
					/*��ȡ����ʱ������ǰһ������(iv_pre_crypt)*/
					
					
					
					nInBufLen -= 8;
					
					pInBuf += 8;
					
					
					
					dest_i=0; /*dest_iָ��dest_buf��һ��λ��*/
					
				}
			}
			return TRUE;
}
