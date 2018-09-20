#pragma once

// ResetSlot 是用来拷贝Widget的时候,把对应的信号槽也拷贝过来.
#include <map>


namespace biz_utility
{
	class Slot0
	{
	public:
		virtual ~Slot0() {}
		virtual void operator ()() = 0;
		virtual bool operator == (const Slot0&) const = 0;
		virtual SrcSigContainer* GetSlotContainer() const = 0;
		virtual void ResetSlot(std::map<void*,void*>& mapClone) = 0;	// ResetSlot.1
		virtual Slot0* Clone() = 0;										// ResetSlot.6
	};


	template<typename TObj>
	class SlotObj0 : public Slot0
	{
	public:
		SlotObj0(TObj* obj, void (TObj::*pfunc)())
			: _obj(obj), _pfunc(pfunc){}

		virtual void operator ()()
		{
			(_obj->*_pfunc)();
		}

		virtual bool operator == (const Slot0& r) const
		{
			if (NULL == r.GetSlotContainer())
				return false;

			const SlotObj0<TObj>* rr = dynamic_cast<const SlotObj0<TObj>*>(&r);
			if (NULL == rr)
				return false;
			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

		virtual SrcSigContainer* GetSlotContainer() const
		{
			return &_obj->_src_sig_container;
		}

		virtual void ResetSlot(std::map<void*,void*>& mapClone)	// ResetSlot.2
		{
			assert_(mapClone.find(_obj) != mapClone.end());
			_obj = (TObj*)mapClone[_obj];
		}

		virtual Slot0* Clone()	// ResetSlot.7
		{
			return new SlotObj0(*this);
		}
		


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)();
	};


	class SlotFunc0 : public Slot0
	{
	public:
		SlotFunc0(void (*pfunc)()) : _pfunc(pfunc){}
		virtual void operator ()()
		{
			(*_pfunc)();
		}

		virtual bool operator == (const Slot0& r) const
		{
			if (r.GetSlotContainer() != NULL)
				return false;

			const SlotFunc0* rr = dynamic_cast<const SlotFunc0*>(&r);
			return ( _pfunc == rr->_pfunc);
		}

		virtual SrcSigContainer* GetSlotContainer() const
		{
			return NULL;
		}

		virtual void ResetSlot(std::map<void*,void*>& mapClone){}	// ResetSlot.3

		virtual Slot0* Clone()	// ResetSlot.7
		{
			return new SlotFunc0(*this);
		}
		

	private:
		void (*_pfunc)();
	};


	class Signal0 : public BaseSignal
	{
	public:
		Signal0() : _currentRunSlot(NULL){}

		~Signal0()
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (; it != _vSlots.end(); ++it)
			{
				SrcSigContainer* container = (*it)->GetSlotContainer();
				if (container != NULL)
					container->RemoveSignal(this);
				delete *it;
			}
		}

		virtual void RemoveSlotByContainer(SrcSigContainer* slotContainer)
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			while (it != _vSlots.end())
			{
				if ((*it)->GetSlotContainer() == slotContainer)
				{
					if (_currentRunSlot >= *it)
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

		void operator ()()
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				if (_destSlotContainer != NULL
					&& (*it)->GetSlotContainer() != _destSlotContainer)
					continue;
				_currentRunSlot = *it;
				(**it)();
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)())
		{
			Connect(new SlotObj0<TObj>(obj, pfunc));
			obj->_src_sig_container.AddSignal(this);
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)())
		{
			SlotObj0<TObj> slot = SlotObj0<TObj>(obj, pfunc);
			DisConnect(&slot);
		}

		void Connect(void (*pfunc)())
		{
			Connect(new SlotFunc0(pfunc));
		}

		void DisConnect(void (*pfunc)())
		{
			SlotFunc0 slot = SlotFunc0( pfunc);
			DisConnect(&slot);
		}

		void ResetSlot(std::map<void*,void*>& mapClone)		// ResetSlot.4
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
				(*it)->ResetSlot(mapClone);
		}

		Signal0& operator = (const Signal0& rhs)		// ResetSlot.5
		{
			std::vector<Slot0*>::const_iterator it = rhs._vSlots.begin();
			for (; it!=rhs._vSlots.end(); ++it)
				_vSlots.push_back((*it)->Clone());
			return *this;
		}

		Signal0(const Signal0& rhs)		// ResetSlot.8
		{
			std::vector<Slot0*>::const_iterator it = rhs._vSlots.begin();
			for (; it!=rhs._vSlots.end(); ++it)
				_vSlots.push_back((*it)->Clone());
		}
		

	private:
		void Connect(Slot0* slot)
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
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

		void DisConnect(Slot0* slot)
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
				{
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

			if (NULL == slot->GetSlotContainer())
				return;

			it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if (slot->GetSlotContainer() == (*it)->GetSlotContainer())
					break;
			}

			if (it == _vSlots.end())
				slot->GetSlotContainer()->RemoveSignal(this);
		}

		void CleanDelayDeletes()
		{
			for (int i=0; i<(int)_delayRemoves.size(); i++)
			{
				std::vector<Slot0*>::iterator it =
					std::find(_vSlots.begin(), _vSlots.end(), _delayRemoves[i]);

				if (it != _vSlots.end())
				{
					delete *it;
					_vSlots.erase(it);
				}
			}
			_delayRemoves.clear();
		}


	private:
		std::vector<Slot0*>	_vSlots;
		std::vector<Slot0*>	_delayRemoves;
		Slot0*				_currentRunSlot;
	};
}