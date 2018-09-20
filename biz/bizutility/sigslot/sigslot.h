#pragma once

#include <vector>
#include <algorithm>	// find


// Ҫ���ĵ�����
// 1. ��֧�ֶ��̣߳����̻߳������ܶ�����;
// 2. ��������źźͲ۵�����������⣬�κ�һ������֮��ά����Ӧ����������ʾ����״̬,
//	��Ҫ���źŶ���(Signal1)����źŲ۶���Signal1::_vSlots,
//	���źŲ���������ź�Դ����SrcSigContainer::_vSig1s
// 3. ������źŹ�����ʱ�򲻹�Ҫɾ����Ӧ��Signal1::_vSlots
//	��Ҫ��鵱�����һ�����������ֲ��ӣ�Ҫɾ�����Ӷ�Ӧ���źŶ���;
// 4. �ź������ź�, SlotSignal1��Signal1Ҫ�໥����.
// !!!���ò۵�ʱ�����ɾ������,����ǧ����ɾ���ź�Դ���м��мǣ�
//boost::signal< void ( const std::string& ) >  boostSignal;


namespace biz_utility
{
#define SLOT_OBJ biz_utility::SrcSigContainer _src_sig_container;


	class BaseSlot;
	class SlotSignal;
	class SrcSigContainer;


	class BaseSignal
	{
		friend SlotSignal;
	public:
		~BaseSignal();
		BaseSignal();
		void Clear();
		void Connect(BaseSlot*);
		void DisConnect(BaseSlot*);
		BaseSignal(const BaseSignal& rhs);
		BaseSignal& operator = (const BaseSignal& rhs);
		void CleanDelayDeletes();
		void RemoveSlotByContainer(SrcSigContainer* slotContainer);

	protected:
		std::vector<BaseSlot*>		_vSlots;
		std::vector<BaseSlot*>		_delayRemoves;
		std::vector<SlotSignal*>	_signal2SlotWrappers; // ��signal��װΪslot
		BaseSlot*					_currentRunSlot;
	};


	class BaseSlot
	{
	public:
		virtual ~BaseSlot() {}
		virtual bool operator == (const BaseSlot&) const = 0;
		virtual SrcSigContainer* GetSlotContainer() const { return NULL; }
		virtual BaseSlot* Clone4(BaseSignal* owner) const = 0;
		// slot����������signal����ʱ����
	};


	// ʹ�ò۵Ķ�����������������ͨ�� SLOT_OBJ
	class SrcSigContainer
	{
	public:
		~SrcSigContainer()
		{
			std::vector<BaseSignal*>::iterator it = _vSigs.begin();
			for (; it!=_vSigs.end(); ++it)
				(*it)->RemoveSlotByContainer(this);
		}


		void AddSignal(BaseSignal* sig)
		{
			if (std::find(_vSigs.begin(), _vSigs.end(), sig) == _vSigs.end())
				_vSigs.push_back(sig);
		}


		void RemoveSignal(BaseSignal* sig)
		{
			std::vector<BaseSignal*>::iterator it =
				std::find(_vSigs.begin(), _vSigs.end(), sig);
			if (it != _vSigs.end())
				_vSigs.erase(it);
		}


	private:
		std::vector<BaseSignal*> _vSigs;
	};


	class SlotSignal
	{
	public:
		SlotSignal(BaseSignal* signal, BaseSignal* srcsignal)
			:_innerSignal(signal),
			_srcsignal(srcsignal)
		{
			_innerSignal->_signal2SlotWrappers.push_back(this);
		}

		~SlotSignal()
		{
			std::vector<SlotSignal*>::iterator it =
				std::find(_innerSignal->_signal2SlotWrappers.begin(),
				_innerSignal->_signal2SlotWrappers.end(), this);
			_innerSignal->_signal2SlotWrappers.erase(it);
		}

		virtual void OnInnerSignalDestory() = 0;


	protected:
		BaseSignal*	_innerSignal;
		BaseSignal* _srcsignal;
	};
}


#include "sigslot0.h"
#include "sigslot1.h"
#include "sigslot2.h"
#include "sigslot3.h"
#include "sigslot4.h"
#include "sigslot5.h"
#include "sigslot6.h"
#include "sigslot7.h"
#include "sigslot8.h"