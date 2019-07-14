// IniFile.cpp: implementation of the CIniReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



// Used to retrieve a value give the section and key
CString CIniReader::GetKeyValue(CString strKey,CString strSection)
{
	TCHAR ac_Result[512];

	// Get the info from the .ini file	
	m_lRetValue = GetPrivateProfileString((LPCTSTR)strSection,(LPCTSTR)strKey,
		_T(""), ac_Result, 512, (LPCTSTR)m_strFileName);	

	CString strResult(ac_Result);
	return strResult;
}

// Used to add or set a key value pair to a section
long CIniReader::SetKey(CString strSection, CString strKey,  CString strValue)
{
	m_lRetValue = WritePrivateProfileString (strSection, strKey, 
                             strValue, m_strFileName);
	return m_lRetValue;
}

// Used to find out if a given section exists
BOOL CIniReader::IsSectionExists(CString strSection)
{
	TCHAR ac_Result[256];	
	CString csAux;
	// Get the info from the .ini file	
	m_lRetValue = GetPrivateProfileString((LPCTSTR)strSection, NULL,
		_T(""), ac_Result, 256, (LPCTSTR)m_strFileName);
	// Return if we could retrieve any info from that section
	return (m_lRetValue > 0);
}

// Used to retrieve all of the  section names in the ini file
CStringList* CIniReader::GetSectionNames()  //returns collection of section names
{
	TCHAR ac_Result[2048];
	m_sectionList->RemoveAll();
	
	m_lRetValue = GetPrivateProfileSectionNames(ac_Result,2048,(LPCTSTR)m_strFileName);
	
	CString strSectionName;
	for(int i=0; i<m_lRetValue; i++)
	{
		if(ac_Result[i] != _T('\0')) 
		{
			strSectionName = strSectionName + ac_Result[i];
		} 
		else 
		{
			if(strSectionName != _T("")) 
			{
				m_sectionList->InsertAfter(m_sectionList->GetTailPosition(),strSectionName);
			}
			strSectionName = _T("");
		}
	}
	return m_sectionList;
}

// Used to retrieve all key/value pairs of a given section.  
CStringList* CIniReader::GetSectionData(CString strSection)  
{
	TCHAR ac_Result[2048];  //change size depending on needs
	m_sectionDataList->RemoveAll();
	m_lRetValue = GetPrivateProfileSection((LPCTSTR)strSection, ac_Result, 2048, (LPCTSTR)m_strFileName);

	CString strSectionData;
	for(int i=0; i<m_lRetValue; i++)
	{
		if(ac_Result[i] != _T('\0')) 
		{
			strSectionData = strSectionData + ac_Result[i];
		} 
		else 
		{
			if(strSectionData != _T("")) 
			{
				m_sectionDataList->InsertAfter(m_sectionDataList->GetTailPosition(),strSectionData);
			}
			strSectionData = _T("");
		}
	}
	return m_sectionDataList;
}

BOOL CIniReader::SetINIFileName(CString strINIFile)
{
	CFile file;
	if (!file.Open(strINIFile, CFile::modeReadWrite))
	{
		if (!file.Open(strINIFile, CFile::modeCreate))
			return FALSE;
	}
	m_strFileName = strINIFile;
	file.Close();
	return TRUE;
}

BOOL CIniReader::DeleteContent()
{
	CFile file;
	if (!file.Open(m_strFileName, CFile::modeReadWrite))
	{
		return FALSE;
	}
	file.SetLength(0);
	file.Close();
	return TRUE;
}
