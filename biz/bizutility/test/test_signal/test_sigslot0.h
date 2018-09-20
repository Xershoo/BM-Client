#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;


Signal0 sig1;


class SlotContainer0
{
public:
	SLOT_OBJ;

	void slot1()
	{
		printf("SlotContainer0::slot1()\n");
	}

	void slot2()
	{
		printf("SlotContainer0::slot2()\n");
		//delete this;
	}
};

void slot3()
{
	printf("slot3()\n");
}


void test_sigslot0()
{
	
	SlotContainer0* pSlotCont = new SlotContainer0();

	// 1
	sig1.Connect(pSlotCont, &SlotContainer0::slot1);
	sig1.Connect(pSlotCont, &SlotContainer0::slot2);
	sig1();

	// 2
	sig1.DisConnect(pSlotCont, &SlotContainer0::slot1);
	//pSigCont->_sig1(2);

	// 3
	//delete pSlotCont;
	//pSigCont->_sig1(3);

	// 4 global
	sig1.Connect(slot3);
	sig1();
	sig1.DisConnect(slot3);
	sig1();

	// 5
	//for (int i=0; i<1000000000; ++i)
	//{
	//	sig1.Connect(pSlotCont, &SlotContainer0::slot1);
	//	sig1.DisConnect(pSlotCont, &SlotContainer0::slot1);
	//	sig1(i);
	//}
	delete pSlotCont;
}