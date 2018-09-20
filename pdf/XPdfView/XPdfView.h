#pragma once

#ifdef XPDFVIEW_EXPORTS
#define XPDFVIEW_API __declspec(dllexport)
#else
#define XPDFVIEW_API __declspec(dllimport)
#endif

#define WM_PDF_SHOW_CMD		(WM_USER+)

extern XPDFVIEW_API int WINAPI OpenPDFFile(WCHAR* pwszFileName,HWND hShowWnd,RECT rc,LPARAM lParam=NULL,BOOL bCtrl=TRUE);
extern XPDFVIEW_API void WINAPI ClosePDFFile(int nId);
extern XPDFVIEW_API void WINAPI PDFNextPage(int nId);
extern XPDFVIEW_API void WINAPI PDFPrevPage(int nId);
extern XPDFVIEW_API void WINAPI PDFZoomIn(int nId);
extern XPDFVIEW_API void WINAPI PDFZoomOut(int nId);
extern XPDFVIEW_API void WINAPI PDFShow(int nId,BOOL bShow);


