#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;


Signal2<int, int&> sig2;


class SlotContainer2
{
public:
	SLOT_OBJ;

	void slot1(int k, int& t)
	{
		printf("SlotContainer::slot1(%d, %d)\n", k, t);
		t = 19;
	}

	void slot2(int k, int& t)
	{
		printf("SlotContainer::slot2(%d, %d)\n", k, t);
		//delete this;
	}
};

void slot3(int k, int& t)
{
	printf("slot3(%d£¬%d)\n", k, t);
}


void test_sigslot2()
{
	SlotContainer2* pSlotCont = new SlotContainer2();

	// 1
	sig2.Connect(pSlotCont, &SlotContainer2::slot1);
	sig2.Connect(pSlotCont, &SlotContainer2::slot2);
	int t = 9;
	sig2(1, t);
	printf("t = %d\n", t);

	// 2
	sig2.DisConnect(pSlotCont, &SlotContainer2::slot1);
	sig2(2, t);

	// 3
	//delete pSlotCont;
	//sig2(3, t);

	// 4 global
	sig2.Connect(slot3);
	sig2(4, t);
	sig2.DisConnect(slot3);
	sig2(5, t);

	// 5
	//for (int i=0; i<1000000000; ++i)
	//{
	//	sig2.Connect(pSlotCont, &SlotContainer2::slot1);
	//	sig2.DisConnect(pSlotCont, &SlotContainer2::slot1);
	//	sig2(i, t);
	//}

	delete pSlotCont;
}