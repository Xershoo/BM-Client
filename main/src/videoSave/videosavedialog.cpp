#include "videosavedialog.h"
#include <QFileDialog>

VideoSaveDialog::VideoSaveDialog(QWidget *parent)
	: C8CommonWindow(parent),m_saveFilePath("")
{
	ui.setupUi(this);
	ui.edit_file_path->adjustSize();

	connect(ui.pushButton_Cancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.pushButton_OK, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui.pushButton_select, SIGNAL(clicked()), this, SLOT(showFileDlg()));

	resetContentsMargin(ui.gridLayout_2);
}

VideoSaveDialog::~VideoSaveDialog()
{

}

void VideoSaveDialog::setTitleBarRect()
{
	QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void VideoSaveDialog::close()
{
	QWidget::close();
}

void VideoSaveDialog::showFileDlg()
{
	//�ļ��Ի����ʽ����
	QString file = QFileDialog::getOpenFileName(
		this,
		"/home/foxman",
		"Images (*.png *.xpm *.jpg)"
		);
	m_saveFilePath = file;
	ui.edit_file_path->setText(m_saveFilePath);
}
void VideoSaveDialog::save()
{
	//�ж�·���Ƿ�Ϸ�

	//�ļ������¿�һ���̣߳�
	m_saveFilePath = ui.edit_file_path->text();

	this->close();
}
