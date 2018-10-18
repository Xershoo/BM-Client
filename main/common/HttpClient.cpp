//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：HttpClient.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：HTTP下载，上传，通信等接口类的实现
//	修改记录：
//**********************************************************************
#include "HttpClient.h"
#include "HttpSessionMgr.h"
#include "jason/include/json.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Util.h"

#define _CRT_SECURE_NO_WARNINGS

CHttpClient::CHttpClient():m_pcurl(NULL),m_lParam(NULL)
{  
	m_pcurl = curl_easy_init();  
}  

CHttpClient::~CHttpClient()  
{  
	if(m_pcurl)
	{
		curl_easy_cleanup(m_pcurl);  
	}
}  

size_t CHttpClient::WriteData(char *data, size_t block_size, size_t block_count, string *response)  
{  
	if(data == NULL) 
    {
        return 0;  
    }

	size_t len = block_size * block_count;  
	response->append(data, len);

	return len;  
}  

size_t CHttpClient::WriteFile(char *data, size_t block_size, size_t block_count, FILE *file)  
{  
	if(data == NULL) 
    {
        return 0; 
    }

	size_t len = block_size * block_count;  
	if(NULL != file)
	{
		fwrite(data, len, 1, file);   
	}

	return len;  
}  

size_t CHttpClient::ProgressCallback(void * ptrData,double dDownTotal,double dDownNow,double dUploadTotal,double dUploadNow)
{
	qDebug("CUrl progress call,%.02f,%.02f,%.02f,%.02f\n",dDownTotal,dDownNow,dUploadTotal,dUploadNow);

    CHttpSessionMgr* pSessMgr = NULL;
    if(!CHttpSessionMgr::isValid() )
    {
        return 1;
    }

    pSessMgr = CHttpSessionMgr::GetInstance();	
	LPHTTPSESSION pHttpSess = (LPHTTPSESSION)ptrData;
    if (NULL == pHttpSess || pHttpSess->bCancle)
    {
	    return 1;
    }

    if(!pSessMgr->isExsitSession(pHttpSess))
    {
        return 1;
    }

    long nTotal = (long)dDownTotal == 0 ? (long)dUploadTotal : (long)dDownTotal;
    long nDo = (long)dDownNow == 0 ? (long)dUploadNow : (long)dDownNow;
    if (nTotal > 0 && nDo > 0)
    {
        pSessMgr->HttpProgressCallBack(ptrData,nTotal,nDo);
    }

	return 0;
}

bool CHttpClient::Get(const string& url, string &response, int timeout)  
{  
	return Request(url, "", "get", response, timeout);  
}  

bool CHttpClient::Put(const string &url, const string &request, string &response, int timeout)  
{  
	return Request(url, request, "put", response, timeout);  
}  

bool CHttpClient::Post(const string& url, const string &request, string &response, int timeout)  
{  
	return Request(url, request, "post", response, timeout);  
}  

bool CHttpClient::Delete(const string &url, string &response, int timeout)  
{  
	return Request(url, "", "delete", response, timeout);  
}  

