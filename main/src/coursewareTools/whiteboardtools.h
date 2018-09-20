#ifndef WHITEBOARDTOOLS_H
#define WHITEBOARDTOOLS_H

#include <QWidget>
#include "ui_whiteboardtools.h"
#include "qcoursewarepreview.h"
#include "qselectcolor.h"
#include "qselecttext.h"

class whiteboardtools : public QWidget
{
    Q_OBJECT 

public:
    whiteboardtools(QWidget *parent = 0);
    ~whiteboardtools();
    bool  releaseViewData();
    void  showUI(bool bISShow);
    void  setRegion();

public slots:
    void ShowPreview(QString filePath, int npage);
    void RemovePreview(QString filePath);

signals:
    void show_stu_video_list();

protected slots:
    void clickBtnUndo();
    void clickBtnText();
    void clickBtnPreview();
    void clickBtnPen();
    void clickBtnMove();
    void clickBtnEraser();
    void clickBtnColor();
    void clickBtnClearAll();

    void leftSlideHideLabelClicked();
    void showStuVideoListWndBtnClicked();
    void ShowNowColor(int nColor);
private:
    Ui::whiteboardtools ui;

protected:
    QCoursewarePreview m_coursewarePreview;
    QSelectColor       m_selectColor;
    QSelectText        m_selectText;
};

#endif // WHITEBOARDTOOLS_H
