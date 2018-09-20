
#include "stdafx.h"
#include "download_service.h"
#include <atlstr.h>
#include <atlbase.h>
#include <urlmon.h>
#include "EC_HttpGet.h"

class DownloadClient : public IBindStatusCallback {
 public:
  DownloadClient(DownloadService* service);
  ~DownloadClient();

  std::wstring url() const { return url_; }
  DWORD total_read() const { return total_read_; }
  const char* content() const { return content_.c_str(); }
  
  bool Start(const std::wstring& url);
  void Cancel();

  STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding* pib);
  STDMETHOD(GetPriority)(LONG* pnPriority);
  STDMETHOD(OnLowResource)(DWORD reserved);
  STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax,
                        ULONG ulStatusCode, LPCWSTR szStatusText);
  STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError);
  STDMETHOD(GetBindInfo)(DWORD* grfBINDF, BINDINFO* pbindinfo);
  STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize,
                             FORMATETC* pformatetc, STGMEDIUM* pstgmed);
  STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown* punk);

 private:
  void OnRead(BYTE* pBytes, DWORD dwSize);
  void OnStop(bool succeed);

  DownloadService* service_;
  std::wstring url_;
  bool cancel_;

  ULONG ref_count_;
  CComPtr<IMoniker> moniker_;
  CComPtr<IBindCtx> bind_ctx_;
  CComPtr<IBinding> binding_;
  CComPtr<IStream> stream_;
  DWORD total_read_;
  std::string content_;
};

DownloadClient::DownloadClient(DownloadService* service)
    : ref_count_(1), service_(service),
      cancel_(false), total_read_(0) {
  ATLASSERT(service_);
}

DownloadClient::~DownloadClient() {
}

bool DownloadClient::Start(const std::wstring& url) {
  ATLASSERT(url_.empty());  // 只允许设置一次.
  url_ = url;

  HRESULT hr = S_OK;
  hr = CreateURLMoniker(NULL, url_.c_str(), &moniker_);
  if (SUCCEEDED(hr))
    hr = CreateBindCtx(0, &bind_ctx_);

  if (SUCCEEDED(hr))
    hr = RegisterBindStatusCallback(bind_ctx_, this, 0, 0L);
  else
    moniker_.Release();

  CComPtr<IStream> dummy_stream;
  if (SUCCEEDED(hr))
    hr = moniker_->BindToStorage(bind_ctx_, 0, __uuidof(IStream),
                                 reinterpret_cast<void**>(&dummy_stream));

  return SUCCEEDED(hr);
}

void DownloadClient::Cancel() {
  ATLASSERT(!cancel_);  // 只允许设置一次.
  cancel_ = true;
  service_ = NULL;
  if (binding_)
    binding_->Abort();
}

void DownloadClient::OnRead(BYTE* pBytes, DWORD dwSize) {
  content_.resize(total_read_ + dwSize + 1);
  char* write = const_cast<char*>(content_.data());
  write += total_read_;
  CopyMemory(write, pBytes, dwSize);
}

void DownloadClient::OnStop(bool succeed) {
  if (cancel_)
    return;

  ATLASSERT(service_);
 // service_->OnClientStop(this, succeed);
}

STDMETHODIMP DownloadClient::QueryInterface(REFIID riid, void** ppvObject) {
  HRESULT hr = E_NOINTERFACE;
  if (IsEqualIID(riid, IID_IUnknown) ||
      IsEqualIID(riid, IID_IBindStatusCallback)) {
    (*ppvObject) = static_cast<IBindStatusCallback*>(this);
    (reinterpret_cast<IBindStatusCallback*>(*ppvObject))->AddRef();
    hr = S_OK;
  }

  return hr;
}

STDMETHODIMP_(ULONG) DownloadClient::AddRef() {
  return InterlockedIncrement(&ref_count_);
}

STDMETHODIMP_(ULONG) DownloadClient::Release() {
  ULONG ref_count = InterlockedDecrement(&ref_count_);
  if (ref_count == 0)
    delete this;

  return ref_count;
}

STDMETHODIMP DownloadClient::OnStartBinding(DWORD dwReserved, IBinding* pib) {
  binding_ = pib;
  return S_OK;
}

STDMETHODIMP DownloadClient::GetPriority(LONG* pnPriority) {
  HRESULT hr = S_OK;
  if (pnPriority)
    *pnPriority = THREAD_PRIORITY_NORMAL;
  else
    hr = E_INVALIDARG;
  return S_OK;
}

STDMETHODIMP DownloadClient::OnLowResource(DWORD reserved) {
  return S_OK;
}

STDMETHODIMP DownloadClient::OnProgress(
    ULONG ulProgress, ULONG ulProgressMax,
    ULONG ulStatusCode, LPCWSTR szStatusText) {
  if (ulStatusCode == BINDSTATUS_DOWNLOADINGDATA) {
    ATLASSERT(service_);
  //  service_->OnClientReportProgress(this, ulProgress, ulProgressMax);
  }

  return S_OK;
}