bool CHttpClient::Save( const std::string& url, const char *filename, int timeout, long long ltotalsize)  
{  
	if(NULL==m_pcurl)
	{
		return false;
	}

	if(NULL==filename||NULL==filename[0])
	{
		return false;
	}

	//支持断点续传，先获取文件大小，如果文件存在并且非空，则断点续传
	ifstream infile(filename, ifstream::binary);
	long long nlocalfilesize = 0;
	stringstream ss;

	if (infile.good())
	{
		infile.seekg(0, infile.end);
		int nLength = infile.tellg();
		nlocalfilesize = nLength;
		infile.seekg(0, infile.beg);
		if (nLength > 0)
		{
			ss.str("");
			ss << nlocalfilesize;
			ss << "-";
			long long ltotal = ltotalsize;
			if (ltotal > 0 && ltotal > nlocalfilesize)
			{
				ss << ltotal;
			}
			
            qDebug("%s had download %d(%d)", filename, nLength, ltotalsize);
		}
	}

	infile.close();

	m_response_code = 0;  

	FILE* file = NULL;
	if (0 == nlocalfilesize)
	{
		file = fopen(filename,"wb");  
	}
	else
	{
        file = fopen(filename,"ab");
    }
    
	if(NULL == file)
	{
		return false;
	}

	curl_easy_reset(m_pcurl);  
	curl_easy_setopt(m_pcurl, CURLOPT_URL, url.c_str());  

	string srange;	
	ss >> srange;

	if (srange.size()>0)
	{
		curl_easy_setopt(m_pcurl, CURLOPT_RANGE,srange.c_str());
	}
	curl_easy_setopt(m_pcurl, CURLOPT_WRITEFUNCTION, WriteFile);  
	curl_easy_setopt(m_pcurl, CURLOPT_WRITEDATA, file);  
	curl_easy_setopt(m_pcurl, CURLOPT_FOLLOWLOCATION, 1);  
	curl_easy_setopt(m_pcurl, CURLOPT_NOSIGNAL, 1);  
	curl_easy_setopt(m_pcurl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(m_pcurl, CURLOPT_TIMEOUT, timeout); 
	curl_easy_setopt(m_pcurl, CURLOPT_NOPROGRESS, 0L);                
	curl_easy_setopt(m_pcurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	if(m_lParam)
	{
		curl_easy_setopt(m_pcurl, CURLOPT_PROGRESSDATA, m_lParam);
	}

	//处理信号终端导致阻塞（时长为timeout）,
	//解决方法：缩短下载超时时长为30S，增加重复连接次数，同时采用断点续传，这样可解决次问题
	CURLcode rc = curl_easy_perform(m_pcurl);  
	fclose(file);  
	if(rc != CURLE_OK)  
	{  
		m_error =  string(curl_easy_strerror(rc));  
		return false;  
	}  

	rc = curl_easy_getinfo(m_pcurl, CURLINFO_RESPONSE_CODE , &m_response_code);   
	if(rc != CURLE_OK)  
	{  
		m_error =  string(curl_easy_strerror(rc));  
		return false;  
	}
	
    if(m_response_code == 404)
	{
        return false;
    }

	return true;  
}  

bool CHttpClient::Upload(const std::string& url, const char *filename,string &response,int& timeout,string& form,const char* sfilename /* = NULL */)
{
	if(NULL==m_pcurl)
	{
		return false;
	}

	if(NULL==filename||NULL==filename[0])
	{
		return false;
	}

	struct _stat file_info = { 0 };
	FILE * fp = fopen(filename, "rb"); 
	if(NULL==fp)    
	{
		return false;
	}

	if(_fstat(fileno(fp), &file_info) != 0)    
	{
		return false;
	}

	fclose(fp);

	if(file_info.st_size <= 0)
	{
		return false;
	}

	response.clear();  
	response.reserve(64 * 1024);

	m_response_code = 0;
	curl_easy_reset(m_pcurl);

	bool br = false;

	struct curl_httppost* formpost = NULL;
	struct curl_httppost* lastptr = NULL;
	struct curl_slist* headerlist = NULL;
	static const char listbuf[] = "Expect:";

	std::string strname;

	if(NULL == sfilename || NULL == sfilename[0])
	{
		strname = (strrchr(filename,'\\')+1);
	}
	else
	{
		strname = sfilename;
	}

	do
	{
		headerlist = curl_slist_append(headerlist, listbuf);
		if(NULL==headerlist)
		{
			break;
		}

		curl_formadd(&formpost, &lastptr,CURLFORM_COPYNAME, "upload",
			CURLFORM_FILE, filename,CURLFORM_END);

		curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME, "filename",  
			CURLFORM_COPYCONTENTS, strname.c_str(),CURLFORM_END);

		curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME, "submit",  
			CURLFORM_COPYCONTENTS, "send",CURLFORM_END);  

		if(!form.empty())
		{
			string strKey;
			string strValue;

			Json::Reader jReader; 
			Json::Value jValue; 
			
			jReader.parse(form,jValue);
			Json::Value::Members jKeys = jValue.getMemberNames();

			for(Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
			{
				strKey = *iter;
				strValue = jValue[strKey.c_str()].asString();

				curl_formadd(&formpost,&lastptr,CURLFORM_COPYNAME, strKey.c_str(), 
					CURLFORM_COPYCONTENTS, strValue.c_str(),CURLFORM_END); 
			}
		}

		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_URL, url.c_str());
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_VERBOSE, 1L);		
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_FOLLOWLOCATION, 1);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_COOKIEFILE, "");
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_COOKIELIST, "ALL");
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_HTTPHEADER, headerlist);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_HTTPPOST, formpost);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_MAX_SEND_SPEED_LARGE, MAX_SPEED);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_WRITEFUNCTION, WriteData);
		m_response_code = curl_easy_setopt(m_pcurl, CURLOPT_WRITEDATA, &response);

		//progress callback                
		curl_easy_setopt(m_pcurl, CURLOPT_NOPROGRESS, 0L);                
		curl_easy_setopt(m_pcurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
		if(m_lParam)
		{
			curl_easy_setopt(m_pcurl, CURLOPT_PROGRESSDATA, m_lParam);
		}
		DWORD nBegin = ::GetTickCount();
		CURLcode rc = curl_easy_perform(m_pcurl);  
		if(rc != CURLE_OK)  
		{  
			m_error =  string(curl_easy_strerror(rc));  
			break;  
		}
		DWORD nEnd = ::GetTickCount();
		timeout = nEnd - nBegin;

		rc = curl_easy_getinfo(m_pcurl, CURLINFO_RESPONSE_CODE , &m_response_code);   
		if(rc != CURLE_OK)  
		{  
			m_error =  string(curl_easy_strerror(rc));  
			break;  
		}

		br = true;
	}while(false);

	if(NULL != formpost)
	{
		curl_formfree(formpost);	
		formpost = NULL;
	}

	if(NULL != headerlist)
	{
		curl_slist_free_all(headerlist);
		headerlist = NULL;
	}

	Util::PrintTrace("CHttpClient::UploadFile[%d][%s]...[%s][%s][%s][%s]",br,m_error.c_str(),url.c_str(),filename,response.c_str(),form.c_str());

	return br;
}

