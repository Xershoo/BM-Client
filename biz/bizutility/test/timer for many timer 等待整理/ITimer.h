#if !defined(ITimer_H_Createdby_Neou_20041231__INCLUDED)
#define ITimer_H_Createdby_Neou_20041231__INCLUDED


struct ITimerSenseObj 
{
	virtual ~ITimerSenseObj(){}
	virtual void TimeArrive() = 0;
};

//struct ITimer
//{
//	virtual BOOL SetTick(UINT millisecond) = 0;
//	virtual void SynPulse() = 0;
//
//	virtual void RegisterListener(ITimerSenseObj* listener,UINT timeSpan) = 0;
//	virtual void UnRegisterListener(ITimerSenseObj* listener) = 0;
//};

#endif //ITimer_H_Createdby_Neou_20041231__INCLUDED