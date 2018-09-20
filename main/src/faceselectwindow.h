#ifndef FACESELECTWINDOW_H
#define FACESELECTWINDOW_H
#include <QWidget>
#include <QMainWindow>
#include "ui_faceselectwindow.h"
#include "faceselectwindowscene.h"
#include <QLabel>
class FaceSelectWindow : public QWidget
{
    Q_OBJECT

signals:

	void sg_selectface(QString);

public:

    FaceSelectWindow(QWidget *parent = 0);
    ~FaceSelectWindow();
	void selectFaceWindowBtnClicked();

protected:
    bool eventFilter(QObject *, QEvent *);

protected slots:

    void sl_hoverEnterFaceItem(QPointF pos, QSizeF size, QString facePath);
    void sl_hoverLeaveFaceItem();
    void selectFace(QString facePath);

private:
    Ui::FaceSelectWindow ui;
    FaceSelectWindowScene m_scene;
	QLabel *m_labelFacePreview;
};

#endif // FACESELECTWINDOW_H
