//QtAeroWin.cpp

#include "QtAeroWin.h"  
#include <QLibrary>  
#include <QApplication>  
#include <QWidget>  
#include <QList>  
#include <QPointer>

#ifdef Q_WS_WIN

// Blur behind data structures  
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified  
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified  
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified  
#define WM_DWMCOMPOSITIONCHANGED        0x031E    // Composition changed window message  
typedef struct _DWM_BLURBEHIND  
{  
    DWORD dwFlags;  
    BOOL fEnable;  
    HRGN hRgnBlur;  
    BOOL fTransitionOnMaximized;  
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;  
typedef struct _MARGINS  
{  
    int cxLeftWidth;  
    int cxRightWidth;  
    int cyTopHeight;  
    int cyBottomHeight;  
} MARGINS, *PMARGINS;  
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);  
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);  
typedef HRESULT (WINAPI *PtrDwmEnableAeroWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);  
typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);
typedef HRESULT (WINAPI *PtrDwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute,LPCVOID pvAttribute, DWORD cbAttribute);

static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled= 0;  
static PtrDwmEnableAeroWindow pDwmEnableAeroWindow = 0;  
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;  
static PtrDwmGetColorizationColor pDwmGetColorizationColor = 0;
static PtrDwmSetWindowAttribute pDwmSetWindowAttribute = 0;
/* 
 *同步响应DWM状态消息 
 */  
class WindowNotifier : public QWidget  
{  
public:  
    WindowNotifier() { winId(); }  
    void addWidget(QWidget *widget) { widgets.append(widget); }  
    void removeWidget(QWidget *widget) { widgets.removeAll(widget); }  
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);  
private:  
    QWidgetList widgets;  
};  
static bool resolveLibs()  
{  
    if (!pDwmIsCompositionEnabled) {  
        QLibrary dwmLib(QString::fromStdString("dwmapi"));  
        pDwmIsCompositionEnabled =(PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");  
        pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");  
        pDwmEnableAeroWindow = (PtrDwmEnableAeroWindow)dwmLib.resolve("DwmEnableAeroWindow");  
        pDwmGetColorizationColor = (PtrDwmGetColorizationColor)dwmLib.resolve("DwmGetColorizationColor");
		pDwmSetWindowAttribute = (PtrDwmSetWindowAttribute)dwmLib.resolve("DwmSetWindowAttribute");
    }  
    return pDwmIsCompositionEnabled != 0;  
}  
#endif  
/*! 
  * 检查 DWM 是否开启 
  * 
  */  
bool QtAeroWin::isCompositionEnabled()  
{  
#ifdef Q_WS_WIN  
    if (resolveLibs()) {  
        HRESULT hr = S_OK;  
        BOOL isEnabled = false;  
        hr = pDwmIsCompositionEnabled(&isEnabled);  
        if (SUCCEEDED(hr))  
            return isEnabled;  
    }  
#endif  
    return false;  
}  
/*! 
  * 对一个widget实现Aero效果. 
  * 
  */  
bool QtAeroWin::enableAeroWindow(QWidget *widget, bool enable)  
{  
    Q_ASSERT(widget);  
    bool result = false;  
#ifdef Q_WS_WIN  
    if (resolveLibs()) {  
		if(NULL== pDwmEnableAeroWindow){
			return false;
		}
        DWM_BLURBEHIND bb = {0};  
        HRESULT hr = S_OK;  
        bb.fEnable = enable;  
        bb.dwFlags = DWM_BB_ENABLE;  
        bb.hRgnBlur = NULL;  
        widget->setAttribute(Qt::WA_TranslucentBackground, enable);  
        widget->setAttribute(Qt::WA_NoSystemBackground, enable);  
        hr = pDwmEnableAeroWindow((HWND)widget->winId(), &bb);  
        if (SUCCEEDED(hr)) {  
            result = true;  
            windowNotifier()->addWidget(widget);  
        }  
    }  
#endif  
    return result;  
}  
/*! 
  * 设置Aero绘图区 
  */  
bool QtAeroWin::extendFrameIntoClientArea(QWidget *widget, int left, int top, int right, int bottom)  
{  
    Q_ASSERT(widget);  
    Q_UNUSED(left);  
    Q_UNUSED(top);  
    Q_UNUSED(right);  
    Q_UNUSED(bottom);  
    bool result = false;  
#ifdef Q_WS_WIN  
    if (resolveLibs()) { 
		if(NULL== pDwmExtendFrameIntoClientArea){
			return false;
		}
        QLibrary dwmLib(QString("dwmapi"));  
        HRESULT hr = S_OK;  
        MARGINS m = {left, top, right, bottom};  
        hr = pDwmExtendFrameIntoClientArea((HWND)widget->winId(), &m);  
        if (SUCCEEDED(hr)) {  
            result = true;  
            windowNotifier()->addWidget(widget);  
        }  
    }  
#endif  
    return result;  
}  
/*! 
  * 返回当前窗口颜色. 
  */  
QColor QtAeroWin::colorizatinColor()  
{  
    QColor resultColor = QApplication::palette().window().color();  
#ifdef Q_WS_WIN  
    if (resolveLibs()) { 
		if(NULL== pDwmGetColorizationColor){
			return resultColor;
		}
        DWORD color = 0;  
        BOOL opaque = FALSE;  
        QLibrary dwmLib(QString("dwmapi"));  
        HRESULT hr = S_OK;  
        hr = pDwmGetColorizationColor(&color, &opaque);  
        if (SUCCEEDED(hr))  
            resultColor = QColor(color);  
    }  
#endif  
    return resultColor;  
}  
#ifdef Q_WS_WIN  
WindowNotifier *QtAeroWin::windowNotifier()  
{  
    static WindowNotifier *windowNotifierInstance = 0;  
    if (!windowNotifierInstance)  
        windowNotifierInstance = new WindowNotifier;  
    return windowNotifierInstance;  
}

/* 所有窗口响应 DWM 状态变换消息 */  
bool WindowNotifier::nativeEvent(const QByteArray &eventType, void *message, long *result)  
{  
	MSG* msg =(MSG*)message;
    if (msg && msg->message == WM_DWMCOMPOSITIONCHANGED) {  
        bool compositionEnabled = QtAeroWin::isCompositionEnabled();  
        foreach(QWidget * widget, widgets) {  
            if (widget) {  
                widget->setAttribute(Qt::WA_NoSystemBackground, compositionEnabled);  
            }  
            widget->update();  
        }  
    }  
    return QWidget::nativeEvent(eventType,message, result);  
}  
#endif

/*
使用的时候需要在生成窗口的cpp中#include "QtAeroWin.h"
void func()
{
#ifndef Q_WS_WIN 
	window.setAttribute(Qt::WA_TranslucentBackground);  
	window.setAttribute(Qt::WA_NoSystemBackground, false);  
	QPalette pal = window.palette();  
	QColor bg = pal.window().color();  
	bg.setAlpha(180);  
	pal.setColor(QPalette::Window, bg);  
	window.setPalette(pal);  
	window.ensurePolished(); // workaround Oxygen filling the background  
	window.setAttribute(Qt::WA_StyledBackground, false);  
#else  
	if (QtWin::isCompositionEnabled()) {  
		QtWin::extendFrameIntoClientArea(&window);  
		window.setContentsMargins(0, 0, 0, 0);  
	}
#endif

	window.show(); 
}
*/