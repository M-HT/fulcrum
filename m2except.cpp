// Filename: include\m_except.cpp
//
// Developed by Oliver Belaifa in 1997
//


#include<stdio.h>
#include<string.h>
#include<process.h>

#include<gmvesa.h>
#include<m2except.h>
#include<gfxstrio.h>


extern CGfxModeVesa    	   *pVesa;
extern CGfxStrIO 		   *pGfxStrIO;



CMV2NormException::CMV2NormException(char *cError,
									 char *cFile,
									 long lLineNumber)
{
	m_cError = strdup(cError);
	m_cFile = strdup(cFile);

	m_lLineNumber = lLineNumber;
}


CMV2NormException::~CMV2NormException()
{
	delete [] m_cError;
	delete [] m_cFile;

	exit(1);
}


void CMV2NormException::PrintError()
{
	if (pVesa != NULL)
	{
		char pcText[80];
		sprintf(pcText, "%s, in File %s in Line %d.\n", m_cError, m_cFile, m_lLineNumber);
		pGfxStrIO->WriteStr(pcText, (char *)pVesa->m_pMappedLfb);
	}
	else
		printf("%s, in File %s in Line %d.\n", m_cError, m_cFile, m_lLineNumber);
}