STDMETHODIMP DownloadClient::OnStopBinding(HRESULT hresult, LPCWSTR szError) {
  OnStop(SUCCEEDED(hresult));
  binding_ = NULL;
  bind_ctx_ = NULL;
  moniker_ = NULL;
  return S_OK;
}

STDMETHODIMP DownloadClient::GetBindInfo(DWORD* grfBINDF, BINDINFO* pbindinfo) {
  if (pbindinfo == NULL || pbindinfo->cbSize == 0 || grfBINDF == NULL)
    return E_INVALIDARG;

  *grfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE |
              BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;

  ULONG cbSize = pbindinfo->cbSize;  // remember incoming cbSize
  ZeroMemory(pbindinfo, cbSize);  // zero out structure
  pbindinfo->cbSize = cbSize;  // restore cbSize
  pbindinfo->dwBindVerb = BINDVERB_GET;  // set verb
  return S_OK;
}

STDMETHODIMP DownloadClient::OnDataAvailable(
    DWORD grfBSCF, DWORD dwSize,
    FORMATETC* pformatetc, STGMEDIUM* pstgmed) {
  HRESULT hr = S_OK;

  // Get the Stream passed
  if (BSCF_FIRSTDATANOTIFICATION & grfBSCF) {
    if (!stream_ && pstgmed->tymed == TYMED_ISTREAM)
      stream_ = pstgmed->pstm;
  }

  // Minimum amount available that hasn't been read.
  DWORD read = dwSize - total_read_;
  // Placeholder for amount read during this pull.
  DWORD actually_read = 0;

  // If there is some data to be read then go ahead and read them
  if (stream_) {
    if (read > 0) {
      BYTE* bytes = NULL;
      ATLTRY(bytes = new BYTE[read + 1]);
      if (bytes == NULL) {
        // Before returning m_spStream should be released if grfBSCF is
        // BSCF_LASTDATANOTIFICATION.
        hr = E_OUTOFMEMORY;
      } else {
        hr = stream_->Read(bytes, read, &actually_read);
        if (SUCCEEDED(hr)) {
          bytes[actually_read] = 0;
          if (actually_read > 0) {
            OnRead(bytes, actually_read);
            total_read_ += actually_read;
          }
        }
        delete[] bytes;
      }
    }
  }

  if (BSCF_LASTDATANOTIFICATION & grfBSCF)
    stream_ = NULL;
  return hr;
}

STDMETHODIMP DownloadClient::OnObjectAvailable(REFIID riid, IUnknown* punk) {
  return S_OK;
}


DownloadService::DownloadService(DownloadServiceListener* listener)
    : listener_(listener), client_id_generator_(0) {
  ATLASSERT(listener_);
}

DownloadService::~DownloadService() {
  for (DownloadClientMapCIt it = download_clients_.begin();
       it != download_clients_.end();
       ++it) {
   // it->second->Cancel();
   // it->second->Release();  // 释放引用计数.
  }
  download_clients_.clear();
}

ULONG DownloadService::DownloadUrl(const std::wstring& url, const std::wstring& strSaveName) {
  for (DownloadClientMapCIt it = download_clients_.begin();
       it != download_clients_.end();
       ++it) {
    if (it->second->GetUrl() == url)
      return it->first;
  }

  ULONG client_id = client_id_generator_++;
  ATLENSURE(client_id != DownloadService::kInvalidClientId);
  PATCH::CHttpDownloader* client = new PATCH::CHttpDownloader(0,this);
  ATLASSERT(client);
  client->Download(url.c_str(),strSaveName.c_str(), false );
  //  download_clients_[client_id] = client;
    return client_id;
  
}

void DownloadService::CancelClient(ULONG client_id) {
 
}

void DownloadService::OnClientStop(PATCH::CHttpDownloader* client, bool succeed) {
  ULONG client_id = kInvalidClientId;  

  ATL::CString strName(client->GetFileName());
  if (succeed)
    listener_->DownloadSucceed(client_id, CStringA(client->GetFileName()),
                               0);
  else
    listener_->DownloadFailed(client_id);
}

void DownloadService::OnClientReportProgress(PATCH::CHttpDownloader* client,
                                             ULONG ulProgress,
                                             ULONG ulProgressMax) {
  ULONG client_id = kInvalidClientId;
  for (DownloadClientMapCIt it = download_clients_.begin();
       it != download_clients_.end();
       ++it) {
    if (it->second == client) {
      client_id = it->first;
      break;
    }
  }
  ATLENSURE(client_id != kInvalidClientId);
  listener_->ReportProgress(client_id, ulProgress, ulProgressMax);
}
