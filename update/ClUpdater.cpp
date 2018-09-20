#include "stdafx.h"
#include "ClUpdater.h"
#include "CLFile.h"
#include "include/json.h"
#include "StringHelper.h"
#include "HiddenWnd.h"
#include "ShortCut.h"
#include "common/Config.h"
#include "common/lang.h"

CLUpdater* CLUpdater::_updater = NULL;

int		_nCompleteSize = 0;

CLUpdater::CLUpdater() : download_service_(this)
						 ,stage_(kUpdateDownloadProtocolVersion)
{
    GetModuleFileName(NULL, gt_install_path_, 512);  
    //取出文件路径   
    DWORD nSize = 0;
    for (int i=wcslen(gt_install_path_); i>=0; i--)
    {   
        if (gt_install_path_[i] == '\\')
        {
            nSize++;
            gt_install_path_[i] = '\0';
            if (nSize >= 2)
            {
                break;   
            }
        }
    }
	
	::ZeroMemory(gt_update_path, sizeof(gt_update_path));  
	::GetModuleFileNameA(NULL, gt_update_path, MAX_PATH); 
	
	PathAppendW(local_version_filepath, gt_install_path_);
    _strVersionPath.Format(L"%s", gt_install_path_);

    PathAppendW(local_version_filepath, L"local_version");
    CLFile cFile;
    cFile.OpenA(ATL::CStringA(local_version_filepath));
    nSize = cFile.GetFileSize();
    if(nSize > 0)
    {
        char* pBuffer = new char[nSize + 1];
        ZeroMemory(pBuffer, nSize + 1);
        cFile.Read(nSize, pBuffer, &nSize);
        _localverion.InitFromStr(pBuffer);
    }
	_nDownloadIndex = 0;
	_nCompleteSize = 0;
	_nTotalSize = 0;
}

CLUpdater* CLUpdater::GetInstance()
{
	if(_updater == NULL)
		_updater = new CLUpdater;
	return _updater;
}

bool  CLUpdater::Run()
{
	DownloadProtocolVersion();
	return true;
}

void CLUpdater::DownloadProtocolVersion()
{
    WCHAR wszUrl[1024] = { 0 };
    string strUrl = Config::getConfig()->m_urlUpdateProtocol;
    ::MultiByteToWideChar(CP_ACP,0,strUrl.c_str(),strUrl.length(),wszUrl,sizeof(wszUrl)-1);

    stage_ = kUpdateDownloadProtocolVersion;
	download_service_.DownloadUrl(wszUrl, L"tempverion");	
}

void CLUpdater::DownloadManifestVersion()
{  
    WCHAR wszUrl[1024] = { 0 };
    string strUrl = Config::getConfig()->m_urlUpdateManifest;
    ::MultiByteToWideChar(CP_ACP,0,strUrl.c_str(),strUrl.length(),wszUrl,sizeof(wszUrl)-1);

    stage_ = kUpdateStageDownloadLatestManifest;
	download_service_.DownloadUrl(wszUrl, L"tempverion");
}

