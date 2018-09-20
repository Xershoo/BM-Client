#ifndef __CLUPDATER__H__
#define __CLUPDATER__H__

#include "download_service.h"
#include "CLFile.h"
enum UpdateStage {
		kUpdateDownloadProtocolVersion,
		kDontNeedUpdate,	//已经是最新版本，不需要更新
		kUpdateStageDownloadLatestManifest,
		kUpdateLocalFiles,
		kUpdateComplete,
		kUpdateFailed,
	};

struct SLVersion
{
	SLVersion()
	{
		memset(this, 0, sizeof(SLVersion));
		nNum = 0;
	}
	void InitFromStr(const char* pBuffer)
	{
		nNum = 0;
		if(pBuffer == NULL)
			return;
		const char* p = NULL;
		for (int i = 0; i < strlen(pBuffer); i++)		
		{
			char ch = pBuffer[i];
			if(ch != '.' && (ch < '0' || ch > '9'))
				return;
		}
		nMain = atoi(pBuffer);
		p = strchr(pBuffer, '.');
		if(p)
		{
			p++;
			nMin = atoi(p);
			nNum = 2;
			p = strchr(p, '.');
		}
		if(p)
		{
			p++;
			nExe = atoi(p);
			nNum = 3;
			p = strchr(p, '.');
		}
		if(p)
		{
			p++;
			nBuild = atoi(p);
			nNum = 4;
			p = strchr(p, '.');
		}
		if(p)
			nNum = 5;
	}
	bool operator<(SLVersion& sInfo)
	{
		if(nMain < sInfo.nMain)
			return true;
		else if(nMain > sInfo.nMain)
			return false;
		if(nMin < sInfo.nMin)
			return true;
		else if(nMin > sInfo.nMin)
			return false;
		if(nExe < sInfo.nExe)
			return true;
		else if(nExe > sInfo.nExe)
			return false;
		if(nBuild < sInfo.nBuild)
			return true;
		else 
			return false;
	}
	bool operator == (SLVersion& sInfo)
	{
		if(nNum != sInfo.nNum)
			return false;
		if(nMain != sInfo.nMain || nMin != sInfo.nMin || nExe != sInfo.nExe || nBuild != sInfo.nBuild)
			return false;

		return true;
	}
	DWORD nMain;
	DWORD nMin;
	DWORD nExe;
	DWORD nBuild;

	int	 nNum;
};

class CLUpdater :  public DownloadServiceListener
{
private:
	CLUpdater();
	~CLUpdater(){}
public:
	static CLUpdater* GetInstance();
	bool  Run();

	void DownloadSucceed(ULONG client_id, const char* buffer, size_t buffer_size);
	void DownloadFailed(ULONG client_id);
	void ReportProgress(ULONG client_id, ULONG ulProgress, ULONG ulProgressMax);
	
	void DownloadProtocolVersion();
	void DownloadManifestVersion();
	void DonwloadFiles();
	void UpdateComplete(); //文件下载完成，更改版本文件
	float GetCompletePercent();
private:
	void ExitUpdate(); //检测文件不需要更新，退出，并启动class8.exe

	bool ParseLatestManifest(const std::string& json);
protected:
	void DeleteOldVersionPath();
	
private:
	DownloadService download_service_;
	static CLUpdater*	_updater;

	
	UpdateStage stage_;
	wchar_t gt_install_path_[MAX_PATH];
	wchar_t local_version_filepath[MAX_PATH];
	 char gt_update_path[MAX_PATH];
	  CLFile  hProtocolVersionFile_;
	SLVersion	_localverion;
	SLVersion	_newVersion;
	struct SLFileInfo
	{
		ATL::CString strFileName;
		ATL::CStringA strMd5;
		int			 nFileSize;
		int			 nTryTimes;
	};

	struct LatestManifest {
		std::string version;
		std::string url_prefix;
		std::string	 update_info;
		std::vector<SLFileInfo> files;
	};
	LatestManifest latest_manifest_;
	int _nDownloadIndex;

	int		_nTotalSize;
	

	ATL::CString _strVersionPath; //本机当前版本路径
	ATL::CString _strNewVersionPath;//更新版本路径
};

#endif