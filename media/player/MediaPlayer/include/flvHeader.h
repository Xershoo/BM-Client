#ifndef FLVHEADER_H
#define FLVHEADER_H

#pragma comment(lib, "zlib.lib")

typedef struct 
{
    char  tag_header_type;//��ƵΪ0x8 ��ƵΪ0x9  �ű�Ϊ0x12
    char  tag_data_length[3];//���������� no include header size
    unsigned char  timestamp[4];//ʱ��� ���һ���ֽ�Ϊ��չʱ���
    char  stream_id[3];//Always 0
}flv_tag_header_t;

static int big_endian_test()                                                                         
{                                                                                                         
#define FLV_BIGENDIAN 1                                                                                   
#define FLV_LITTLEENDIAN 0                                                                                
	const short n = 1;                                                                                    
	if(*(char *)&n)                                                                                       
	{                                                                                                     
		return FLV_LITTLEENDIAN;                                                                          
	}                                                                                                     
	return FLV_BIGENDIAN;                                                                                 
} 

#define swicth_int(tag_data,len,pd) \
do{\
    unsigned char * ppc_tp = (unsigned char *)pd;\
    *(pd) = 0;\
    if(big_endian_test()){\
        memcpy(ppc_tp,tag_data,len);\
    }\
    else{\
        size_t n_tp = 0;\
        for(; n_tp < len;++n_tp){\
            ppc_tp[n_tp] = tag_data[len-n_tp-1];\
        }\
    }\
}while(0)

#define FLVFILE_COPYSTAMP_INT(nstamp,stamp)\
        nstamp = (unsigned char)stamp[3] << 24 | (unsigned char)stamp[0] << 16 | (unsigned char)stamp[1] << 8 | (unsigned char)stamp[2]

#endif