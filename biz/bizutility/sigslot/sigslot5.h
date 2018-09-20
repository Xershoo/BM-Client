#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	class Slot5 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3, TArg4, TArg5) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	class SlotObj5 : public Slot5<TArg1, TArg2, TArg3, TArg4, TArg5>
	{
	public:
		SlotObj5(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4, TArg5 arg5)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3, arg4, arg5);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj5<TObj, TArg1, TArg2, TArg3, TArg4, TArg5>* rr = dynamic_cast<const SlotObj5<TObj, TArg1, TArg2, TArg3, TArg4, TArg5>*>(&r);
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
			return new SlotObj5(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5);
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	class SlotFunc5 : public Slot5<TArg1, TArg2, TArg3, TArg4, TArg5>
	{
	public:
		SlotFunc5(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4, TArg5 arg5)
		{
			(*_pfunc)(arg1, arg2, arg3, arg4, arg5);
		}

		virtual bool operator == (const Slot5<TArg1, TArg2, TArg3, TArg4, TArg5>& r) const
		{
			const SlotFunc5<TArg1, TArg2, TArg3, TArg4, TArg5>* rr = dynamic_cast<const SlotFunc5<TArg1, TArg2, TArg3, TArg4, TArg5>*>(&r);
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
			return new SlotFunc5(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5> class Signal5;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	class SlotSignal5 : public Slot5<TArg1, TArg2, TArg3, TArg4, TArg5>, public SlotSignal
	{
	public:
		SlotSignal5(Signal5<TArg1, TArg2, TArg3, TArg4, TArg5>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5)
		{
			(*_signal)(arg1, arg2, arg3, arg4, arg5);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal5<TArg1, TArg2, TArg3, TArg4, TArg5>* rr = dynamic_cast<const SlotSignal5<TArg1, TArg2, TArg3, TArg4, TArg5>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal5(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal5<TArg1, TArg2, TArg3, TArg4, TArg5>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	class Signal5 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot5<TArg1, TArg2, TArg3, TArg4, TArg5>*)*it))(arg1, arg2, arg3, arg4, arg5);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5))
		{
			BaseSignal::Connect(new SlotObj5<TObj, TArg1, TArg2, TArg3, TArg4, TArg5>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5))
		{
			SlotObj5<TObj, TArg1, TArg2, TArg3, TArg4, TArg5> slot = SlotObj5<TObj, TArg1, TArg2, TArg3, TArg4, TArg5>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg4, TArg5))
		{
			BaseSignal::Connect(new SlotFunc5<TArg1, TArg2, TArg3, TArg4, TArg5>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5))
		{
			SlotFunc5<TArg1, TArg2, TArg3, TArg4, TArg5> slot = SlotFunc5<TArg1, TArg2, TArg3, TArg4, TArg5>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal5<TArg1, TArg2, TArg3, TArg4, TArg5>& signal)
		{
			BaseSignal::Connect(new SlotSignal5<TArg1, TArg2, TArg3, TArg4, TArg5>(&signal, this));
		}

		void DisConnect(Signal5<TArg1, TArg2, TArg3, TArg4, TArg5>& signal)
		{
			SlotSignal5<TArg1, TArg2, TArg3, TArg4, TArg5> slot = SlotSignal5<TArg1, TArg2, TArg3, TArg4, TArg5>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}