// Filename: TimeCntr.cpp
//
// Developed by Oliver Belaifa in 1997
//

#include<stdlib.h>
#include<time.h>
#include<timecntr.h>


//extern "C"
//{
//	int GetTimeStampCounterPosASM();
//}



void CTimeCounter::StartTimeCounter()
{
//	m_iStartRDTSC = GetTimeStampCounterPosASM();
}


void CTimeCounter::StopTimeCounter()
{
//	m_iStopRDTSC = GetTimeStampCounterPosASM();
}



float CTimeCounter::GetFPS()
{
//	return (float)m_iCPUSpeed/((float)(m_iStopRDTSC - m_iStartRDTSC));
	return 1.0;
}



int CTimeCounter::GetTSC()
{
//	return m_iStopRDTSC - m_iStartRDTSC;
	return 1;
}



void CTimeCounter::Init()
{
	  /*
	int iCurTimePos = (int)clock();
	while(iCurTimePos == (int)clock());

	int iStartRDTSC = GetTimeStampCounterPosASM();
	iCurTimePos = (int)clock();
	while (iCurTimePos == (int)clock());
		// iWaitTime ms warten
	int iStopRDTSC = GetTimeStampCounterPosASM();

	float fCPUSpeed = (float)(iStopRDTSC - iStartRDTSC);
	fCPUSpeed *= 18.29;
	*/
		m_iCPUSpeed = 200000000;//(int)fCPUSpeed;
}



void CTimeCounter::Free()
{
}



CTimeCounter::CTimeCounter()
{
	m_iCPUSpeed = 0;
	m_iStartRDTSC = 0;
	m_iStopRDTSC = 0;
}


CTimeCounter::~CTimeCounter()
{
}