void CLUpdater::DownloadSucceed(ULONG client_id, const char* buffer, size_t buffer_size)
{
	if(stage_ == kUpdateDownloadProtocolVersion)
	{
		 ATL::CString strGtPath;
			  strGtPath.Format(L"%s\\protocol_version", gt_install_path_);
			  ::CopyFile(ATL::CString(buffer), strGtPath, FALSE);
			  ::DeleteFileA(buffer);
			  if(!hProtocolVersionFile_.OpenA(CStringA(strGtPath)))
			  {
				  ExitUpdate();
				  return;
			  }
			  DWORD nFileSize = hProtocolVersionFile_.GetFileSize();
			  if(nFileSize <= 0)
			  {
				  ExitUpdate();
				  return;
			  }
			  char* pReadBuffer = new char[nFileSize + 1];
			  pReadBuffer[nFileSize] = 0;
			  DWORD nRead = 0;
			  if(!hProtocolVersionFile_.Read(nFileSize, pReadBuffer, &nRead))
			  {
				  ExitUpdate();
				  return;
			  }			  
              
              hProtocolVersionFile_.Close();

			  SLVersion sNewVerion;
			  sNewVerion.InitFromStr(pReadBuffer);
			  if(sNewVerion.nNum != 4 || _localverion < sNewVerion)
			  {
				  m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
				 // DownloadManifestVersion();
			  }
			  else
			  {
				  m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, kDontNeedUpdate, 0);
				  //ExitUpdate();
			  }
	}
	else if(stage_ == kUpdateStageDownloadLatestManifest)
	{
		CLFile cFile;
		if(!cFile.OpenA(buffer))
		{
			ExitUpdate();
			return;
		}
		DWORD nFileSize = cFile.GetFileSize();
		if(nFileSize <= 0)
		{
			ExitUpdate();
			return;
		}
		char* pReadBuffer = new char[nFileSize];
		DWORD nRead = 0;
		if(!cFile.Read(nFileSize, pReadBuffer, &nRead))
		{
			ExitUpdate();
			return;
		}
		std::string json(pReadBuffer, nRead);
		delete []pReadBuffer;
		cFile.Close();
		::DeleteFileA(buffer);
		if (!ParseLatestManifest(json)) {
			ExitUpdate();
		//} else if (NeedUpdate()) {
		//	//BroadcastDetectNewVersion();
		//	stage_ = kUpdateLocalFiles;
		//	UpdateLocalFiles();
		} 
		
		//下载文件列表成功，开始下载数据
		stage_ = kUpdateLocalFiles;
		 m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
	}
	else if(stage_ = kUpdateLocalFiles)
	{
		SLFileInfo sInfo = latest_manifest_.files[_nDownloadIndex];
		ATL::CString strNewPath;
		strNewPath.Format(L"%s\\%s", _strNewVersionPath, sInfo.strFileName);
		strNewPath.Replace(L'/', L'\\');		
		ATL::CStringA strMd5;
		CaleFileMd5(strNewPath, strMd5);
		if(strMd5 == sInfo.strMd5 || sInfo.nFileSize == 0)		
			_nDownloadIndex++;
		else
		{
			sInfo.nTryTimes++;
			if(sInfo.nTryTimes > 3)
			{
				DownloadFailed(0);
				return;
			}
		}
		//DonwloadFiles();
		 m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
		
	}
}
void CLUpdater::DownloadFailed(ULONG client_id)
{
	stage_ = kUpdateFailed;
	m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
}
void CLUpdater::ReportProgress(ULONG client_id, ULONG ulProgress, ULONG ulProgressMax)
{

}

void CLUpdater::ExitUpdate()
{
	stage_ = kUpdateComplete;
	m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
}

bool CLUpdater::ParseLatestManifest(const std::string& json) {

	std::string strKey;
	std::string strValue;

	Json::Reader jReader; 
	Json::Value jValue; 
	jReader.parse(json,jValue);
	
	latest_manifest_.version = jValue.get("version", strValue).asString();
	latest_manifest_.url_prefix = jValue.get("url_prefix", strValue).asString();
	latest_manifest_.update_info = jValue.get("update_info", strValue).asString();
	Json::Value jFiles = jValue.get("manifest", strValue);
	_newVersion.InitFromStr(latest_manifest_.version.c_str());
	for (int i= 0; i < jFiles.size(); i++)
	{
		Json::Value oneFile = jFiles[i];
		SLFileInfo sFileInfo;
		sFileInfo.nTryTimes = 0;
		sFileInfo.strFileName = ATL::CString(oneFile.get("file", strValue).asString().c_str());
		sFileInfo.strMd5 = oneFile.get("md5", strValue).asString().c_str();
		sFileInfo.strMd5.MakeUpper();
		sFileInfo.nFileSize = oneFile.get("size",strValue).asInt();
		latest_manifest_.files.push_back(sFileInfo);
		_nTotalSize += sFileInfo.nFileSize;
	}
	
	_strNewVersionPath.Format(L"%s", gt_install_path_);
	return true;
}

void CLUpdater::DonwloadFiles()
{
	if(_nDownloadIndex >= latest_manifest_.files.size())
	{
		//下载完成
		UpdateComplete();
		return;
	}
	SLFileInfo sInfo = latest_manifest_.files[_nDownloadIndex];
	ATL::CString strOldFilePath;
	strOldFilePath.Format(L"%s\\%s", _strVersionPath, sInfo.strFileName);
	ATL::CStringA strMd5;
	CaleFileMd5(strOldFilePath, strMd5);
	ATL::CString strNewPath;
	strNewPath.Format(L"%s\\%s", _strNewVersionPath, sInfo.strFileName);
	strNewPath.Replace(L'/', L'\\');
	wchar_t dir[MAX_PATH] = { 0 };
	
	lstrcpyW(dir, strNewPath);
	if (PathRemoveFileSpecW(dir))
		CLFile::CreateDirectoryByPath(dir);

	if(strMd5 == sInfo.strMd5)
	{
		
		//::CopyFile(strOldFilePath, strNewPath, FALSE); 如果相同，什么都不做
		_nDownloadIndex++;
		//DonwloadFiles();
		_nCompleteSize += sInfo.nFileSize;
		 m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);
	}
	else
	{
		ATL::CString strUrl;
		strUrl.Format(L"%s%s", ATL::CString(latest_manifest_.url_prefix.c_str()), sInfo.strFileName);
		std::wstring wstrurl(strUrl);
		std::wstring wstrpath(strNewPath);
		download_service_.DownloadUrl(wstrurl, wstrpath);
	}

}


