#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;


Signal3<int, int&, int> sig3;


class SlotContainer3
{
public:
	SLOT_OBJ;

	void slot1(int k, int& t, int s)
	{
		printf("SlotContainer3::slot1(%d, %d, %d)\n", k, t, s);
		t = 19;
	}

	void slot2(int k, int& t, int s)
	{
		printf("SlotContainer3::slot2(%d, %d, %d)\n", k, t, s);
		//delete this;
	}
};

void slot3(int k, int& t, int s)
{
	printf("slot3(%d£¬%d, %d)\n", k, t, s);
}


void test_sigslot3()
{
	SlotContainer3* pSlotCont = new SlotContainer3();

	// 1
	sig3.Connect(pSlotCont, &SlotContainer3::slot1);
	sig3.Connect(pSlotCont, &SlotContainer3::slot2);
	int t = 9;
	sig3(1, t, 2);
	printf("t = %d\n", t);

	// 2
	sig3.DisConnect(pSlotCont, &SlotContainer3::slot1);
	sig3(2, t, 3);

	// 3
	//delete pSlotCont;
	//sig3(3, t);

	// 4 global
	sig3.Connect(slot3);
	sig3(4, t, 5);
	sig3.DisConnect(slot3);
	sig3(5, t, 6);

	// 5
	//for (int i=0; i<1000000000; ++i)
	//{
	//	sig3.Connect(pSlotCont, &SlotContainer3::slot1);
	//	sig3.DisConnect(pSlotCont, &SlotContainer3::slot1);
	//	sig3(i, t, i+1);
	//}

	delete pSlotCont;
}