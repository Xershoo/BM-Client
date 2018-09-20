#pragma once

#include <vector>
#include <algorithm>	// find


// 要当心的问题
// 1. 不支持多线程，多线程会引发很多问题;
// 2. 如何满足信号和槽的随机析构问题，任何一方析构之后维护对应的数据来表示连接状态,
//	主要是信号对象(Signal1)里的信号槽队列Signal1::_vSlots,
//	和信号槽容器里的信号源队列SrcSigContainer::_vSig1s
// 3. 当解除信号关联的时候不光要删除对应的Signal1::_vSlots
//	还要检查当是最后一个关联的这种槽子，要删除槽子对应的信号队列;
// 4. 信号连接信号, SlotSignal1与Signal1要相互引用.
// !!!调用槽的时候可以删除槽子,但是千万不能删除信号源，切记切记！
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
		std::vector<SlotSignal*>	_signal2SlotWrappers; // 将signal包装为slot
		BaseSlot*					_currentRunSlot;
	};


	class BaseSlot
	{
	public:
		virtual ~BaseSlot() {}
		virtual bool operator == (const BaseSlot&) const = 0;
		virtual SrcSigContainer* GetSlotContainer() const { return NULL; }
		virtual BaseSlot* Clone4(BaseSignal* owner) const = 0;
		// slot对象都是属于signal的临时对象
	};


	// 使用槽的对象必须声明这个对象通过 SLOT_OBJ
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