void CLUpdater::UpdateComplete()
{
	CLFile cFile;
	cFile.OpenA(ATL::CStringA(local_version_filepath));
	DWORD dwWrite = 0;
	cFile.Write(latest_manifest_.version.length(), latest_manifest_.version.c_str(), &dwWrite);

	cFile.Close();
	stage_ = kUpdateComplete;
	m_HiddenWnd.PostMessage(UPdATE_DOWNLOAD_SUCCESS, stage_, 0);

	ATL::CString strNewPath;
    WCHAR wszPFile[MAX_PATH] = {0};
    ::LoadStringW(NULL,IDS_PRODUCT_FILE,wszPFile,MAX_PATH);
	strNewPath.Format(L"%s\\bin\\%s", _strNewVersionPath,wszPFile);
	
    //更改快捷方式
	app_common::CLShortCut shortcutmgr;
	ATL::CString link_path;
    
    WCHAR wszProductName[MAX_PATH] = { 0 };
    WCHAR wszPFileLink[MAX_PATH] = { 0 };
    
    char szProduct[MAX_PATH] = {0};
    ::LoadStringA(NULL,IDS_PRODUCT,szProduct,MAX_PATH);
    if(LangSet::loadLangSet("szProduct") == LangSet::LANG_ENG)
    {
        ::LoadStringW(NULL,IDS_PRODUCT_NAME_EN,wszProductName,MAX_PATH);
    }
    else
    {
        ::LoadStringW(NULL,IDS_PRODUCT_NAME,wszProductName,MAX_PATH);
    }

    wcscat(wszProductName,L".lnk");
    
    wcscpy(wszPFileLink,wszPFile);
    wcscat(wszPFileLink,L".lnk");

	if( shortcutmgr.AppHasShorCutInDesktop(wszPFileLink, link_path) ||
		shortcutmgr.AppHasShorCutInDesktop(wszProductName, link_path)) 
    {
		ATL::CString old_link_exe_path;
		shortcutmgr.GetShorCutExePath(link_path, old_link_exe_path);
		old_link_exe_path.MakeLower();
		if (old_link_exe_path.Find(wszPFile) < 0) 
        {
			shortcutmgr.CreateFileDeskTopShortcut(strNewPath, wszProductName,
				NULL, 0, NULL);

			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
		}
	}

	//删除老版本
	DeleteOldVersionPath();
}

float CLUpdater::GetCompletePercent()
{
	if(_nTotalSize == 0)
		return 100.0;
	return (float)_nCompleteSize * 100 / _nTotalSize;
}

void CLUpdater::DeleteOldVersionPath()
{
	ATL::CString s1(gt_install_path_);
	s1 += L'\\';
	ATL::CString s2;     
	HANDLE   h   =   NULL;     
	WIN32_FIND_DATA   data;     	
	s2   =   s1   + _T("*.*");     
	std::vector<ATL::CString> strDeleteFiles;
	if((h=::FindFirstFile(s2,&data))!=0)     
	{
		SLVersion sVersion;
		do     
		{     
			if(   (ATL::CString(data.cFileName) == _T("."))   ||   (ATL::CString(data.cFileName) == _T("..")  ) )     
				continue;     
			if   ((data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) )   //文件夹     
			{
				CStringA strAsiFile(data.cFileName);
				sVersion.InitFromStr(strAsiFile);
				if(sVersion.nNum == 4)
				{
					if(sVersion == _localverion || sVersion == _newVersion)
						continue;
					s2   =   s1 + ATL::CString(data.cFileName);   //提取文件名     
					strDeleteFiles.push_back(s2);
				}
			}			
		}while(::FindNextFile(h,&data));  
	}

	for (size_t i = 0; i < strDeleteFiles.size(); i++)
	{
		CLFile::DeleteDirectory((LPTSTR)(LPCTSTR)strDeleteFiles[i]);
	}
}