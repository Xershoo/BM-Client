#ifndef SLIDERWIDGET_H
#define SLIDERWIDGET_H

#include <QWidget>
#include "ui_sliderwidget.h"
#include "control/c8popupwindow.h"
#include <QDialog>

class SliderWidget : public C8PopupWindow
{
	Q_OBJECT
signals:
	void sliderValueChanged(int);

public:
	SliderWidget(QWidget *parent = 0);
	~SliderWidget();

	void setSliderValue(int value);
	int getSliderValue()const;

	void setRange(int min,int max);
private:
	Ui::SliderWidget ui;

private slots:
	void valueChange(int);

};

#endif // SLIDERWIDGET_H
