#include "biz.h"
#include "convert.h"
#include "gnet/socket/net.h"
#include "protocol/rpcgen_edu.h"
#include "gnet/type_convert.h"
#include "log_biz.h"
#include <atlstr.h>
#include <string>

#include "../../config.h"

namespace biz
{
	void Biz::ProcessProtocol(gnet::Aio::SID sid, gnet::Protocol* p)
	{
		switch (p->GetType())
		{		
		case gnet::LoginRet::PROTOCOL_TYPE:
			{
				gnet::LoginRet* pp = (gnet::LoginRet*)p;

				if(pp->retcode_ == 0)
				{
					std::string strServ = gnet::oct2str(pp->link_);
					int nPos = strServ.find(':',0);
					
					std::string strIp = strServ.substr(0,nPos);
					nPos ++;
					std::string strPort = strServ.substr(nPos,strServ.length() - nPos);

					onLogin(pp->retcode_,pp->userid_,gnet::oct2wstr(pp->token_).c_str(),
						strIp.c_str(),strPort.c_str(),pp->netisp_);

					if(_callback)
					{
						_callback->OnLogin(pp->userid_, pp->usertype_);
					}
				}
				else
				{
					onLogin(pp->retcode_,0,NULL,NULL,NULL,0);

					if(_callback)
					{
						_callback->OnLoginError(pp->retcode_);
					}
				}
			}
			break;

		case gnet::TokenValidateResp::PROTOCOL_TYPE:
			{
				gnet::TokenValidateResp* pp = (gnet::TokenValidateResp*)p;
				if(pp->retcode_ == 0)
				{
					QueryUserInfo(_userLogin.nUserId);
					
					_timesRelogin = 0;
					_relogin = false;
					_userLogin.nState = USER_VALIDATE_OK;
				}
				else
				{
					//try login again......
					autoRelogin();
				}
			}
			break;
		case gnet::KickOut::PROTOCOL_TYPE:
			{
				gnet::KickOut* pp = (gnet::KickOut*)p;
				if(_callback)
				{
					_callback->OnLogKickOut(pp->userid_);
				}
			}
			break;
		case gnet::ReturnUserEdu::PROTOCOL_TYPE:
			{
				gnet::ReturnUserEdu* pp = (gnet::ReturnUserEdu*)p;
				SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp->userid_);
				sInfo.nUserId = pp->userid_;
				sInfo._nEduInfoSize = pp->eduinfolist_.size();
				if(sInfo._nEduInfoSize > MAXEDUINFO)
				{
					sInfo._nEduInfoSize = MAXEDUINFO;
				}

				for (int i = 0; i < sInfo._nEduInfoSize; i++)
				{
					wcscpy_s(sInfo._sEduInfo[i].szSchoolName, gnet::oct2wstr(pp->eduinfolist_[i].university_).c_str());
					wcscpy_s(sInfo._sEduInfo[i].szCollege, gnet::oct2wstr(pp->eduinfolist_[i].college_).c_str());
					wcscpy_s(sInfo._sEduInfo[i].szGrade, gnet::oct2wstr(pp->eduinfolist_[i].grade_).c_str());
					wcscpy_s(sInfo._sEduInfo[i].szMajor, gnet::oct2wstr(pp->eduinfolist_[i].majorr_).c_str());
					wcscpy_s(sInfo._sEduInfo[i].szStudentNo, gnet::oct2wstr(pp->eduinfolist_[i].studentno_).c_str());
					sInfo._sEduInfo[i].nVerifyStatus = pp->eduinfolist_[i].verifystatus_;
				}
				_userinfoContainer.UpdateUserINfo(sInfo);
			}
			break;

		case gnet::UserEnter::PROTOCOL_TYPE:
			{
				gnet::UserEnter& pp = dynamic_cast<gnet::UserEnter&>(*p);
				SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp.userinfo_.userid_);

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp.classid_);
				if(pRoom == NULL)
				{
					return;
				}

				LOG << "Receive user enter message:";

				SLClassRoomBaseInfo roomBaseInfo = pRoom->GetRoomBaseInfo();

				LOG <<"uid:"<<pp.userinfo_.userid_<<" cid:"<< pp.cid_ << " real name:" <<gnet::oct2wstr(pp.userinfo_.realname_).c_str(); 
				LOG << " nick name:" << gnet::oct2wstr(pp.userinfo_.nickname_).c_str();
				LOG << " pull url: " << gnet::oct2str(pp.userinfo_.pulladdr_).c_str();
				LOG << " push url: " << gnet::oct2str(pp.userinfo_.pushaddr_).c_str();

				if(sInfo.nUserId != _userLogin.nUserId) //self no overwrite
				{
					wcscpy_s(sInfo.szRealName, gnet::oct2wstr(pp.userinfo_.realname_).c_str());
					wcscpy_s(sInfo.szNickName, gnet::oct2wstr(pp.userinfo_.nickname_).c_str());					
					wcscpy_s(sInfo.szSignature, gnet::oct2wstr(pp.userinfo_.signature_).c_str());
					wcscpy_s(sInfo.szPicUrl, gnet::oct2wstr(pp.userinfo_.pic_).c_str());
					sInfo.nGender = pp.userinfo_.gender_;
					sInfo._nBanType = pp.userinfo_.bantype_;
					sInfo.nDevice = pp.device_;
					wcscpy_s(sInfo.szMobile, gnet::oct2wstr(pp.userinfo_.mobile_).c_str());
					wcscpy_s(sInfo.szEmail, gnet::oct2wstr(pp.userinfo_.email_).c_str());
					sInfo._nUserState = pp.userinfo_.state_;
				}				
				
