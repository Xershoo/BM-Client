#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;


Signal4<int, int&, int, std::string> sig4;


class SlotContainer4
{
public:
	SLOT_OBJ;

	void slot1(int k, int& t, int s, std::string str)
	{
		printf("SlotContainer3::slot1(%d, %d, %d, %s)\n", k, t, s, str.c_str());
		t = 19;
	}

	void slot2(int k, int& t, int s, std::string str)
	{
		printf("SlotContainer3::slot2(%d, %d, %d, %s)\n", k, t, s, str.c_str());
		//delete this;
	}
};

void slot3(int k, int& t, int s, std::string str)
{
	printf("slot3(%d£¬%d, %d, %s)\n", k, t, s, str.c_str());
}


void test_sigslot4()
{
	SlotContainer4* pSlotCont = new SlotContainer4();

	// 1
	sig4.Connect(pSlotCont, &SlotContainer4::slot1);
	sig4.Connect(pSlotCont, &SlotContainer4::slot2);
	int t = 9;
	sig4(1, t, 2, "test1");
	printf("t = %d\n", t);

	// 2
	sig4.DisConnect(pSlotCont, &SlotContainer4::slot1);
	sig4(2, t, 3, "test2");

	// 3
	//delete pSlotCont;
	//sig4(3, t, "test3");

	// 4 global
	sig4.Connect(slot3);
	sig4(4, t, 5, "test4");
	sig4.DisConnect(slot3);
	sig4(5, t, 6, "test5");

	// 5
	//for (int i=0; i<1000000000; ++i)
	//{
	//	sig4.Connect(pSlotCont, &SlotContainer4::slot1);
	//	sig4.DisConnect(pSlotCont, &SlotContainer4::slot1);
	//	sig4(i, t, i+1, "test");
	//}

	delete pSlotCont;
}