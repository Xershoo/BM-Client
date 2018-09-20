#ifndef MULITCAMERALISTITEMWIDGET_H
#define MULITCAMERALISTITEMWIDGET_H

#include <QWidget>
#include "ui_mulitcameralistitemwidget.h"

class MulitCameraListItemWidget : public QWidget
{
    Q_OBJECT
signals:
    void itemClicked(MulitCameraListItemWidget* item);

public:
    explicit MulitCameraListItemWidget(int type, QString name, int seat,int id,QWidget *parent = 0);
    ~MulitCameraListItemWidget();

    void setCameraSeat(int seat);

	inline int getCameraType()
	{
		return m_cameraType;
	}

	inline int getCameraId()
	{
		return m_cameraId;
	}

	inline int getCameraSeat()
	{
		return m_cameraSeat;
	}

protected slots:
    void labelClicked();

private:
    Ui::MulitCameraListItemWidget ui;
	int m_cameraType;
	int m_cameraId;
	int m_cameraSeat;
};

#endif // MULITCAMERALISTITEMWIDGET_H
