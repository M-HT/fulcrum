// Filename: include\m_except.h
//
// Developed by Oliver Belaifa in 1997
//


#ifndef _M2EXCEPT_H_INCLUDED
#define _M2EXCEPT_H_INCLUDED

class CMV2NormException
{
public:
	char *m_cError;
	char *m_cFile;
	long  m_lLineNumber;

	CMV2NormException(const char *cError, const char *, long);
	~CMV2NormException();

	void PrintError();
};

#endif
