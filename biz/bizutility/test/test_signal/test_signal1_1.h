#pragma once

#include "../sigslot/sigslot.h"
using namespace biz_utility;


class SlotC
{
public:
	SLOT_OBJ;
	void FF(int kk)
	{

	}
} slotc;

void test_signal1()
{
	Signal1<int>	s1;
	Signal1<int>	s2;
	Signal1<int>	s3;
	s1.Connect(&slotc, &SlotC::FF);
	s1.DisConnect(&slotc, &SlotC::FF);
	s1(5);
	s2.Connect(s1);
	s2(6);
	s3.Connect(s1);
	s3(7);
	Signal1<int>	s4 = s3;
	s4(8);

	Signal0				d1;
	Signal0				d2;
	d1.Connect(d2);

	Signal2<int,int>	ss1;
	Signal2<int,int>	ss2;
	ss2.Connect(ss1);

	Signal3<int,int,int>	sss1;
	Signal3<int,int,int>	sss2;
	sss2.Connect(sss1);

	Signal4<int,int,int,int>	ssss1;
	Signal4<int,int,int,int>	ssss2;
	ssss2.Connect(ssss1);

	Signal5<int,int,int,int,int>	sssss1;
	Signal5<int,int,int,int,int>	sssss2;
	sssss2.Connect(sssss1);

	Signal6<int,int,int,int,int,int>	ssssss1;
	Signal6<int,int,int,int,int,int>	ssssss2;
	ssssss2.Connect(ssssss1);

	Signal7<int,int,int,int,int,int,int>	sssssss1;
	Signal7<int,int,int,int,int,int,int>	sssssss2;
	sssssss2.Connect(sssssss1);


	Signal8<int,int,int,int,int,int,int,int>	ssssssss1;
	Signal8<int,int,int,int,int,int,int,int>	ssssssss2;
	ssssssss2.Connect(ssssssss1);
}