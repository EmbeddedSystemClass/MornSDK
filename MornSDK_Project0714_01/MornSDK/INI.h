// IniFile.h: interface for the CIniReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_)
#define AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcoll.h>

class CIniReader  
{
public:
	// method to set INI file name, if not already specified 
	BOOL SetINIFileName(CString strINIFile);
	BOOL DeleteContent();
	
	// methods to return the lists of section data and section names
	CStringList* GetSectionData(CString strSection);
	CStringList* GetSectionNames();
	
	// check if the section exists in the file
	BOOL IsSectionExists(CString strSection);

	// updates the key value, if key already exists, else creates a key-value pair
	long SetKey(CString strSection, CString strKey,  CString strValue);

	// give the key value for the specified key of a section
	CString GetKeyValue(CString strKey,CString strSection);
	
	// default constructor
	CIniReader()
	{
		m_sectionList = new CStringList();
		m_sectionDataList = new CStringList();
	}
	
	CIniReader(CString strFile)
	{
		m_strFileName = strFile;
		m_sectionList = new CStringList();
		m_sectionDataList = new CStringList();
	}
	
	~CIniReader()
	{
		delete m_sectionList;
		delete m_sectionDataList;
	}
	
private:
	// lists to keep sections and section data
	CStringList *m_sectionDataList;
	CStringList *m_sectionList;
	
	CString m_strSection;
	long m_lRetValue;
	
	// ini file name 
	CString m_strFileName;
};

#endif // !defined(AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_)
