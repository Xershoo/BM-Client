#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2>
	class Slot2 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2>
	class SlotObj2 : public Slot2<TArg1, TArg2>
	{
	public:
		SlotObj2(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2)
		{
			(_obj->*_pfunc)(arg1, arg2);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj2<TObj, TArg1, TArg2>* rr = dynamic_cast<const SlotObj2<TObj, TArg1, TArg2>*>(&r);
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
			return new SlotObj2(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2);
	};


	template<typename TArg1, typename TArg2>
	class SlotFunc2 : public Slot2<TArg1, TArg2>
	{
	public:
		SlotFunc2(void (*pfunc)(TArg1, TArg2)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2)
		{
			(*_pfunc)(arg1, arg2);
		}

		virtual bool operator == (const Slot2<TArg1, TArg2>& r) const
		{
			const SlotFunc2<TArg1, TArg2>* rr = dynamic_cast<const SlotFunc2<TArg1, TArg2>*>(&r);
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
			return new SlotFunc2(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2> class Signal2;
	template<typename TArg1, typename TArg2>
	class SlotSignal2 : public Slot2<TArg1, TArg2>, public SlotSignal
	{
	public:
		SlotSignal2(Signal2<TArg1, TArg2>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2)
		{
			(*_signal)(arg1, arg2);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal2<TArg1,TArg2>* rr = dynamic_cast<const SlotSignal2<TArg1,TArg2>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal2(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal2<TArg1,TArg2>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2>
	class Signal2 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot2<TArg1,TArg2>*)*it))(arg1,arg2);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2))
		{
			BaseSignal::Connect(new SlotObj2<TObj, TArg1, TArg2>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2))
		{
			SlotObj2<TObj, TArg1, TArg2> slot = SlotObj2<TObj, TArg1, TArg2>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2))
		{
			BaseSignal::Connect(new SlotFunc2<TArg1, TArg2>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2))
		{
			SlotFunc2<TArg1, TArg2> slot = SlotFunc2<TArg1, TArg2>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal2<TArg1,TArg2>& signal)
		{
			BaseSignal::Connect(new SlotSignal2<TArg1,TArg2>(&signal, this));
		}

		void DisConnect(Signal2<TArg1,TArg2>& signal)
		{
			SlotSignal2<TArg1,TArg2> slot = SlotSignal2<TArg1,TArg2>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}