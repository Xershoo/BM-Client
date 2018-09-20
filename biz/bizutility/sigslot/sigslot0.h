#pragma once

#include <map>


namespace biz_utility
{
	class Slot0 : public BaseSlot
	{
	public:
		virtual void operator ()() = 0;
	};


	template<typename TObj>
	class SlotObj0 : public Slot0
	{
	public:
		SlotObj0(TObj* obj, void (TObj::*pfunc)(), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()()
		{
			(_obj->*_pfunc)();
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj0<TObj>* rr = dynamic_cast<const SlotObj0<TObj>*>(&r);
			if (NULL == rr)
				return false;

			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

		virtual SrcSigContainer* GetSlotContainer() const
		{
			return &_obj->_src_sig_container;
		}

		virtual BaseSlot* Clone4(BaseSignal* signal) const
		{
			return new SlotObj0(_obj, _pfunc, signal);
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

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotFunc0* rr = dynamic_cast<const SlotFunc0*>(&r);
			if (NULL == rr)
				return false;

			return ( _pfunc == rr->_pfunc);
		}

		virtual SrcSigContainer* GetSlotContainer() const
		{
			return NULL;
		}

		virtual BaseSlot* Clone4(BaseSignal*) const
		{
			return new SlotFunc0(_pfunc);
		}


	private:
		void (*_pfunc)();
	};


	// slot 的信号包装
	class Signal0;
	class SlotSignal0 : public Slot0, public SlotSignal
	{
	public:
		SlotSignal0(Signal0* signal, BaseSignal* srcsignal);

		virtual void operator ()();

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal0* rr = dynamic_cast<const SlotSignal0*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal0(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal0*	_signal;		// 被包装的信号
	};


	class Signal0 : public BaseSignal
	{
	public:
		void operator ()()
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot0*)*it))();
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)())
		{
			BaseSignal::Connect(new SlotObj0<TObj>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)())
		{
			SlotObj0<TObj> slot = SlotObj0<TObj>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)())
		{
			BaseSignal::Connect(new SlotFunc0(pfunc));
		}

		void DisConnect(void (*pfunc)())
		{
			SlotFunc0 slot = SlotFunc0(pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal0& signal)
		{
			BaseSignal::Connect(new SlotSignal0(&signal, this));
		}

		void DisConnect(Signal0& signal)
		{
			SlotSignal0 slot = SlotSignal0(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}