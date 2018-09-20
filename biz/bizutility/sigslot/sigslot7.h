#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	class Slot7 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	class SlotObj7 : public Slot7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>
	{
	public:
		SlotObj7(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj7<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>* rr = dynamic_cast<const SlotObj7<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>*>(&r);
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
			return new SlotObj7(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7);
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	class SlotFunc7 : public Slot7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>
	{
	public:
		SlotFunc7(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
		{
			(*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		}

		virtual bool operator == (const Slot7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>& r) const
		{
			const SlotFunc7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>* rr = dynamic_cast<const SlotFunc7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>*>(&r);
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
			return new SlotFunc7(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7> class Signal7;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	class SlotSignal7 : public Slot7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>, public SlotSignal
	{
	public:
		SlotSignal7(Signal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
		{
			(*_signal)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>* rr = dynamic_cast<const SlotSignal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal7(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	class Signal7 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>*)*it))(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7))
		{
			BaseSignal::Connect(new SlotObj7<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7))
		{
			SlotObj7<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7> slot = SlotObj7<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg4, TArg5, TArg6, TArg7))
		{
			BaseSignal::Connect(new SlotFunc7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7))
		{
			SlotFunc7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7> slot = SlotFunc7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>& signal)
		{
			BaseSignal::Connect(new SlotSignal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>(&signal, this));
		}

		void DisConnect(Signal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>& signal)
		{
			SlotSignal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7> slot = SlotSignal7<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}