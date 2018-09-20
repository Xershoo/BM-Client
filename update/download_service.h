
#ifndef IM_GTUPDATER_DOWNLOAD_SERVICE_H_
#define IM_GTUPDATER_DOWNLOAD_SERVICE_H_

#include <windows.h>
#include <map>
#include <string>

namespace PATCH
{
	class CHttpDownloader;
}

class DownloadServiceListener {
 public:
  virtual void DownloadSucceed(ULONG client_id, const char* buffer,
                               size_t buffer_size) = 0;
  virtual void DownloadFailed(ULONG client_id) = 0;
  virtual void ReportProgress(ULONG client_id, ULONG ulProgress,
                              ULONG ulProgressMax) = 0;
};

class DownloadService {
 public:
  DownloadService(DownloadServiceListener* listener);
  ~DownloadService();

  static const ULONG kInvalidClientId = -1;

  ULONG DownloadUrl(const std::wstring& url, const std::wstring& strSaveName);
  void CancelClient(ULONG client_id);
 void OnClientStop(PATCH::CHttpDownloader* client, bool succeed);
 private:
	 friend class PATCH::CHttpDownloader;
 
  void OnClientReportProgress(PATCH::CHttpDownloader* client, ULONG ulProgress,
                              ULONG ulProgressMax);

  DownloadServiceListener* listener_;

  typedef std::map<ULONG, PATCH::CHttpDownloader*> DownloadClientMap;
  typedef DownloadClientMap::iterator DownloadClientMapIt;
  typedef DownloadClientMap::const_iterator DownloadClientMapCIt;
  DownloadClientMap download_clients_;

  ULONG client_id_generator_;
};

#endif  // IM_UPDATER_DOWNLOAD_SERVICE_H_
