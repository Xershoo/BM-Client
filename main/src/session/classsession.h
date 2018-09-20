#pragma once

struct PublishParam;

#include <QString>

class ClassSeeion
{
public:
	static ClassSeeion* GetInst();
	static void FreeInst();
	bool	IsTeacher();
	bool	IsStudent();
    int     getAuthority();

    bool    getHeadImage(QString& imgFile);
private:
	ClassSeeion(void);
	~ClassSeeion(void);

private:
	static ClassSeeion*	_pHandle;
public:

	__int64			_nUserId;
	__int64			_nClassRoomId;
	__int64			_nCourseId;
	int				_nMyAuthority;
	bool			_bBeginedClass;
	int				_nInterBeginTime;
    
	//��ҳ��ͻ��˽������, bool, classid, courseid
	bool			m_loginBytokenUid;
	__int64			m_classID;
	__int64			m_courseID;
};

