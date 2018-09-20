#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;

class SigContariner
{
public:
	Signal1<int> _sig1;
};


class SlotContainer
{
public:
	SLOT_OBJ;

	void slot1(int k)
	{
		printf("SlotContainer::slot1(%d)\n", k);
	}

	void slot2(int k)
	{
		printf("SlotContainer::slot2(%d)\n", k);
		//delete this;
	}

	virtual void slot3(int k)
	{
		printf("SlotContainer::slot3(%d)\n", k);
	}
};

class SlotContainerX : public SlotContainer
{
public:
	virtual void slot3(int k)
	{
		printf("SlotContainerX::slot3(%d)\n", k);
	}
};

void slot3(int k)
{
	printf("slot3(%d)\n", k);
}


void test_sigslot1()
{
	SigContariner* pSigCont = new SigContariner();
	SlotContainer* pSlotCont = new SlotContainer();

	// 1
	pSigCont->_sig1.Connect(pSlotCont, &SlotContainer::slot1);
	pSigCont->_sig1.Connect(pSlotCont, &SlotContainer::slot2);
	pSigCont->_sig1(1);

	// 2
	pSigCont->_sig1.DisConnect(pSlotCont, &SlotContainer::slot1);
	//pSigCont->_sig1(2);

	// 3
	//delete pSlotCont;
	//pSigCont->_sig1(3);

	// 4 global
	pSigCont->_sig1.Connect(slot3);
	pSigCont->_sig1(4);
	pSigCont->_sig1.DisConnect(slot3);
	pSigCont->_sig1(5);

	// 5
	//for (int i=0; i<1000000000; ++i)
	//{
	//	pSigCont->_sig1.Connect(pSlotCont, &SlotContainer::slot1);
	//	pSigCont->_sig1.DisConnect(pSlotCont, &SlotContainer::slot1);
	//	pSigCont->_sig1(i);
	//}

	// 6 对继承对象，多态的支持.
	SlotContainer* pSlotContX = new SlotContainerX();
	pSigCont->_sig1.Connect(pSlotContX, &SlotContainer::slot3);
	pSigCont->_sig1(6);

	// 7 定向发送
	pSigCont->_sig1.SetDest(pSlotContX);
	pSigCont->_sig1(7);
}