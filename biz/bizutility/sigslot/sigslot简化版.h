#pragma once

#include <vector>

// 简化版没有考虑信号或槽析构的时候对连接的影响，如果槽子析构，发出信号会引用析构对象。


namespace biz_utility
{
	// 0 Parameter
	class Slot0
	{
	public:
		virtual void operator ()() = 0;
		virtual bool operator == (const Slot0&) = 0;
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

		virtual bool operator == (const Slot0& r)
		{
			const SlotObj0<TObj>* rr = dynamic_cast<const SlotObj0<TObj>*>(&r);
			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

	private:
		TObj*	_obj;
		void (TObj::*_pfunc)();
	};


	class Signal0
	{
	public:
		void operator ()()
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				(**it)();
			}
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)())
		{
			Connect(new SlotObj0<TObj>(obj, pfunc));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)())
		{
			SlotObj0<TObj> slot = SlotObj0<TObj>(obj, pfunc);
			DisConnect(&slot);
		}

		// global function todo ???


	private:
		void Connect(Slot0* slot)
		{
			std::vector<Slot0*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
					return;
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
					delete *it;
					_vSlots.erase(it);
					return;
				}
			}
		}


	private:
		std::vector<Slot0*>	_vSlots;
	};


	// 1 Parameter
	template<typename TArg1>
	class Slot1
	{
	public:
		virtual void operator ()(TArg1) = 0;
		virtual bool operator == (const Slot1<TArg1>&) = 0;
	};


	template<typename TObj, typename TArg1>
	class SlotObj1 : public Slot1<TArg1>
	{
	public:
		SlotObj1(TObj* obj, void (TObj::*pfunc)(TArg1))
			: _obj(obj), _pfunc(pfunc){}

		virtual void operator ()(TArg1 arg1)
		{
			(_obj->*_pfunc)(arg1);
		}

		virtual bool operator == (const Slot1<TArg1>& r)
		{
			const SlotObj1<TObj, TArg1>* rr = dynamic_cast<const SlotObj1<TObj, TArg1>*>(&r);
			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1);
	};

	template<typename TArg1>
	class Signal1
	{
	public:
		void operator ()(TArg1 arg1)
		{
			std::vector<Slot1<TArg1>*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				(**it)(arg1);
			}
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1))
		{
			Connect(new SlotObj1<TObj, TArg1>(obj, pfunc));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1))
		{
			SlotObj1<TObj, TArg1> slot = SlotObj1<TObj, TArg1>(obj, pfunc);
			DisConnect(&slot);
		}


	private:
		void Connect(Slot1<TArg1>* slot)
		{
			std::vector<Slot1<TArg1>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
					return;
			}
			_vSlots.push_back(slot);
		}

		void DisConnect(Slot1<TArg1>* slot)
		{
			std::vector<Slot1<TArg1>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
				{
					delete *it;
					_vSlots.erase(it);
					return;
				}
			}
		}


	private:
		std::vector<Slot1<TArg1>*>	_vSlots;
	};


	// 2 Parameter
	template<typename TArg1, typename TArg2>
	class Slot2
	{
	public:
		virtual void operator ()(TArg1, TArg2) = 0;
		virtual bool operator == (const Slot2<TArg1, TArg2>&) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2>
	class SlotObj2 : public Slot2<TArg1, TArg2>
	{
	public:
		SlotObj2(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2))
			: _obj(obj), _pfunc(pfunc){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2)
		{
			(_obj->*_pfunc)(arg1, arg2);
		}

		virtual bool operator == (const Slot2<TArg1, TArg2>& r)
		{
			const SlotObj2<TObj, TArg1, TArg2>* rr =
				dynamic_cast<const SlotObj2<TObj, TArg1, TArg2>*>(&r);
			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2);
	};


	template<typename TArg1, typename TArg2>
	class Signal2
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2)
		{
			std::vector<Slot2<TArg1, TArg2>*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				(**it)(arg1, arg2);
			}
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2))
		{
			Connect(new SlotObj2<TObj, TArg1, TArg2>(obj, pfunc));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2))
		{
			SlotObj2<TObj, TArg1, TArg2> slot = SlotObj2<TObj, TArg1, TArg2>(obj, pfunc);
			DisConnect(&slot);
		}


	private:
		void Connect(Slot2<TArg1, TArg2>* slot)
		{
			std::vector<Slot2<TArg1, TArg2>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
					return;
			}
			_vSlots.push_back(slot);
		}

		void DisConnect(Slot2<TArg1, TArg2>* slot)
		{
			std::vector<Slot2<TArg1, TArg2>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
				{
					delete *it;
					_vSlots.erase(it);
					return;
				}
			}
		}


	private:
		std::vector<Slot2<TArg1, TArg2>*>	_vSlots;
	};


	// 3 Parameter
	template<typename TArg1, typename TArg2, typename TArg3>
	class Slot3
	{
	public:
		virtual void operator ()(TArg1, TArg2, TArg3) = 0;
		virtual bool operator == (const Slot3<TArg1, TArg2, TArg3>&) = 0;
	};


	template<typename TObj, typename TArg1, typename TArg2, typename TArg3>
	class SlotObj3 : public Slot3<TArg1, TArg2, TArg3>
	{
	public:
		SlotObj3(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3))
			: _obj(obj), _pfunc(pfunc){}

		virtual void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			(_obj->*_pfunc)(arg1, arg2, arg3);
		}

		virtual bool operator == (const Slot3<TArg1, TArg2, TArg3>& r)
		{
			const SlotObj3<TObj, TArg1, TArg2, TArg3>* rr =
				dynamic_cast<const SlotObj3<TObj, TArg1, TArg2, TArg3>*>(&r);
			return (_obj == rr->_obj && _pfunc == rr->_pfunc);
		}

	private:
		TObj*	_obj;
		void (TObj::*_pfunc)(TArg1, TArg2, TArg3);
	};


	template<typename TArg1, typename TArg2, typename TArg3>
	class Signal3
	{
	public:
		void operator ()(TArg1 arg1, TArg2 arg2, TArg3 arg3)
		{
			std::vector<Slot3<TArg1, TArg2, TArg3>*>::iterator it = _vSlots.begin();
			for (;it!=_vSlots.end(); ++it)
			{
				(**it)(arg1, arg2, arg3);
			}
		}

		template<typename TObj>
		void Connect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3))
		{
			Connect(new SlotObj3<TObj, TArg1, TArg2, TArg3>(obj, pfunc));
		}

		template<typename TObj>
		void DisConnect(TObj* obj, void (TObj::*pfunc)(TArg1, TArg2, TArg3))
		{
			SlotObj3<TObj, TArg1, TArg2, TArg3> slot =
				SlotObj3<TObj, TArg1, TArg2, TArg3>(obj, pfunc);
			DisConnect(&slot);
		}


	private:
		void Connect(Slot3<TArg1, TArg2, TArg3>* slot)
		{
			std::vector<Slot3<TArg1, TArg2, TArg3>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
					return;
			}
			_vSlots.push_back(slot);
		}

		void DisConnect(Slot3<TArg1, TArg2, TArg3>* slot)
		{
			std::vector<Slot3<TArg1, TArg2, TArg3>*>::iterator it = _vSlots.begin();
			for (; it!=_vSlots.end(); ++it)
			{
				if ((**it) == *slot)
				{
					delete *it;
					_vSlots.erase(it);
					return;
				}
			}
		}


	private:
		std::vector<Slot3<TArg1, TArg2, TArg3>*>	_vSlots;
	};
}

