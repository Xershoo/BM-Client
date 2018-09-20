#pragma once


namespace biz_utility
{
	template<typename TArg1, typename TArg2, typename TArg3>
	class Slot3 : public BaseSlot
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3>
	class SlotObj3 : public Slot3<TArg1, TArg2, TArg3>
	{
	public:
		SlotObj3(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3), BaseSignal* signal)
			: _obj(obj), _pfunc(pfunc)
		{
			obj->_src_sig_container.AddSignal(signal);
		}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotObj3<TObj, TArg1, TArg2, TArg3>* rr = dynamic_cast<const SlotObj3<TObj, TArg1, TArg2, TArg3>*>(&r);
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
			return new SlotObj3(_obj, _pfunc, signal);
		}


	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3);
	};


	template<typename TArg1, typename TArg2, typename TArg3>
	class SlotFunc3 : public Slot3<TArg1, TArg2, TArg3>
	{
	public:
		SlotFunc3(void (*pfunc)(TArg1, TArg2, TArg3)) : _pfunc(pfunc){}
		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			(*_pfunc)(arg1, arg2, arg3);
		}

		virtual bool operator == (const Slot3<TArg1, TArg2, TArg3>& r) const
		{
			const SlotFunc3<TArg1, TArg2, TArg2>* rr = dynamic_cast<const SlotFunc3<TArg1, TArg2, TArg2>*>(&r);
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
			return new SlotFunc3(_pfunc);
		}


	private:
		void (*_pfunc)(TArg1, TArg2, TArg3);
	};


	// slot 的信号包装
	template<typename TArg1, typename TArg2, typename TArg3> class Signal3;
	template<typename TArg1, typename TArg2, typename TArg3>
	class SlotSignal3 : public Slot3<TArg1, TArg2, TArg3>, public SlotSignal
	{
	public:
		SlotSignal3(Signal3<TArg1, TArg2, TArg3>* signal, BaseSignal* srcsignal)
			:SlotSignal(signal, srcsignal),
			_signal(signal){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			(*_signal)(arg1, arg2, arg3);
		}

		virtual bool operator == (const BaseSlot& r) const
		{
			const SlotSignal3<TArg1,TArg2,TArg3>* rr = dynamic_cast<const SlotSignal3<TArg1,TArg2,TArg3>*>(&r);
			if (NULL == rr)
				return false;

			return (_signal == rr->_signal);
		}

		virtual BaseSlot* Clone4(BaseSignal* bsignal) const
		{
			return new SlotSignal3(_signal, bsignal);
		}

		virtual void OnInnerSignalDestory()
		{
			_srcsignal->DisConnect(this);
		}


	private:
		Signal3<TArg1,TArg2,TArg3>*	_signal;		// 被包装的信号
	};


	template<typename TArg1, typename TArg2, typename TArg3>
	class Signal3 : public BaseSignal
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			std::vector<BaseSlot*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				_currentRunSlot = *it;
				(*((Slot3<TArg1,TArg2,TArg3>*)*it))(arg1,arg2,arg3);
				_currentRunSlot = NULL;
			}
			CleanDelayDeletes();
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3))
		{
			BaseSignal::Connect(new SlotObj3<TObj, TArg1, TArg2, TArg3>(obj, pfunc, this));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3))
		{
			SlotObj3<TObj, TArg1, TArg2, TArg3> slot = SlotObj3<TObj, TArg1, TArg2, TArg3>(obj, pfunc, this);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(void (*pfunc)(TArg1, TArg2, TArg3))
		{
			BaseSignal::Connect(new SlotFunc3<TArg1, TArg2, TArg3>(pfunc));
		}

		void DisConnect(void (*pfunc)(TArg1, TArg2, TArg3))
		{
			SlotFunc3<TArg1, TArg2, TArg3> slot = SlotFunc2<TArg1, TArg2, TArg3>( pfunc);
			BaseSignal::DisConnect(&slot);
		}

		void Connect(Signal3<TArg1,TArg2,TArg3>& signal)
		{
			BaseSignal::Connect(new SlotSignal3<TArg1,TArg2,TArg3>(&signal, this));
		}

		void DisConnect(Signal3<TArg1,TArg2,TArg3>& signal)
		{
			SlotSignal3<TArg1,TArg2,TArg3> slot = SlotSignal3<TArg1,TArg2,TArg3>(&signal, this);
			BaseSignal::DisConnect(&slot);
		}
	};
}