				sInfo._nClassRoomId = pp.classid_;
				sInfo.nUserAuthority = (EUserAuthority)pp.userinfo_.authority_;
				wcscpy_s(sInfo.szPullUrl, ATL::CString(gnet::oct2str(pp.userinfo_.pulladdr_).c_str()));
				wcscpy_s(sInfo.szPushUrl, ATL::CString(gnet::oct2str(pp.userinfo_.pushaddr_).c_str()));				
								
				_userinfoContainer.AddUserInfo(sInfo);
				pRoom->AddUser(sInfo.nUserId);

				if(_callback )
				{
					_callback->OnUserEnter(sInfo);
				}				
			}
			break;
		case gnet::UserWelcome::PROTOCOL_TYPE:
			{
				gnet::UserWelcome& pp = dynamic_cast<gnet::UserWelcome&>(*p);
				int nCode = pp.ret_;

				if(nCode !=0)
				{
					if(_callback)
					{
						_callback->OnEnterClassError(pp.classid_,nCode);
					};

					break;
				}

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp.classid_);
				if(pRoom == NULL)
				{
					break;
				};

				SLClassRoomBaseInfo& sRoomInfo = pRoom->GetRoomBaseInfo();				
				sRoomInfo._nCourseId = pp.cid_;
                sRoomInfo._nRoomId = pp.classid_;
				sRoomInfo._nClassState = pp.classstate_;
				sRoomInfo._nClassMode = pp.classmode_;
                sRoomInfo._nBeforeTime = pp.timebeforeclass_;
				sRoomInfo._nProcessTime = pp.durationtime_;
				sRoomInfo._nTeacherVideo = pp.teachervedio_;
				sRoomInfo._feedback = pp.feedback_;
                sRoomInfo._mainshow = pp.mainshow_;
                
				wcscpy_s(sRoomInfo._szRoomName,gnet::oct2wstr(pp.cname_).c_str());
				wcscpy_s(sRoomInfo._szUserHeadUrl,gnet::oct2wstr(pp.userheadurl_).c_str());
				
				ATL::CStringA  strCode(gnet::oct2str(pp.code_).c_str());
				wcscpy_s(sRoomInfo._szDevAccessCode,ATL::CStringW(strCode));
								
				for (size_t i = 0; i < pp.topvideolist_.size(); i++)
				{
					pRoom->TopVideoListOpt(pp.topvideolist_[i], 0, 0);
				}
				
				__int64 nTeatchId = 0;
				__int64 nRoom = pp.classid_;
				SLClassRoomShowInfo sShowInfo;
				sShowInfo._nRoomId = pp.classid_;
				sShowInfo._nShowType = pp.currentshow_.showtype_;
				wcscpy_s(sShowInfo._szShowName, ATL::CString(gnet::oct2str(pp.currentshow_.name_).c_str()));
				sShowInfo._nShowPage = pp.currentshow_.page_;
				pRoom->UpdateShowInfo(sShowInfo);

				if(_callback)
				{
					_callback->OnEnterClassError(pp.classid_,0);
				};

				LOG << "Receive welcome message user information:"; 
				for (size_t i = 0; i < pp.userlist_.size(); i++)
				{
					bool bExist = false;
					SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp.userlist_[i].userid_,&bExist);
					
					LOG <<"uid:"<< pp.userlist_[i].userid_<<" cid:"<< pp.cid_ << " real name:" <<gnet::oct2wstr(pp.userlist_[i].realname_).c_str() ;
					LOG	<< " nick name:" << gnet::oct2wstr(pp.userlist_[i].nickname_).c_str();
					LOG << " pull url: " << gnet::oct2str(pp.userlist_[i].pulladdr_).c_str();
					LOG << " push url: " << gnet::oct2str(pp.userlist_[i].pushaddr_).c_str();					

					if(sInfo.nUserId != _userLogin.nUserId)		// self no overwrite
					{
						if(!bExist)
						{
							wcscpy_s(sInfo.szRealName, gnet::oct2wstr(pp.userlist_[i].realname_).c_str());
							wcscpy_s(sInfo.szNickName, gnet::oct2wstr(pp.userlist_[i].nickname_).c_str());

							wcscpy_s(sInfo.szSignature, gnet::oct2wstr(pp.userlist_[i].signature_).c_str());
							wcscpy_s(sInfo.szPicUrl, gnet::oct2wstr(pp.userlist_[i].pic_).c_str());
							sInfo.nGender = pp.userlist_[i].gender_;
							sInfo.nDevice = pp.userlist_[i].device_;
							sInfo._nBanType = pp.userlist_[i].bantype_;
							wcscpy_s(sInfo.szMobile, gnet::oct2wstr(pp.userlist_[i].mobile_).c_str());
							wcscpy_s(sInfo.szEmail, gnet::oct2wstr(pp.userlist_[i].email_).c_str());
						}

						wcscpy_s(sInfo.szPullUrl, ATL::CString(gnet::oct2str(pp.userlist_[i].pulladdr_).c_str()));
						wcscpy_s(sInfo.szPushUrl, ATL::CString(gnet::oct2str(pp.userlist_[i].pushaddr_).c_str()));
					}
					else
					{
						wcscpy_s(sInfo.szPullUrl,L"");
						wcscpy_s(sInfo.szPushUrl,L"");
					}
					
					sInfo.nUserAuthority = (EUserAuthority)pp.userlist_[i].authority_;
					sInfo._nClassRoomId = pp.classid_;
					sInfo._nUserState = pp.userlist_[i].state_;
					_userinfoContainer.UpdateUserINfo(sInfo);
					
					pRoom->AddUser(sInfo.nUserId);

					if(sInfo.nUserAuthority == biz::UserAu_Teacher)
					{
						nTeatchId= sInfo.nUserId;
                        sRoomInfo._nTeacherId = nTeatchId;
					}

					if(_callback)
					{
						_callback->OnUserEnter(sInfo);						
					}
				}

				if(_callback)
				{
					_callback->ClassStateChange(sRoomInfo._nCourseId,sRoomInfo._nRoomId,sRoomInfo._nClassState,nTeatchId,0);
				}

				for (size_t i = 0; i < pp.cousewarelist_.size(); i++)
				{
					pRoom->AddCouseware(std::wstring(ATL::CString(gnet::oct2str(pp.cousewarelist_[i]).c_str())));

					if(_callback)
					{
						SLCursewaveListOpt slCWLOpt;
						slCWLOpt._nRoomId = pp.classid_;

						wcscpy_s(slCWLOpt._szFileName, gnet::oct2wstr(pp.cousewarelist_[i]).c_str());
						slCWLOpt._nUserId = nTeatchId;
						slCWLOpt._nOpt = biz::ECursewaveOpt_ADD;

						if(_callback)
						{
							_callback->OnCursewaveListEvent(slCWLOpt);
						}
					}
				}
								
				for( size_t i= 0 ; i< pp.whiteboard_.size();i++)
				{
					SLWhiteBoardEvent sEventInfo;
					sEventInfo._nRoomId = pp.whiteboard_[i].cid_;
					sEventInfo._nUserId = pp.whiteboard_[i].userid_;
					sEventInfo._nOwerUserId = pp.whiteboard_[i].oweruid_;				
					sEventInfo._nPageId = pp.whiteboard_[i].pageId_;
					sEventInfo._nPaintId = pp.whiteboard_[i].paintId_;
					sEventInfo._nPainType = (PainType)pp.whiteboard_[i].paintype_;
					sEventInfo._strParam = gnet::oct2str(pp.whiteboard_[i].arguments_);

					if(_callback)
					{
						_callback->OnWhiteBoardEvent(sEventInfo);
					}
				}

				if(sShowInfo._nShowType != gnet::ShowInfo::BLANK)
				{
					if(_callback)
					{
						_callback->ShowTypeChange(sShowInfo);
					}
				}

                if(_callback)
                {
                    _callback->MainShowChange(sRoomInfo._mainshow,sRoomInfo._nRoomId,sRoomInfo._nTeacherId);
                }
			}
			break;
		case gnet::ReturnUser::PROTOCOL_TYPE:
			{
				gnet::ReturnUser& pp = dynamic_cast<gnet::ReturnUser&>(*p);
				bool userExsit = false;
				SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp.userinfo_.userid_,&userExsit);
				sInfo.nUserId = pp.userinfo_.userid_;
				if(!userExsit) {
					sInfo.nUserAuthority = (EUserAuthority)pp.userinfo_.authority_;
				}

				wcscpy_s(sInfo.szNickName,gnet::oct2wstr(pp.userinfo_.nickname_).c_str());
				wcscpy_s(sInfo.szRealName,gnet::oct2wstr(pp.userinfo_.realname_).c_str());
				wcscpy_s(sInfo.szSignature,gnet::oct2wstr(pp.userinfo_.signature_).c_str());
				wcscpy_s(sInfo.szPicUrl,gnet::oct2wstr(pp.userinfo_.pic_).c_str());
				wcscpy_s(sInfo.szMobile,gnet::oct2wstr(pp.userinfo_.mobile_).c_str());
				wcscpy_s(sInfo.szEmail,gnet::oct2wstr(pp.userinfo_.email_).c_str());
				sInfo._nBanType = pp.userinfo_.bantype_;
				sInfo.nGender = pp.userinfo_.gender_;
				sInfo._nUserState = pp.userinfo_.state_;
				sInfo.nDevice = pp.userinfo_.device_;

				_userinfoContainer.UpdateUserINfo(sInfo);
				if(_callback)
				{
					_callback->OnUserInfoReturn(sInfo);
				}
				
			}
			break;
		case gnet::UserLeave::PROTOCOL_TYPE:
			{
				gnet::UserLeave& pp = dynamic_cast<gnet::UserLeave&>(*p);
				if(_callback)
				{
					_callback->OnUserLever(pp.userid_, pp.cid_, 0);
				}

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp.cid_);
				pRoom->DelUser(pp.userid_);
				if(_userLogin.nUserId != pp.userid_)
				{
					_userinfoContainer.DeleteUser(pp.userid_);
				}
			}
			break;
		case gnet::SwitchClassShow::PROTOCOL_TYPE:
			{
				gnet::SwitchClassShow& pp = dynamic_cast<gnet::SwitchClassShow&>(*p);
				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp.cid_);
				if(pRoom == NULL)
				{
					return;
				}

				SLClassRoomShowInfo sShowInfo;
				sShowInfo._nRoomId = pp.cid_;
				sShowInfo._nShowType = pp.currentshow_.showtype_;
				wcscpy_s(sShowInfo._szShowName, ATL::CString(gnet::oct2str(pp.currentshow_.name_).c_str()));
				sShowInfo._nShowPage = pp.currentshow_.page_;
				pRoom->UpdateShowInfo(sShowInfo);
				if(_callback)
				{
					_callback->ShowTypeChange(sShowInfo);
				}
			}
			break;
		case gnet::SetClassState::PROTOCOL_TYPE:
			{
				gnet::SetClassState* pp = dynamic_cast<gnet::SetClassState*>(p);
				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->classid_);
				if(pRoom == NULL)
				{
					return;
				}
				
				char nState = biz::Eclassroomstate_Free;
				if(pp->classstate_ == gnet::SetClassState::CLASS_BEGIN)
				{
					nState = biz::EClassroomstate_Doing;
				}
				else if(pp->classstate_ == gnet::SetClassState::CLASS_END)
				{
					nState = biz::Eclassroomstate_Free;
				}
				else
				{
					return;
				}

				if(pp->ret_ == 0)
				{
					if(nState == biz::Eclassroomstate_Free)
					{
						auto pSpeakInfo = pRoom->GetSpeakUserInfo();	
						if(pSpeakInfo)
						{
							pSpeakInfo->_nUserState  = pSpeakInfo->_nUserState & (~biz::eUserState_speak);
							_callback->OnUserSpeakAction(pp->cid_, pSpeakInfo->nUserId, biz::eUserspeekstate_clean_speak);
						}
					}

					pRoom->SetClassState(nState);
				}
				
				if(_callback)
				{
					_callback->ClassStateChange(pp->cid_, pp->classid_,nState, pp->userid_,pp->ret_);
				}
			}
			break;
		case gnet::CreateWhiteBoard::PROTOCOL_TYPE:
			{
				gnet::CreateWhiteBoard* pp = dynamic_cast<gnet::CreateWhiteBoard*>(p);
				SLWhiteBoardOpt sWhiteBoardOpt;
				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
				if(pRoom == NULL)
				{
					return;
				}

				sWhiteBoardOpt._nOpt = (EWhiteBoard_Opt)pp->actionytype_;
				sWhiteBoardOpt._nUserId = pp->userid_;
				sWhiteBoardOpt._nRoomId = pp->cid_;
				sWhiteBoardOpt._nPageId = pp->wbid_;
				wcscpy_s(sWhiteBoardOpt._szwbName,gnet::oct2wstr(pp->wbname_).c_str());
				pRoom->UpdateWhiteBoardOpt(sWhiteBoardOpt);
				if(_callback)
				{
					_callback->OnWhiteBoardOpt(sWhiteBoardOpt);
				}
			}
			break;
		case gnet::AddCourseWare::PROTOCOL_TYPE:		//添加、删除课件
			{
				gnet::AddCourseWare& pp =  dynamic_cast<gnet::AddCourseWare&>(*p);
				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp.cid_);
				if(pRoom== NULL)
				{
					return;
				}

				if(pp.actiontype_ == gnet::AddCourseWare::ADD)
				{
					pRoom->AddCouseware(gnet::oct2wstr(pp.cwname_));
				}
				else
				{
					pRoom->DelCouseware(gnet::oct2wstr(pp.cwname_));
				}

				if(_callback)
				{
					SLCursewaveListOpt slCWLOpt;
					slCWLOpt._nRoomId = pp.cid_;

					wcscpy_s(slCWLOpt._szFileName, gnet::oct2wstr(pp.cwname_).c_str());
					slCWLOpt._nUserId = pp.userid_;
					slCWLOpt._nOpt = pp.actiontype_;

					if(_callback)
					{
						_callback->OnCursewaveListEvent(slCWLOpt);
					}
				}
			}
			break;
		case gnet::WhiteBoardEvent::PROTOCOL_TYPE:
			{
				gnet::WhiteBoardEvent& pp = dynamic_cast<gnet::WhiteBoardEvent&>(*p);
				SLWhiteBoardEvent sEventInfo;
				sEventInfo._nRoomId = pp.action_.cid_;
				sEventInfo._nUserId = pp.action_.userid_;
				sEventInfo._nOwerUserId = pp.action_.oweruid_;				
				sEventInfo._nPageId = pp.action_.pageId_;
				sEventInfo._nPaintId = pp.action_.paintId_;
				sEventInfo._nPainType = (PainType)pp.action_.paintype_;
				sEventInfo._strParam = gnet::oct2str(pp.action_.arguments_);
				if(_callback)
				{
					_callback->OnWhiteBoardEvent(sEventInfo);
				}
			}
			break;

		case  gnet::SendTextMsg::PROTOCOL_TYPE:
			{
				gnet::SendTextMsg* pp = dynamic_cast<gnet::SendTextMsg*>(p);
				SLTextMsgInfo sMsgInfo;
				sMsgInfo._nClassRoomId = pp->cid_;
				sMsgInfo._nSendUserId = pp->userid_;
				sMsgInfo._nRecvUserId = pp->recvid_;
				sMsgInfo._nMsgType = (ETextMsgType)pp->recvtype_;
				sMsgInfo._nGroupId = pp->recvgroupid_;
				sMsgInfo._nTimer = pp->time_;
				std::wstring strmsg(gnet::oct2wstr(pp->message_));
				sMsgInfo._nMsgLen = strmsg.length();
				memcpy(sMsgInfo._szMsg, strmsg.c_str(), sMsgInfo._nMsgLen * sizeof(wchar_t));
				
				if(_callback)
				{
					_callback->OnRecvClassMessage(sMsgInfo);
				}
			}
			break;
		case gnet::CreateTalkGroup::PROTOCOL_TYPE:
			{
				gnet::CreateTalkGroup* pp = dynamic_cast<gnet::CreateTalkGroup*>(p);
				if(pp->ret_ == 1)
				{
					return;
				}

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
				if(pRoom == NULL)
				{
					return;
				}

				if(pp->actionytype_ == eCreateTalkGroup_start)
				{
					for (auto iter = pp->grouplist_.begin(); iter != pp->grouplist_.end(); iter++)
					{
						for (auto iterList = iter->second.uidlist_.begin(); 
									iterList != iter->second.uidlist_.end(); iterList++)
						{
							pRoom->AddDiscussUser(iter->first, *iterList);
						}
					}
				}
				else if(pp->actionytype_ == eCreateTalkGroup_end)
				{
					pRoom->ClearDiscussUser();
				}
				
				if(_callback)
					_callback->OnDiscussGroupOpt((int)pp->actionytype_, pp->cid_,gnet::oct2wstr(pp->topic_).c_str());
			}
			break;
		case gnet::ClassAction::PROTOCOL_TYPE:
			{
				gnet::ClassAction* pp = dynamic_cast<gnet::ClassAction*>(p);	
				int nUserState = _userinfoContainer.GetUserState(pp->userid_);
				if(_callback)
				{
					if(pp->actiontype_== gnet::ClassAction::ADD_STUDENT_VIDEO)
					{
						CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
						pRoom->TopVideoListOpt(pp->userid_, 0, 0);
						_callback->OnTopVideoChange(pp->cid_, pp->userid_, pp->teacheruid_, true);
					}
					else if(pp->actiontype_ == gnet::ClassAction::DEL_STUDENT_VIDEO)
					{							
						CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
						pRoom->TopVideoListOpt(pp->userid_, 1, 0);		
						_callback->OnTopVideoChange(pp->cid_, pp->userid_, pp->teacheruid_, false);
					}
					else if(pp->actiontype_ == gnet::ClassAction::ENTER_GROUP ||
						    pp->actiontype_ == gnet::ClassAction::LEAVE_GROUP)
							_callback->OnTeacherEnterDiscussGroup(pp->cid_, pp->userid_, pp->teacheruid_, pp->actiontype_);
					else if(pp->actiontype_ == gnet::ClassAction::CALL_TEACHER)
					{
						_callback->OnGroupCallTeacher(pp->cid_, pp->userid_);
					}
					else if(pp->actiontype_ == gnet::ClassAction::KICKOUT)
					{
						_callback->OnUserLever( pp->userid_,pp->cid_,0);
					}
					else 
					{
						if(pp->actiontype_ == gnet::ClassAction::ASK_SPEAK)
							nUserState = nUserState | eUserState_ask_speak;
						else if( pp->actiontype_ == gnet::ClassAction::CANCEL_SPEAK )
						{
							nUserState = nUserState & (~eUserState_speak);
							nUserState = nUserState & (~eUserState_ask_speak);
						}
						else if(pp->actiontype_ == gnet::ClassAction::CLEAN_SPEAK)
						{
							nUserState = nUserState & (~eUserState_speak);
							nUserState = nUserState & (~eUserState_ask_speak);
							if(pp->userid_ == 0)
								_userinfoContainer.CleanStudentsAskSpeak();
						}
						else if(pp->actiontype_ == gnet::ClassAction::ALLOW_SPEAK)
						{
							nUserState = nUserState & (~eUserState_ask_speak);
							nUserState = nUserState | eUserState_speak;
							CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
							auto pNowSpeakInfo = pRoom->GetSpeakUserInfo();
							if(pNowSpeakInfo)
							{
								pNowSpeakInfo->_nUserState = pNowSpeakInfo->_nUserState & (~eUserState_speak);
								_callback->OnUserSpeakAction(pp->cid_, pNowSpeakInfo->nUserId, gnet::ClassAction::CLEAN_SPEAK);
							}
						}
						else if(pp->actiontype_ == gnet::ClassAction::OPEN_VOICE)
							nUserState = nUserState | eUserState_user_voice;
						else if(pp->actiontype_ == gnet::ClassAction::CLOSE_VOICE)
							nUserState = nUserState & (~eUserState_user_voice);
						else if(pp->actiontype_ == gnet::ClassAction::OPEN_VIDEO)
							nUserState = nUserState | eUserState_user_video;
						else if(pp->actiontype_ == gnet::ClassAction::CLOSE_VIDEO)
						{
							nUserState = nUserState & (~eUserState_user_video);
							CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
							pRoom->TopVideoListOpt(pp->userid_, 1, 0);		
							//_callback->OnTopVideoChange(pp->cid_, pp->userid_, pp->teacheruid_, false);
						}
						else if(pp->actiontype_ == gnet::ClassAction::MUTE)
						{
							nUserState = nUserState | eUserState_user_mute;
						}
						else if(pp->actiontype_ == gnet::ClassAction::UNMUTE)
						{
							nUserState = nUserState & (~eUserState_user_mute);
						}

						_userinfoContainer.SetUserState(pp->userid_, nUserState);
						_callback->OnUserSpeakAction(pp->cid_, pp->userid_, pp->actiontype_);
					}
				}
			}
			break;
		
		case gnet::ReturnCourse::PROTOCOL_TYPE:
			{
				gnet::ReturnCourse* pp = dynamic_cast<gnet::ReturnCourse*>(p);				
				
				SLCourseInfo sci;

				sci._nCourseId = pp->course_.courseid_;				
				wcscpy_s(sci._szCourseName, gnet::oct2wstr(pp->course_.name_).c_str());
				wcscpy_s(sci._szCourseDesc, gnet::oct2wstr(pp->course_.description_).c_str());
				wcscpy_s(sci._szUserHeadUrl, gnet::oct2wstr(pp->course_.userheadurl_).c_str());
				wcscpy_s(sci._szCourseCoverUrl, gnet::oct2wstr(pp->course_.coverurl_).c_str());
				wcscpy_s(sci._szSnaptShotUrl, gnet::oct2wstr(pp->course_.snapshoturl_).c_str());
				sci._nCompulSoryType = pp->course_.compulsorytype_;
				sci._nCourseCredit = pp->course_.coursecredit_;
				sci._nTeacherId = pp->course_.teacherid_;
				wcscpy_s(sci._szHostUniversity, gnet::oct2wstr(pp->course_.hostuniversity_).c_str());
				wcscpy_s(sci._szHostCollege, gnet::oct2wstr(pp->course_.hostcollege_).c_str());
				wcscpy_s(sci._szHostMajor, gnet::oct2wstr(pp->course_.hostmajor_).c_str());
				wcscpy_s(sci._szSchoolYear, gnet::oct2wstr(pp->course_.schoolyear_).c_str());
				wcscpy_s(sci._szSchoolTerm, gnet::oct2wstr(pp->course_.schoolterm_).c_str());
				sci._nOnlineType = pp->course_.onlinetype_;
				sci._nOpenType = pp->course_.opentype_;
				wcscpy_s(sci._szOfflineClassroomAddress, gnet::oct2wstr(pp->course_.offline_classroomaddress_).c_str());
				sci._nAsideType = pp->course_.asidetype_;
				sci._nMaxAguestCount = pp->course_.max_aguestcount_;
				sci._nGuestType = pp->course_.guesttype_;
				sci._nMaxAguestCount = pp->course_.max_aguestcount_;
				
				int i = 0;
				for(int i=0;i<pp->course_.studentsid_.size();i++)
				{
					sci._vecStudentsId.push_back(pp->course_.studentsid_[i]);
				}
				
				SLClassRoomBaseInfo cbi;
				cbi._nRoomId = sci._nCourseId;
				cbi._nAllowAside = sci._nAsideType;
				cbi._nMaxAsideNum = sci._nMaxAsideCount;
				cbi._nAllowTry = sci._nGuestType;
				cbi._nMaxTryNum = sci._nMaxAguestCount;
				cbi._nClassMode = sci._nOnlineType;
				cbi._nTeacherId = sci._nTeacherId;
				wcscpy_s(cbi._szRoomName,sci._szCourseName);
				
				_classroomContainer.UpdateRoomBaseInfo(cbi);
				
				_courseinfoContainer.UpdateCourseInfo(sci);
				_callback->OnRecvCourseInfo(sci._nCourseId);

			};

			break;
		case gnet::QueryUserListResp::PROTOCOL_TYPE:				//课堂的用户详细信息
			{
				gnet::QueryUserListResp * pp = dynamic_cast<gnet::QueryUserListResp*>(p);

				LOG << "Receive user list message:"; 
					
				for (size_t i = 0; i < pp->userlist_.size(); i++)
				{
					LOG <<"uid:"<<pp->userlist_[i].userid_<<" cid:"<< pp->cid_;
					LOG << " real name:" <<gnet::oct2wstr(pp->userlist_[i].realname_).c_str(); 
					LOG	<< " nick name:" << gnet::oct2wstr(pp->userlist_[i].nickname_).c_str();
					LOG << " pull url: " << gnet::oct2str(pp->userlist_[i].pulladdr_).c_str();
					LOG << " push url: " << gnet::oct2str(pp->userlist_[i].pushaddr_).c_str();	

					SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp->userlist_[i].userid_);

					wcscpy_s(sInfo.szNickName, gnet::oct2wstr(pp->userlist_[i].nickname_).c_str());
					wcscpy_s(sInfo.szRealName, gnet::oct2wstr(pp->userlist_[i].realname_).c_str());
					/* no overwrite
					wcscpy_s(sInfo.szPullUrl, ATL::CString(gnet::oct2str(pp->userlist_[i].pulladdr_).c_str()));

					sInfo.nUserId = pp->userlist_[i].userid_;
					sInfo.nUserAuthority = (EUserAuthority)pp->userlist_[i].authority_;
					wcscpy_s(sInfo.szPicUrl, gnet::oct2wstr(pp->userlist_[i].pic_).c_str());
					sInfo._nUserState = pp->userlist_[i].state_;
					*/

					sInfo._nClassRoomId = pp->cid_;
					wcscpy_s(sInfo.szSignature, gnet::oct2wstr(pp->userlist_[i].signature_).c_str());					
					sInfo.nGender = pp->userlist_[i].gender_;
					sInfo._nBanType = pp->userlist_[i].bantype_;
					//sInfo.nDevice = pp->userlist_[i].device_;
					wcscpy_s(sInfo.szMobile, gnet::oct2wstr(pp->userlist_[i].mobile_).c_str());
					wcscpy_s(sInfo.szEmail, gnet::oct2wstr(pp->userlist_[i].email_).c_str());					

					_userinfoContainer.UpdateUserINfo(sInfo);

					if(_callback)
					{
						_callback->OnUpdateUserInfo(sInfo.nUserId);
					}
				}
			}
			break;

		case gnet::NotifyVideoList::PROTOCOL_TYPE:
			{
				gnet::NotifyVideoList * pp = dynamic_cast<gnet::NotifyVideoList*>(p);
								
				if(_callback)
				{
					_callback->OnRecvStudentVideoList(pp->cid_,pp->topvideolist_);
				}
			}
			break;

		case gnet::RefreshVideoListResp::PROTOCOL_TYPE:
			{
				gnet::RefreshVideoListResp * pp = dynamic_cast<gnet::RefreshVideoListResp*>(p);

                CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
                if(NULL != pRoom)
                {
                    pRoom->TopVideoListOpt(0,2,0);  //清除原有的列表
                    for (size_t i = 0; i < pp->topvideolist_.size(); i++)
                    {
                        pRoom->TopVideoListOpt(pp->topvideolist_[i], 0, 0);
                    }
                }

				if(_callback)
				{
					_callback->OnRecvStudentVideoList(pp->cid_,pp->topvideolist_);
				}
			}

			break;

		case gnet::SetClassMode::PROTOCOL_TYPE:
			{
				gnet::SetClassMode * pp = dynamic_cast<gnet::SetClassMode*>(p);
				if(_callback)
				{
					_callback->OnSetClassMode(pp->cid_,pp->userid_,pp->classmode_);
				}

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
				if(NULL != pRoom)
				{
					SLClassRoomBaseInfo& sRoomInfo = pRoom->GetRoomBaseInfo();
					sRoomInfo._nClassMode = pp->classmode_;
				}
			}
			break;
		case gnet::QueryPreCWaresResp::PROTOCOL_TYPE:
			{
				gnet::QueryPreCWaresResp * pp = dynamic_cast<gnet::QueryPreCWaresResp*>(p);

				std::vector<SLCoursewareInfo> vecCourseware;

				for(int i=0 ;i<pp->courselist_.size();i++)
				{
					SLCoursewareInfo ci;
					wcscpy_s(ci._wszName,gnet::oct2wstr(pp->courselist_[i].name_).c_str());
					wcscpy_s(ci._wszFile,gnet::oct2wstr(pp->courselist_[i].url_).c_str());
					vecCourseware.push_back(ci);
				}
				
				if(_callback)
				{
					_callback->OnRecvPrevCoursewareList(pp->cid_,vecCourseware);
				}
			}
			break;

		case gnet::ReturnToken::PROTOCOL_TYPE:
			{
				gnet::ReturnToken * pp = dynamic_cast<gnet::ReturnToken*>(p);

				if(_callback)
				{
					ATL::CStringA	strToken((LPCWSTR)gnet::oct2wstr(pp->token_).c_str());
					_callback->OnRecvUploadToken(pp->retcode_,pp->userid_,pp->validatetime_,strToken.GetString());
				}
			}
			break;

		case gnet::SetTeacherVedio::PROTOCOL_TYPE:
			{
				gnet::SetTeacherVedio * pp = dynamic_cast<gnet::SetTeacherVedio*>(p);

				if(_callback)
				{
					_callback->OnTeacherSwitchVideo(pp->cid_,pp->userid_,pp->teachervedio_);
				}
			}

			break;
		case gnet::MesgResp::PROTOCOL_TYPE:
			{
				gnet::MesgResp* pp = dynamic_cast<gnet::MesgResp*>(p);
				std::vector<SLMessageInfo>	msgList;
				
				for(int i = 0;i < pp->message_.size();i++)
				{
					SLMessageInfo msg;
					wcscpy_s(msg._wszTitle,oct2wstr(pp->message_[i].title_).c_str());
					wcscpy_s(msg._wszContent,oct2wstr(pp->message_[i].content_).c_str());
					wcscpy_s(msg._wszUrl,oct2wstr(pp->message_[i].url_).c_str());					
					msgList.push_back(msg);
				}

				if(_callback)
				{
					_callback->OnRecvMsgList(pp->userid_,pp->msgtype_,msgList);
				}
			}

			break;

		case gnet::PromoteAsAssistant::PROTOCOL_TYPE:
			{
				gnet::PromoteAsAssistant* pp = dynamic_cast<gnet::PromoteAsAssistant*>(p);
				SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp->userid_);
				if(sInfo._nClassRoomId != pp->cid_)
				{
					break;
				}
				
				if(pp->ret_ == gnet::PromoteAsAssistant::SUCCESS)
				{
					sInfo.nUserAuthority = biz::UserAu_Assistant;
					_userinfoContainer.UpdateUserINfo(sInfo);
				}

				if(_callback)
				{
					_callback->OnRecvSetAssistantMsg(pp->cid_,pp->userid_,true,pp->ret_);
				}
			}
			break;
		case gnet::CancelAssistant::PROTOCOL_TYPE:
			{
				gnet::CancelAssistant* pp = dynamic_cast<gnet::CancelAssistant*>(p);
				SLUserInfo sInfo = _userinfoContainer.GetUserInfoById(pp->userid_);
				if(sInfo._nClassRoomId != pp->cid_)
				{
					break;
				}

				sInfo.nUserAuthority = biz::UserAu_Student;
				_userinfoContainer.UpdateUserINfo(sInfo);

				if(_callback)
				{
					_callback->OnRecvSetAssistantMsg(pp->cid_,pp->userid_,false,0);
				}
			}
			break;
		case gnet::UpdateCodeResp::PROTOCOL_TYPE:
			{
				gnet::UpdateCodeResp* pp = dynamic_cast<gnet::UpdateCodeResp*>(p);
				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->cid_);
				if(pRoom == NULL)
				{
					break;
				}

				SLClassRoomBaseInfo& sRoomInfo = pRoom->GetRoomBaseInfo();
				ATL::CStringA  strCode(gnet::oct2str(pp->code_).c_str());
				wcscpy_s(sRoomInfo._szDevAccessCode,ATL::CStringW(strCode));

				if(_callback)
				{
					_callback->OnRecvUpdateDevCodeMsg(pp->cid_);
				}
			}

			break;

		case gnet::MobileConnectClassResp::PROTOCOL_TYPE:
			{
  				gnet::MobileConnectClassResp * pp = dynamic_cast<gnet::MobileConnectClassResp*>(p);
				if(pp->ret_ != gnet::MobileConnectClassResp::OK)
				{
					break;
				}

				if(_callback)
				{
					_callback->OnRecvNetDevAccess(pp->cid_,pp->userid_,gnet::oct2wstr(pp->devicename_).c_str());
				}
			}

			break;

		case gnet::ChooseMobileResp::PROTOCOL_TYPE:
			{
				gnet::ChooseMobileResp * pp = dynamic_cast<gnet::ChooseMobileResp*>(p);
				if(_callback)
				{
					_callback->OnRecvChooseMobileResponse(pp->tid_,pp->userid_,pp->ret_,pp->actiontype_);
				}
			}

			break;

		case gnet::Kick::PROTOCOL_TYPE:
			{
				gnet::Kick * pp = dynamic_cast<gnet::Kick*>(p);
				if(pp->notifytype_ != gnet::Kick::MOBILE_OFF)
				{
					break;
				}

				if(_callback)
				{
					_callback->OnRecvMobileNotify(pp->tid_,pp->userid_);
				}
			}

			break;

		case gnet::AutoLoginTokenRsp::PROTOCOL_TYPE:
			{
				gnet::AutoLoginTokenRsp* pp = (gnet::AutoLoginTokenRsp*)p;
				WCHAR wszToken[512] = { 0 };
				wcscpy_s(wszToken,gnet::oct2wstr(pp->token_).c_str());

				if(_callback)
				{
					_callback->OnRecvAutoLoginToken(pp->userid_,pp->retcode_,wszToken);
				}
			}
			break;

		case gnet::MesgReminder::PROTOCOL_TYPE:
			{
				gnet::MesgReminder* pp = (gnet::MesgReminder*)p;

				CLClassRoomInfo* pRoom = (CLClassRoomInfo*)_classroomContainer.GetClassRoomInterface(pp->classinfo_.classid_);
				if(pRoom != NULL)
				{
					if(pp->msgtype_ == gnet::MesgReminder::CLASS_ON)
					{
						pRoom->SetClassState(biz::Eclassroomstate_Going);

					}
					else if(pp->msgtype_ == gnet::MesgReminder::SIGN)
					{
						pRoom->SetClassState(biz::Eclassroomstate_Ready);
					}
				}
				
				if(_callback)
				{
					_callback->OnRecvClassMsgReminder(pp->classinfo_.courseid_,pp->classinfo_.classid_,pp->msgtype_);
				}
			}
			break;
		case gnet::Error::PROTOCOL_TYPE:
			{
				gnet::Error* pp = (gnet::Error*)p;	
				if(_callback)
				{
					_callback->OnError(pp->cid_, pp->errtype_);
				}
			}
			break;

        case gnet::SetMainShow::PROTOCOL_TYPE:
            {
                gnet::SetMainShow* pp = (gnet::SetMainShow*)p;
                if(_callback)
                {
                    _callback->MainShowChange(pp->showtype_,pp->classid_,pp->teacher_);
                }
            }
            break;
		}
	}


}