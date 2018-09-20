#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	class Slot6 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	class SlotObj6 : public Slot6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>
	{
	public:
		SlotObj6(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj6<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>* rr = dynamic_cast<const SlotObj6<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>*>(&r);
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
			return new SlotObj6(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6);
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	class SlotFunc6 : public Slot6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>
	{
	public:
		SlotFunc6(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
		{
			(*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6);
		}

		virtual bool operator == (const Slot6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>& r) const
		{
			const SlotFunc6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>* rr = dynamic_cast<const SlotFunc6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>*>(&r);
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
			return new SlotFunc6(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6> class Signal6;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	class SlotSignal6 : public Slot6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>, public SlotSignal
	{
	public:
		SlotSignal6(Signal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
		{
			(*_signal)(arg1, arg2, arg3, arg4, arg5, arg6);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>* rr = dynamic_cast<const SlotSignal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal6(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	class Signal6 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>*)*it))(arg1, arg2, arg3, arg4, arg5, arg6);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6))
		{
			BaseSignal::Connect(new SlotObj6<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6))
		{
			SlotObj6<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> slot = SlotObj6<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg4, TArg5, TArg6))
		{
			BaseSignal::Connect(new SlotFunc6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6))
		{
			SlotFunc6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> slot = SlotFunc6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>& signal)
		{
			BaseSignal::Connect(new SlotSignal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>(&signal, this));
		}

		void DisConnect(Signal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>& signal)
		{
			SlotSignal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> slot = SlotSignal6<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}