#include "sliderwidget.h"

SliderWidget::SliderWidget(QWidget *parent)
	: C8PopupWindow(parent,"")
{
	ui.setupUi(this);
	ui.verticalSlider->setMaximum(100);
	ui.verticalSlider->setMinimum(0);

	connect(ui.verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChange(int)));
}

SliderWidget::~SliderWidget()
{

}

void SliderWidget::setSliderValue(int value)
{
	ui.verticalSlider->setValue(value);	
}

int SliderWidget::getSliderValue()const
{
	return ui.verticalSlider->value();
}

void SliderWidget::valueChange(int value)
{
	emit sliderValueChanged(value);
}

void SliderWidget::setRange(int min,int max)
{
	ui.verticalSlider->setRange(min,max);
}