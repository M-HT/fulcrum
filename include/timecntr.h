// Filename: include\TimeCntr.h
//
// Developed by Oliver Belaifa in 1997
//

#ifndef _TimeCntr_H_INCLUDED
#define _TimeCntr_H_INCLUDED



class CTimeCounter
{
public:
	int		m_iCPUSpeed;
	int		m_iStartRDTSC;
	int		m_iStopRDTSC;
	
	void	Init();
	void	Free();

	void	StartTimeCounter();
	void	StopTimeCounter();
	
	int		GetTSC();
	float	GetFPS();

	CTimeCounter();
	~CTimeCounter();
};


#endif
