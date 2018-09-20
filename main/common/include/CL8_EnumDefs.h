//CL8_EnumDefs.h

#pragma once

#define  nWhiteBoardViewWBIdBegin  50

typedef enum _EnumLoginStyle
{
	UserLogStyle_None = 0,	//默认，记住用户名
	UserLogStyle_Bar,		//网吧模式
	UserLogStyle_PW,		//记住密码
	UserLogStyle_Auto,		//自动登录
}EnmuserLoginStyle;


typedef enum _LOGIN_STATUS
{
	LOGIN_SUCCESS,								// 登录成功
	LOGIN_TIMEOUT,								// 登录超时
	LOGIN_KICKOFF,								// 被踢出
	LOGIN_LOGOUT,								// 正常退出
	LOGIN_RECONNECT,							// 登录重连
	LOGIN_ERROR,								// 登录出错
	LOGIN_ING,									// 正在登录
	LOGIN_NEEDREGISTER,							//需要激活账号
	LOGIN_REGISTER_FAILED,						//账号激活失败

	LOGIN_FRI_RETRY,							// 重新登录好友
	LOGIN_FRI_OFFLINE,							// 离线方式登录好友
}LOGIN_STATUS;