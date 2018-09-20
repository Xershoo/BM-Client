#include "sigslot.h"

namespace biz_utility
{
	BaseSignal::BaseSignal() : _currentRunSlot(NULL){}


	BaseSignal::~BaseSignal()
	{
		Clear();
		if (!_signal2SlotWrappers.empty())
		{
			std::vector<SlotSignal*> temp = _signal2SlotWrappers;
			std::vector<SlotSignal*>::iterator it = temp.begin();
			for (; it!= temp.end(); ++it)
				(*it)->OnInnerSignalDestory();
		}
	}


	void BaseSignal::Clear()
	{
		std::vector<BaseSlot*>::iterator it = _vSlots.begin();
		for (; it != _vSlots.end(); ++it)
		{
			SrcSigContainer* container = (*it)->GetSlotContainer();
			if (container != NULL)
				container->RemoveSignal(this);
			delete *it;
		}
		_vSlots.clear();
	}



	void BaseSignal::Connect(BaseSlot* slot)
	{
		std::vector<BaseSlot*>::iterator it = _vSlots.begin();
		for (; it!=_vSlots.end(); ++it)
		{
			if ((**it) == *slot)
			{
				delete slot;
				return;
			}
		}
		_vSlots.push_back(slot);
	}


	void BaseSignal::DisConnect(BaseSlot* slot)
	{
		SrcSigContainer* srcContainer = slot->GetSlotContainer();

		std::vector<BaseSlot*>::iterator it = _vSlots.begin();
		for (; it!=_vSlots.end(); ++it)
		{
			if ((**it) == *slot)
			{	// 6！如果在调用的时候删除槽.不能删除之前的，可以删除之后的.
				if (_currentRunSlot >= *it)	
					_delayRemoves.push_back(*it);
				else
				{
					delete *it;
					_vSlots.erase(it);
				}
				break;
			}
		}

		if (NULL == srcContainer)
			return;

		// 3、当解除信号关联的时候，该信号和槽子没有任何关联的时候要，删除槽子的信号源。
		it = _vSlots.begin();
		for (; it!=_vSlots.end(); ++it)
		{
			if (srcContainer == (*it)->GetSlotContainer())
				break;
		}

		if (it == _vSlots.end())
			srcContainer->RemoveSignal(this);
	}


	BaseSignal::BaseSignal(const BaseSignal& rhs)
	{
		Clear();
		std::vector<BaseSlot*>::const_iterator it = rhs._vSlots.begin();
		for (; it != rhs._vSlots.end(); ++it)
			Connect((*it)->Clone4(this));
	}


	BaseSignal& BaseSignal::operator = (const BaseSignal& rhs)
	{
		Clear();
		std::vector<BaseSlot*>::const_iterator it = rhs._vSlots.begin();
		for (; it != rhs._vSlots.end(); ++it)
			Connect((*it)->Clone4(this));
		return *this;
	}


	void BaseSignal::CleanDelayDeletes()
	{
		for (int i=0; i<(int)_delayRemoves.size(); i++)
		{
			std::vector<BaseSlot*>::iterator it =
				std::find(_vSlots.begin(), _vSlots.end(), _delayRemoves[i]);

			if (it != _vSlots.end())
			{
				delete *it;
				_vSlots.erase(it);
			}
		}
		_delayRemoves.clear();
	}


	void BaseSignal::RemoveSlotByContainer(SrcSigContainer* slotContainer)
	{
		std::vector<BaseSlot*>::iterator it = _vSlots.begin();
		while (it != _vSlots.end())
		{
			if ((*it)->GetSlotContainer() == slotContainer)
			{
				if (_currentRunSlot >= *it)	// 6! 不能删除之前的，可以删除之后的
				{
					_delayRemoves.push_back(*it);
					++it;
				}
				else
				{
					delete *it;
					it = _vSlots.erase(it);
				}
			}
			else
				++it;
		}
	}


	// 因为不是模板
	SlotSignal0::SlotSignal0(Signal0* signal, BaseSignal* srcsignal)
		:SlotSignal(signal, srcsignal),
		_signal(signal){}


	void SlotSignal0::operator ()()
	{
		(*_signal)();
	}
}