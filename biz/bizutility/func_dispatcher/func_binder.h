#pragma once


template <typename TRet, typename TC>
class mem_fun_binder_0
{
public:
	mem_fun_binder_0(TRet (TC::*pfunc)(), TC* pc) : m_pc(pc), m_pMemberFun(pfunc){}
	TRet operator()() const { return (m_pc->*m_pMemberFun)(); }

	TC* m_pc;
	TRet (TC::*m_pMemberFun)();
};


template <typename TRet, typename TC, typename TArg>
class mem_fun_binder_1
{
public:
	mem_fun_binder_1(TRet (TC::*pfunc)(TArg), TC* pc, TArg arg)
		: m_arg(arg),
		m_pc(pc),
		m_pMemberFun(pfunc){}
	TRet operator()() const { return (m_pc->*m_pMemberFun)(m_arg); }

	TC* m_pc;
	TRet (TC::*m_pMemberFun)(TArg);
	TArg m_arg;
};


template <typename TRet, typename TC, typename TArg1, typename TArg2>
class mem_fun_binder_2
{
public:
	mem_fun_binder_2(TRet (TC::*pfunc)(TArg1, TArg2), TC* pc, TArg1 arg1, TArg2 arg2)
		: m_pMemberFun(pfunc),
		m_pc(pc),
		m_arg1(arg1),
		m_arg2(arg2){}
	TRet operator()() const { return (m_pc->*m_pMemberFun)(m_arg1, m_arg2); }

	TC* m_pc;
	TRet (TC::*m_pMemberFun)(TArg1, TArg2);
	TArg1 m_arg1;
	TArg2 m_arg2;
};


template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3>
class mem_fun_binder_3
{
public:
	mem_fun_binder_3(TRet (TC::*pfunc)(TArg1, TArg2, TArg3), TC* pc, TArg1 arg1, TArg2 arg2, TArg3 arg3)
		: m_pMemberFun(pfunc),
		m_pc(pc),
		m_arg1(arg1),
		m_arg2(arg2),
		m_arg3(arg3){}
	TRet operator()() const { return (m_pc->*m_pMemberFun)(m_arg1, m_arg2, m_arg3); }

	TC* m_pc;
	TRet (TC::*m_pMemberFun)(TArg1, TArg2, TArg3);
	TArg1 m_arg1;
	TArg2 m_arg2;
	TArg3 m_arg3;
};


template <typename TRet, typename TC, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
class mem_fun_binder_4
{
public:
	mem_fun_binder_4(TRet (TC::*pfunc)(TArg1, TArg2, TArg3, TArg4), TC* pc, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		: m_pMemberFun(pfunc),
		m_pc(pc),
		m_arg1(arg1),
		m_arg2(arg2),
		m_arg3(arg3),
		m_arg4(arg4){}
	TRet operator()() const { return (m_pc->*m_pMemberFun)(m_arg1, m_arg2, m_arg3, m_arg4); }

	TC* m_pc;
	TRet (TC::*m_pMemberFun)(TArg1, TArg2, TArg3, TArg4);
	TArg1 m_arg1;
	TArg2 m_arg2;
	TArg3 m_arg3;
	TArg4 m_arg4;
};