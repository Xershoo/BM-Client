/************************************************************************/
/* SCFILEUSER.H�ж���һЩ�����ļ���ص�Ӧ�ò����ĺ���
/* liw 20120316
/************************************************************************/
#ifndef SCFILEUSER_H
#define SCFILEUSER_H

#include <wtypes.h>

//����Ӧ��Ŀ¼�²����ļ�,Ŀ¼��������Ŀ¼
//���� :pszDir ���ҵ�·��
//      pszName ���ҵ��ļ�����ͨ�����ʽ
//      pszFileName ��Ų��ҵ��ļ�����      
//      nFileNum �����������nFileNum �����Ǳ�ʾ����ĸ����������ʾ���ҵ��ļ�
//����ֵ����ʾ�ҵ����ļ��������������ֵ��nFileNum ����ȣ�˵�������������ļ����Ƶ��ڴ治�����·������
long sc_FindFile(const char* pszDir,const char * pszName,char *pszFileName[512],unsigned int* nFileNum);

//�ж�һ��Ŀ¼�Ƿ�Ϊ��Ŀ¼
//���� :pszDir ���ҵ�·��
//����ֵ:��ΪTRUE���ǿ�ΪFALSE
BOOL sc_DirIsEmpty(const char* pszDir);

char *sc_module_GetLibDir ();
#endif