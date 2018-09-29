#ifndef QTWIN_AERO_H  
#define QTWIN_AERO_H  
#include <QColor>  
#include <QWidget>

#define Q_WS_WIN
#include <qt_windows.h> 

class WindowNotifier;  
class QtAeroWin
{  
public:  
    static bool enableAeroWindow(QWidget *widget, bool enable = true);  
    static bool extendFrameIntoClientArea(QWidget *widget,  
                                          int left = -1, int top = -1,  
                                          int right = -1, int bottom = -1);  
    static bool isCompositionEnabled();  
    static QColor colorizatinColor();  
private:  
    static WindowNotifier *windowNotifier();  
};  
#endif // QTWIN_AERO_H  
 
