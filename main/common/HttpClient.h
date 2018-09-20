//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：HttpClient.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：提供HTTP下载，上传，通信等接口类的定义
//	修改记录：
//**********************************************************************

#include <QtCore\QObject>
#include <curl/curl.h>
#include <string>
#include <sys/stat.h>

using std::string;  

#define MAX_SPEED (100*1024)

class CHttpClient 
{
public: 	
	CHttpClient();
	~CHttpClient();

	void SetParam(LPVOID lParam);

	bool Get(const string& url, string &response, int timeout);
	bool Put(const string &url, const string &request, string &response, int timeout);
	bool Post(const string& url, const string &request, string &response, int timeout);
	bool Delete(const string &url, string &response, int timeout);
	bool Save( const std::string& url, const char *filename, int timeout, long long ltotalsize = 0);
	bool Upload(const std::string& url, const char *filename,string &response,int& timeout,string& form,const char* sfilename = NULL);

	long GetResponseCode();
	string GetErrorString();

protected:
	static size_t WriteData(char *data, size_t block_size, size_t block_count, string *response);
	static size_t WriteFile(char *data, size_t block_size, size_t block_count, FILE *file);
	static size_t ProgressCallback(void * ptrData,double dDownTotal,double dDownNow,double dUploadTotal,double dUploadNow);

private:  
	bool Request(const string &url, const string &request, string method, string &response, int timeout);

private:  
	CURL  *	m_pcurl;  
	string	m_error;  
	long	m_response_code; 
	long	m_nFileSize;

	LPVOID	m_lParam;
}; 