bool CHttpClient::Request(const string &url, const string &request, string method, string &response, int timeout)  
{  
	if(NULL==m_pcurl)
	{
		return false;
	}

	m_response_code = 0;  

	response.clear();  
	response.reserve(64 * 1024);  

	curl_easy_reset(m_pcurl);  
	curl_easy_setopt(m_pcurl, CURLOPT_URL, url.c_str());  
	curl_easy_setopt(m_pcurl, CURLOPT_NOSIGNAL, 1);  
	curl_easy_setopt(m_pcurl, CURLOPT_TIMEOUT, timeout);  
	curl_easy_setopt(m_pcurl, CURLOPT_WRITEFUNCTION, WriteData);  
	curl_easy_setopt(m_pcurl, CURLOPT_WRITEDATA, &response);  
	if(method == "get")  
	{  
		curl_easy_setopt(m_pcurl, CURLOPT_HTTPGET, 1);  
	}  
	else if(method == "post")  
	{  
		curl_easy_setopt(m_pcurl, CURLOPT_POST, 1L);  
		curl_easy_setopt(m_pcurl, CURLOPT_POSTFIELDSIZE, request.length());  
		curl_easy_setopt(m_pcurl, CURLOPT_POSTFIELDS, request.c_str());  
	}  
	else if(method == "put")  
	{  
		curl_easy_setopt(m_pcurl, CURLOPT_CUSTOMREQUEST, "PUT");  
		curl_easy_setopt(m_pcurl, CURLOPT_POSTFIELDSIZE, request.length());  
		curl_easy_setopt(m_pcurl, CURLOPT_POSTFIELDS, request.c_str());  
	}  
	else if(method == "delete")  
	{  
		curl_easy_setopt(m_pcurl, CURLOPT_CUSTOMREQUEST, "DELETE");  
	}  
	else  
	{  
		return false;  
	}  

	CURLcode rc = curl_easy_perform(m_pcurl);  
	if(rc != CURLE_OK)  
	{  
		m_error =  string(curl_easy_strerror(rc));  
		return false;  
	}  

	rc = curl_easy_getinfo(m_pcurl, CURLINFO_RESPONSE_CODE , &m_response_code);   
	if(rc != CURLE_OK)  
	{  
		m_error =  string(curl_easy_strerror(rc));  
		return false;  
	}  

	return true;  
}

void CHttpClient::SetParam(LPVOID lParam)
{
	m_lParam = lParam;
}

long CHttpClient::GetResponseCode()
{
	return m_response_code;
}

string CHttpClient::GetErrorString()
{
	return m_error;
}