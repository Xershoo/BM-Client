#pragma once


namespace biz_utility
{
	template<typename TArg1>
	class Slot1 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1) = 0;
	};


	// slot的成员函数包装
	template<typename TObj, typename TArg1>
	class SlotObj1 : public Slot1<TArg1>
	{
	public:
		SlotObj1(TObj* obj, void (TObj::*pfunc)(TArg1), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1)
		{
			(_obj->*_pfunc)(arg1);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj1<TObj, TArg1>* rr = dynamic_cast<const SlotObj1<TObj, TArg1>*>(&r);
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
			return new SlotObj1(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1);
	};


	// slot 的全局函数包装
	template<typename TArg1>
	class SlotFunc1 : public Slot1<TArg1>
	{
	public:
		SlotFunc1(void (*pfunc)(TArg1)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1)
		{
			(*_pfunc)(arg1);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotFunc1<TArg1>* rr = dynamic_cast<const SlotFunc1<TArg1>*>(&r);
			if (NULL == rr)
				return false;

			return ( _pfunc == rr->_pfunc);
		}

		virtual BaseSlot* Clone4(BaseSignal*) const
		{
			return new SlotFunc1(_obj, _pfunc);
		}


	private:
		void (*_pfunc)(TArg1);
	};


	// slot 的信号包装
	template<typename TArg1> class Signal1;
	template<typename TArg1>
	class SlotSignal1 : public Slot1<TArg1>, public SlotSignal
	{
	public:
		SlotSignal1(Signal1<TArg1>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1)
		{
			(*_signal)(arg1);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal1<TArg1>* rr = dynamic_cast<const SlotSignal1<TArg1>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal1(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal1<TArg1>*	_signal;		// 被包装的信号
	};


	template<typename TArg1>
	class Signal1 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot1<TArg1>*)*it))(arg1);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1))
		{
			BaseSignal::Connect(new SlotObj1<TObj, TArg1>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1))
		{
			SlotObj1<TObj, TArg1> slot = SlotObj1<TObj, TArg1>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1))
		{
			BaseSignal::Connect(new SlotFunc1<TArg1>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1))
		{
			SlotFunc1<TArg1> slot = SlotFunc1<TArg1>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal1<TArg1>& signal)
		{
			BaseSignal::Connect(new SlotSignal1<TArg1>(&signal, this));
		}

		void DisConnect(Signal1<TArg1>& signal)
		{
			SlotSignal1<TArg1> slot = SlotSignal1<TArg1>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}