#include "classsession.h"
#include "common/varname.h"
#include "biz/interface/interface.h"
#include "biz/interface/IUserInfoData.h"
#include "Env.h"
#include <QFileInfo>
#include <QPixmap>

ClassSeeion* ClassSeeion::_pHandle = NULL;

ClassSeeion::ClassSeeion(void)
{
	_nUserId = 0;
	_nClassRoomId = 1000;
	_nCourseId = 0;
	_nMyAuthority = 0;
	_bBeginedClass = false;
	_nInterBeginTime = 0;
	m_loginBytokenUid = false;
}

ClassSeeion::~ClassSeeion(void)
{

}

ClassSeeion* ClassSeeion::GetInst()
{
	if(_pHandle == NULL)
	{	
		_pHandle = new ClassSeeion;
	}
	return _pHandle;
}

void ClassSeeion::FreeInst()
{
	if(_pHandle)
		delete _pHandle;
	_pHandle = NULL;
}

bool ClassSeeion::IsTeacher()
{
	biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_nUserId);
	return (sMyInfo.nUserAuthority == biz::UserAu_Teacher) ? true : false;
}

bool ClassSeeion::IsStudent()
{
	biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_nUserId);
	return (sMyInfo.nUserAuthority == biz::UserAu_Student || sMyInfo.nUserAuthority == biz::UserAu_Assistant) ? true : false;
}

int ClassSeeion::getAuthority()
{
    biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_nUserId);
    return sMyInfo.nUserAuthority;
}

bool ClassSeeion::getHeadImage(QString& imgFile)
{
    biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(_nUserId);	

    QString imgHeader = Env::GetUserPicCachePath(_nUserId)+"user_head.jpg";
    QFileInfo fileHead(imgHeader);
    if(fileHead.exists())
    {
        QFile hdrFile(imgHeader);
        QImage headImage;
        
        hdrFile.open(QIODevice::ReadOnly);
        bool br = headImage.loadFromData(hdrFile.readAll());
        if(br&&!headImage.isNull())
        {
            imgFile = imgHeader;
            return true;
        }
    }

    int nFlag = (biz::UserAu_Teacher == myInfo.nUserAuthority) ? 1 : 0;
    if (biz::Gender_Male == myInfo.nGender)
    {
        imgHeader = QString("userListHeadIcon_%1.png").arg((nFlag) ? 0 : 1);
        imgHeader = Env::currentThemeResPath() + imgHeader;
    }
    else
    {
        imgHeader = QString("userListHeadIcon_%1.png").arg((nFlag) ? 2 : 3);
        imgHeader = Env::currentThemeResPath() + imgHeader;
    }

    imgFile = imgHeader;
    return false;
    
}