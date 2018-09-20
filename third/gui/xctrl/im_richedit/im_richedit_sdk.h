
#ifndef IM_RICHEDIT_IM_RICHEDIT_SDK_H_
#define IM_RICHEDIT_IM_RICHEDIT_SDK_H_

#include <windows.h>
#include <imm.h>
#include <richedit.h>
#include <richole.h>
#include <textserv.h>

#if defined(IM_RICHEDIT_IMPLEMENTATION)
#define IM_RICHEDIT_EXPORT __declspec(dllexport)
#else
#define IM_RICHEDIT_EXPORT __declspec(dllimport)
#endif  // defined(IM_RICHEDIT_IMPLEMENTATION)

// RichEdit使用注意:
//   1.设置CFE_LINK后立即调用AutoURLDetect会导致RichEdit解析当前Word是否为链接.
//     如果想避免这种情况, 必须在这CFE_LINK后插入空格以便把Word区分出来.
//   2.想要对ITextServices发送EM_SCROLLCARET消息, 必须设置ES_NOHIDESEL风格, 或者
//     发送EM_HIDESELECTION消息改变设置(自动滚动到底部功能).

namespace im_richedit {

class IMRichEditHost {
 public:
  virtual void OnEraseBackground(HDC dc, const RECT& rect) = 0;
};

class IMRichEdit {
 public:
  virtual void DeleteThis() = 0;
  virtual int  GetSelectionCharSize() const = 0;
  virtual void SetSelectionCharSize(int size) = 0;
  virtual BSTR GetSelectionCharFace() const = 0;
  virtual void SetSelectionCharFace(const wchar_t* face_name) = 0;
  virtual bool GetSelectionCharBold() const = 0;
  virtual void SetSelectionCharBold(bool bold) = 0;
  virtual bool GetSelectionCharItalic() const = 0;
  virtual void SetSelectionCharItalic(bool italic) = 0;
  virtual void SetSelectionCharColor(COLORREF color) = 0;
  virtual int  SaveCharFormat() = 0;
  virtual bool RestoreCharFormat(int save_state) = 0;
  virtual void SetParaLineSpacing(float spacing) = 0;
  virtual void SetParaLineSpaceAfter(int space) = 0;
  virtual void SetParaStartIndent(int indent) = 0;
  virtual void SetParaBullet(bool bullet) = 0;
  virtual void SetSelAll() = 0;
  virtual void SetCaretToEnd() = 0;
  virtual void ScrollToBottom() = 0;
  virtual void SetAutoURLDetect(bool enable) = 0;
  virtual void InsertText(const wchar_t* text) = 0;
  virtual void InsertLink(const wchar_t* text) = 0;
  virtual void InsertBreak() = 0;
  virtual bool InsertPicture(const wchar_t* uri, DWORD user_data,long cx_show = 0,long cy_show =0) = 0;
  virtual bool GetContent(IStream** content_stream) const = 0;
  virtual int  GetLineCount() const = 0;
  virtual void DeleteTopLines(int lines) = 0;
  virtual int  GetParagraphCount() const = 0;
  virtual void DeleteTopParagraphs(int paras) = 0;
  virtual void DeleteParagraphs(int startParas, int endParas) = 0; 
  virtual void SetObjectPicture(long object_index, const wchar_t* uri,long cx_show = 0,long cy_show = 0) = 0;
  virtual bool PtInZoomPictureObject(POINT pt,wchar_t* uri,int nLength) = 0;
  virtual bool PtInZoomPictureObjectWithUserData(POINT pt,wchar_t* uri,int nLength, DWORD &userData) = 0;
};


class ITextWinHost : public IUnknown {
 public:
  //@cmember Generic Send Message interface
  virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam,
                                LRESULT* plresult) = 0;
};

class ITextWinHostDelegate {
 public:
  virtual HWND TxGetHostWindow() = 0;
  virtual BOOL TxRegisterDragDrop(LPDROPTARGET pDropTarget) = 0;
  virtual void TxRevokeDragDrop() = 0;
  virtual BOOL TxEnableScrollBar(UINT sb_flags, UINT arrows) = 0;
  virtual BOOL TxShowScrollBar(int bar, BOOL show) = 0;
  virtual int  TxSetScrollInfo(int bar, LPCSCROLLINFO lpsi, BOOL redraw) = 0;
  virtual BOOL TxSetScrollRange(int bar, int min_pos, int max_pos,
                                BOOL redraw) = 0;
  virtual int  TxSetScrollPos(int bar, int pos, BOOL redraw) = 0;
  virtual void TxInvalidateRect(LPCRECT rect, BOOL erase) = 0;
  virtual void TxSetCapture() = 0;
  virtual void TxReleaseCapture() = 0;
  virtual void TxSetFocus() = 0;
  virtual HRESULT OnTxNotify(DWORD iNotify, void* pv) = 0;
};

}  // namespace im_richedit

extern "C" {

IM_RICHEDIT_EXPORT HDC IMGetThreadMemDC();

IM_RICHEDIT_EXPORT im_richedit::IMRichEdit* CreateIMRichEdit(
    IRichEditOle* richedit_ole, im_richedit::IMRichEditHost* host);

IM_RICHEDIT_EXPORT im_richedit::ITextWinHost* CreateTextHostExt(
    DWORD style, im_richedit::ITextWinHostDelegate* delegate);

}  // extern "C"

#endif  // IM_RICHEDIT_IM_RICHEDIT_SDK_H_
