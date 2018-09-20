/************************************************************************/
/* SCFILEUSER.H中定义一些关于文件相关的应用操作的函数
/* liw 20120316
/************************************************************************/
#ifndef SCFILEUSER_H
#define SCFILEUSER_H

#include <wtypes.h>

//在相应的目录下查找文件,目录不能有子目录
//参数 :pszDir 查找的路径
//      pszName 查找的文件，是通配符形式
//      pszFileName 存放查找的文件名称      
//      nFileNum 输入输出参数nFileNum 输入是表示数组的个数，输出表示查找的文件
//返回值：表示找到的文件个数，如果返回值与nFileNum 不相等，说明外面申请存放文件名称的内存不够从新分配查找
long sc_FindFile(const char* pszDir,const char * pszName,char *pszFileName[512],unsigned int* nFileNum);

//判断一个目录是否为空目录
//参数 :pszDir 查找的路径
//返回值:空为TRUE，非空为FALSE
BOOL sc_DirIsEmpty(const char* pszDir);

char *sc_module_GetLibDir ();
#endif