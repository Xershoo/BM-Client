/************************************************************************/
/* 动态加载动态库的函数的封装，可以实现跨平台函数
/* liw 20120309
/************************************************************************/

//加载的库的句柄
typedef void* HLIBRARY;

//动态加载函数
//参数：pszLibName 所需要加载的库的名称
//返回值 成功返回库的句柄，失败返回NULL
HLIBRARY  HLIB_LoadLibrary(const char * pszLibName);

//获取要引用的函数，将符号名互评标识号转化为DLL的内部地址
//参数：hlib 库句柄
//      pszFunc 对应的函数名称或标识
//返回值: 成功返回相应函数名称或标识在DLL对应的函数地址，失败返回NULL
void* HLIB_GetProcAddress(HLIBRARY hlib,const char* pszFunc);

//释放
//参数：hlib库句柄
//返回值:成功返回1，失败返回0
int HLIB_FreeLibrary (HLIBRARY hlib);   

//获取库加载过程中的错误信息
//返回值:返回具体的错误信息
const char* HLIB_GetErrorMsg(void);