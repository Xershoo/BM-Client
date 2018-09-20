#include "CLCourseInfoContainer.h"

namespace biz
{
	CLCourseInfoContainer::CLCourseInfoContainer()
	{

	}

	CLCourseInfoContainer::~CLCourseInfoContainer()
	{
		for(int i=0;i<m_vecCourseInfo.size();i++)
		{
			SLCourseInfo* pci = m_vecCourseInfo.at(i);
			if(NULL!=pci)
			{
				delete pci;
				pci = NULL;
			}
		}
		m_vecCourseInfo.clear();
	}

	int CLCourseInfoContainer::GetCourseCount()
	{
		return (int)m_vecCourseInfo.size();
	}

	 SLCourseInfo& CLCourseInfoContainer::GetCourseInfoById(__int64 nCourseId)
	 {	
		 for(int i=0;i<m_vecCourseInfo.size();i++)
		 {
			 if(m_vecCourseInfo[i]->_nCourseId == nCourseId)
			 {
				 return *m_vecCourseInfo[i];
			 }
		 }

		 SLCourseInfo * pSci = new SLCourseInfo();
		 
		 pSci->_nCourseId =nCourseId;

		 m_vecCourseInfo.push_back(pSci);

		 return *pSci;
	 }

	 SLCourseInfo& CLCourseInfoContainer::GetCourseInfoByIndex(int nIndex)
	 {
		 SLCourseInfo* sci = NULL;
		 if(nIndex < m_vecCourseInfo.size())
		 {
			 sci = m_vecCourseInfo[nIndex];
		 }

		 return *sci;
	 }

	 void CLCourseInfoContainer::UpdateCourseInfo(SLCourseInfo& scInfo)
	 {
		 for(int i=0;i<m_vecCourseInfo.size();i++)
		 {
			 if(m_vecCourseInfo[i]->_nCourseId == scInfo._nCourseId)
			 {
				 *m_vecCourseInfo[i] = scInfo;

				 return;
			 }
		 }
		 
		 SLCourseInfo * pSci = new SLCourseInfo();

		 *pSci = scInfo;

		 m_vecCourseInfo.push_back(pSci);
		 
		 
		 return;
	}
}