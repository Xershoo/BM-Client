#pragma once

#include "Login_p.h"
#include "LoginRet_p.h"
#include "TokenValidate_p.h"
#include "TokenValidateResp_p.h"
#include "QueryUser_p.h"
#include "ReturnUser_p.h"
#include "QueryUserListResp_p.h"
#include "QueryUserEdu_p.h"
#include "ReturnUserEdu_p.h"
#include "QueryCourse_p.h"
#include "ReturnCourse_p.h"
#include "QueryToken_p.h"
#include "ReturnToken_p.h"
#include "FeedBack_p.h"
#include "QueryPreCWares_p.h"
#include "QueryPreCWaresResp_p.h"
#include "MesgReq_p.h"
#include "MesgResp_p.h"
#include "QueryAutoLoginToken_p.h"
#include "AutoLoginTokenRsp_p.h"
#include "UserEnter_p.h"
#include "UserLeave_p.h"
#include "MesgReminder_p.h"
#include "EvaluateClass_p.h"
#include "UserWelcome_p.h"
#include "SetMainShow_p.h"
#include "SwitchClassShow_p.h"
#include "CreateWhiteBoard_p.h"
#include "AddCourseWare_p.h"
#include "CreateTalkGroup_p.h"
#include "SetClassState_p.h"
#include "SetClassMode_p.h"
#include "SendTextMsg_p.h"
#include "ClassAction_p.h"
#include "WhiteBoardEvent_p.h"
#include "Error_p.h"
#include "KickOut_p.h"
#include "NotifyVideoList_p.h"
#include "SetTeacherVedio_p.h"
#include "RefreshVideoListReq_p.h"
#include "RefreshVideoListResp_p.h"
#include "MobileConnectClassReq_p.h"
#include "MobileConnectClassResp_p.h"
#include "UpdateCodeReq_p.h"
#include "UpdateCodeResp_p.h"
#include "ChooseMobile_p.h"
#include "ChooseMobileResp_p.h"
#include "Kick_p.h"
#include "PromoteAsAssistant_p.h"
#include "CancelAssistant_p.h"
#include "ClassGoOn_p.h"
#include "ClassDelayTimeOut_p.h"


namespace rpcgen_edu
{
	void RegisterProtocols();
	void UnregisterProtocols();
	std::string FindProtocolName(unsigned int type);
} 