#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	class Slot8 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	class SlotObj8 : public Slot8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>
	{
	public:
		SlotObj8(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj8<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>* rr = dynamic_cast<const SlotObj8<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>*>(&r);
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
			return new SlotObj8(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8);
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	class SlotFunc8 : public Slot8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>
	{
	public:
		SlotFunc8(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
		{
			(*_pfunc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		}

		virtual bool operator == (const Slot8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>& r) const
		{
			const SlotFunc8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>* rr = dynamic_cast<const SlotFunc8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>*>(&r);
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
			return new SlotFunc8(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8> class Signal8;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	class SlotSignal8 : public Slot8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>, public SlotSignal
	{
	public:
		SlotSignal8(Signal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
		{
			(*_signal)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>* rr = dynamic_cast<const SlotSignal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal8(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	class Signal8 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>*)*it))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8))
		{
			BaseSignal::Connect(new SlotObj8<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8))
		{
			SlotObj8<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8> slot = SlotObj8<TObj, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg4, TArg5, TArg6, TArg7, TArg8))
		{
			BaseSignal::Connect(new SlotFunc8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8))
		{
			SlotFunc8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8> slot = SlotFunc8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>& signal)
		{
			BaseSignal::Connect(new SlotSignal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>(&signal, this));
		}

		void DisConnect(Signal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>& signal)
		{
			SlotSignal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8> slot = SlotSignal8<TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}