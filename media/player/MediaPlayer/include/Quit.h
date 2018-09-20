#ifndef QUIT_H
#define QUIT_H

#include "MediaPlayer.h"
#include "PlayProcess.h"

class StopUnit
{
public:
	 CPlayProcess * rp;
	 HWND			    hwnd;
	 int            nIndex;
	 CSCThread      m_thread_stop;
public:
	 StopUnit();
	 ~StopUnit();
	 void clear();
	 void stop();
	 void stopEx();
};


#endif