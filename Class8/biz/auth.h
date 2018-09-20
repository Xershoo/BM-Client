#pragma once

#include "gnet/socket/net.h"
#include "gnet/gnetconvert.h"
#include "gnet/socket/security.h"
#include <string>
#include "interface/data.h"


namespace biz
{

struct AUCallback
{
	virtual void OnLogon(gnet::Aio::SID, AUERRNO, __int64 uid, char utype) = 0;
	virtual void OnKickOut(gnet::Aio::SID, __int64 uid) = 0;
	virtual void DoConnect() = 0;
	virtual void DoSend(gnet::Aio::SID sid, const gnet::Protocol&) = 0;
	virtual void DoSetISecurity(gnet::Aio::SID sid, gnet::Security::Type, gnet::Octets&) = 0;
	virtual void DoSetOSecurity(gnet::Aio::SID sid, gnet::Security::Type, gnet::Octets&) = 0;
};


class AuthClient
{
public:
	AuthClient() : _callback(NULL), _usertype(0) {}

	void SetCallback(AUCallback* cb){ _callback = cb; }

	void Logon(const char* username, const char* pwd, char usertype)
	{
		_username = username;
		_pwd = pwd;
		_usertype = usertype;

		if (_callback != NULL)
			_callback->DoConnect();
	}

	void OnRecv(gnet::Aio::SID sid, gnet::Protocol& p)
	{
		switch (p.GetType())
		{
		case gnet::Challenge::PROTOCOL_TYPE:
			On_Challenge(sid, dynamic_cast<gnet::Challenge&>(p));
			break;
		case gnet::ChallengeResult::PROTOCOL_TYPE:
			On_ChallengeResult(sid, dynamic_cast<gnet::ChallengeResult&>(p));
			break;
		case gnet::KeyExchange::PROTOCOL_TYPE:
			On_KeyExchange(sid, dynamic_cast<gnet::KeyExchange&>(p));
			break;
		case gnet::KickOut::PROTOCOL_TYPE:
			On_KickOut(sid, dynamic_cast<gnet::KickOut&>(p));
			break;
		}
	}


private:
	void On_Challenge(gnet::Aio::SID sid, gnet::Challenge& p)
	{
		gnet::Octets oct(_pwd.c_str(), _pwd.size());
		gnet::MD5Hash md5hash;
		oct = md5hash.Digest(oct);
		md5hash.Update(oct);
		md5hash.Update(p.nonce_);

		gnet::ChallengeResp cresponse;
		cresponse.usertype_ = _usertype;
		cresponse.username_ = gnet::host2net(_username);
		md5hash.Final(cresponse.resp_);
		if (_callback != NULL)
			_callback->DoSend(sid, cresponse);
	}


	void On_ChallengeResult(gnet::Aio::SID sid, gnet::ChallengeResult& p)
	{
		AUERRNO auerrno = AU_UNKNOW;
		if (_callback != NULL)
		{
			switch (p.retcode_)
			{
			case gnet::ChallengeResult::OK:
				auerrno = AU_OK;
				break;
			case gnet::ChallengeResult::SERVER_ERROR:
				auerrno = AU_SERVERERROR;
				break;
			case gnet::ChallengeResult::INVALID_USERNAME:
				auerrno = AU_INVALID_USERNAME;
				break;
			case gnet::ChallengeResult::INVALID_PASSWORD:
				auerrno = AU_INVALID_PASSWORD;
				break;
			}
		}
		if (_callback != NULL)
			_callback->OnLogon(sid, auerrno, p.userid_, p.usertype_);

		//_username.clear();
		//_pwd.clear();
		//_usertype = 0;
	}


	void On_KeyExchange(gnet::Aio::SID sid, gnet::KeyExchange& p)
	{
		gnet::Octets oct(_pwd.c_str(), _pwd.size());
		gnet::MD5Hash md5hash;
		oct = md5hash.Digest(oct);
		md5hash.Update(oct);
		md5hash.Update(p.nonce_);
		md5hash.Final(oct);
		if (_callback != NULL)
			_callback->DoSetOSecurity(sid, gnet::ARCFOURSECURITY, oct);

		gnet::Random randgen;
		gnet::Octets nonce;
		nonce.resize(16);
		randgen.Update(nonce);
		oct.replace(_pwd.c_str(), _pwd.size());
		md5hash.Reset();
		oct = md5hash.Digest(oct);
		md5hash.Update(oct);
		md5hash.Update(nonce);
		md5hash.Final(oct);
		if (_callback != NULL)
			_callback->DoSetISecurity(sid, gnet::ARCFOURSECURITY, oct);

		gnet::KeyExchange keyexchange;
		keyexchange.key_ = oct;
		if (_callback != NULL)
			_callback->DoSend(sid, keyexchange);
	}


	void On_KickOut(gnet::Aio::SID sid, gnet::KickOut& p)
	{
		if (_callback != NULL)
			_callback->OnKickOut(sid, p.userid_);
	}


private:
	AUCallback*		_callback;
	std::string		_username;
	std::string		_pwd;
	char			_usertype;
};

}