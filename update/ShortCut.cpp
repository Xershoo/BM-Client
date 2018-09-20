
#include "ShortCut.h"
#include <ShlObj.h>

namespace app_common
{

	CLShortCut::CLShortCut()
	{
		TCHAR tczPath[MAX_PATH] = {0};
		//指向全用户的桌面
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, tczPath);
		m_strDesktopPath = tczPath;

		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, tczPath);
		m_strQuickLaunchPath.Format(_T("%s\\Microsoft\\Internet Explorer\\Quick Launch"), tczPath);
	}

	CLShortCut::~CLShortCut()
	{
	}

	bool CLShortCut::AppHasShorCutInDesktop(LPCTSTR lpAppName, ATL::CString& strPath)
	{
		if(lpAppName == NULL)
			return false;
		IShellFolder *m_ShellFolderTopMost=NULL;
		HRESULT hr=SHGetDesktopFolder(&m_ShellFolderTopMost);
		if(FAILED(hr))
		{
			return false;
		}
		IShellFolder *m_pFolder;
		LPITEMIDLIST pidlWorkDir=NULL;
		hr = m_ShellFolderTopMost->ParseDisplayName(NULL,NULL,(LPTSTR)(LPCTSTR)m_strDesktopPath,NULL,&pidlWorkDir,NULL);
		if(FAILED(hr))
		{
			return false;
		}
		hr=m_ShellFolderTopMost->BindToObject(pidlWorkDir,NULL,IID_IShellFolder,(LPVOID*)&m_pFolder);
		m_ShellFolderTopMost->Release();	
		IEnumIDList *m_pIEnumFile = NULL;
		hr=m_pFolder->EnumObjects(0,SHCONTF_NONFOLDERS,&m_pIEnumFile);
		LPITEMIDLIST m_pItem=NULL;
		ULONG m_ulwork= 0;
		strPath.Empty();
		while(m_pIEnumFile->Next(1,&m_pItem,&m_ulwork)==S_OK)
		{		
			WIN32_FIND_DATA ffd;
			SHGetDataFromIDList(m_pFolder,m_pItem,SHGDFIL_FINDDATA,&ffd,sizeof(WIN32_FIND_DATA));
			ATL::CString strName(ffd.cFileName);
			if(strName.Find(lpAppName) >= 0)
			{
				strPath.Format(L"%s\\%s",m_strDesktopPath, ffd.cFileName);
				break;
			}
		}
		m_pFolder->Release();
		if(strPath.IsEmpty())
			return false;
		else
			return true; 
	}

	bool CLShortCut::CreateFileDeskTopShortcut(LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName,  LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
	{	

		HRESULT hr;
		IShellLink     *pLink = NULL;  //IShellLink对象指针
		IPersistFile   *ppf; //IPersisFil对象指针
		//创建IShellLink对象
		hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
		if (FAILED(hr))
		{		
			return false;
		}

		//从IShellLink对象中获取IPersistFile接口
		hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
		if (FAILED(hr))
		{
			pLink->Release();
			return false;
		}

		//目标
		if (lpszFileName == NULL)
			pLink->SetPath(_wpgmptr);
		else
			pLink->SetPath(lpszFileName);

		//工作目录
		if (lpszWorkDir != NULL)
			pLink->SetWorkingDirectory(lpszWorkDir);

		//快捷键
		if (wHotkey != 0)
			pLink->SetHotkey(wHotkey);

		//备注
		if (lpszDescription != NULL)
			pLink->SetDescription(lpszDescription);

		//显示方式
		pLink->SetShowCmd(iShowCmd);


		//快捷方式的路径 + 名称
		TCHAR szBuffer[MAX_PATH] = {0};
		if (lpszLnkFileName != NULL) //指定了快捷方式的名称
			_stprintf(szBuffer, L"%s\\%s", m_strDesktopPath, lpszLnkFileName);
		else  
		{
			//没有指定名称，就从取指定文件的文件名作为快捷方式名称。
			const TCHAR *pstr;
			if (lpszFileName != NULL)
				pstr = _tcsrchr(lpszFileName, _T('\\'));
			else
				pstr = _tcsrchr(_wpgmptr,  _T('\\'));

			if (pstr == NULL)
			{    
				ppf->Release();
				pLink->Release();
				return FALSE;
			}
			//注意后缀名要从.exe改为.lnk
			_stprintf(szBuffer, L"%s%s.lnk", m_strDesktopPath, pstr);		
		}

		hr = ppf->Save(szBuffer, TRUE);
		//SHChangeNotify(SHCNE_UPDATEDIR | SHCNE_INTERRUPT, SHCNF_FLUSH | SHCNF_PATH, m_strDesktopPath, 0);

		ppf->Release();
		pLink->Release();
		return SUCCEEDED(hr);
	}

	bool CLShortCut::GetShorCutExePath(LPCTSTR lpAppName, ATL::CString& strExePath)
	{
		strExePath.Empty();
		if(lpAppName == NULL)
			return false;
		IShellLink *m_pIShellLink=NULL;
		IPersistFile *m_pIPersistFile=NULL;
		HRESULT hr=::CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&m_pIShellLink);
		if(hr==S_OK)
		{
			hr=m_pIShellLink->QueryInterface(IID_IPersistFile,(void **)&m_pIPersistFile);
			if(hr==S_OK)
			{
				USES_CONVERSION;
				m_pIPersistFile->Load(T2COLE(lpAppName),STGM_READWRITE);
				wchar_t m_strPath[1000]={0};
				//读取快捷方式的目标路径
				m_pIShellLink->GetPath(m_strPath,260,NULL,SLGP_UNCPRIORITY);
				strExePath = m_strPath;
			}		
		}

		if(m_pIShellLink) 
			m_pIShellLink->Release();
		if(m_pIPersistFile) 
			m_pIPersistFile->Release();
		return true;
	}
	bool CLShortCut::DeleteShortCutInAllUserAppData(LPCTSTR lpAppName)
	{	
		if(lpAppName == NULL)
			return false;
		TCHAR tczPath[MAX_PATH] = {0};
		//指向全用户的桌面
		SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, tczPath);
		PathAppend(tczPath, lpAppName);
		::DeleteFile(tczPath);		
		return true;
	}

}