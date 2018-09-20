#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	class Slot4 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3, TArg4) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	class SlotObj4 : public Slot4<TArg1, TArg2, TArg3, TArg4>
	{
	public:
		SlotObj4(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3, arg4);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj4<TObj, TArg1, TArg2, TArg3, TArg4>* rr = dynamic_cast<const SlotObj4<TObj, TArg1, TArg2, TArg3, TArg4>*>(&r);
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
			return new SlotObj4(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3, TArg4);
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	class SlotFunc4 : public Slot4<TArg1, TArg2, TArg3, TArg4>
	{
	public:
		SlotFunc4(void (*pfunc)(TArg1, TArg2, TArg3, TArg4)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		{
			(*_pfunc)(arg1, arg2, arg3, arg4);
		}

		virtual bool operator == (const Slot4<TArg1, TArg2, TArg3, TArg4>& r) const
		{
			const SlotFunc4<TArg1, TArg2, TArg3, TArg4>* rr = dynamic_cast<const SlotFunc4<TArg1, TArg2, TArg3, TArg4>*>(&r);
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
			return new SlotFunc4(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3, TArg4);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4> class Signal4;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	class SlotSignal4 : public Slot4<TArg1, TArg2, TArg3, TArg4>, public SlotSignal
	{
	public:
		SlotSignal4(Signal4<TArg1, TArg2, TArg3, TArg4>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		{
			(*_signal)(arg1, arg2, arg3, arg4);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal4<TArg1, TArg2, TArg3, TArg4>* rr = dynamic_cast<const SlotSignal4<TArg1, TArg2, TArg3, TArg4>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal4(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal4<TArg1, TArg2, TArg3, TArg4>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	class Signal4 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot4<TArg1, TArg2, TArg3, TArg4>*)*it))(arg1,arg2,arg3,arg4);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4))
		{
			BaseSignal::Connect(new SlotObj4<TObj, TArg1, TArg2, TArg3, TArg4>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4))
		{
			SlotObj4<TObj, TArg1, TArg2, TArg3, TArg4> slot = SlotObj4<TObj, TArg1, TArg2, TArg3, TArg4>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg4))
		{
			BaseSignal::Connect(new SlotFunc4<TArg1, TArg2, TArg3, TArg4>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4))
		{
			SlotFunc4<TArg1, TArg2, TArg3, TArg4> slot = SlotFunc4<TArg1, TArg2, TArg3, TArg4>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal4<TArg1, TArg2, TArg3, TArg4>& signal)
		{
			BaseSignal::Connect(new SlotSignal4<TArg1, TArg2, TArg3, TArg4>(&signal, this));
		}

		void DisConnect(Signal4<TArg1, TArg2, TArg3, TArg4>& signal)
		{
			SlotSignal4<TArg1, TArg2, TArg3, TArg4> slot = SlotSignal4<TArg1, TArg2, TArg3, TArg4>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}