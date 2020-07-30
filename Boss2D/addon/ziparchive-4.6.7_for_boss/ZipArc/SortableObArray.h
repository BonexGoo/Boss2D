/*
*
* Project : ZipArc                      
* Copyright (c) 2001 Tadeusz Dracz
*
* You are free to use and distribute as long as this copyright notice remains intact.
* May not be sold for profit.
*
* Based on work by Douglas Peterson. 
* http://www.codeguru.com/cpp_mfc/sortable_cobarray.shtml
*****************************************************/


#if !defined(AFX_SORTABLEOBARRAY_H__DD07497E_3DB8_468D_B629_3C9F7351BD63__INCLUDED_)
#define AFX_SORTABLEOBARRAY_H__DD07497E_3DB8_468D_B629_3C9F7351BD63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
///////////////////////////// NOTE /////////////////////////////////////
// It is faster to add elements to the array first and then to sort them,
// than to insert them with the function InsertSorted; use this function
// to insert to the sorted array an amount of elements that is a little 
// more than one third of the array size (e.g if the array has 7500 elements
// add with this method 2500 elements)	
////////////////////////////////////////////////////////////////////////
typedef int (*SOA_SORTFUNCTION )(CObject*, CObject*, WPARAM);
class CSortableObArray : public CObArray  
{
protected:
	void Clear();
	SOA_SORTFUNCTION m_pSortFunction;
public:
	WPARAM m_UserData;
	void SetSortFunction(SOA_SORTFUNCTION pFunction, WPARAM lpUserData);
	void RemoveAt( INT_PTR nIndex, INT_PTR nCount = 1 );
	bool m_bDestroyOnDelete;
	void RemoveAll();
	void Sort();
	INT_PTR InsertSorted(CObject *newElement);
	CSortableObArray();
	virtual ~CSortableObArray();

};


template< class TYPE >
class CTypedSortableObArray : public CSortableObArray
{
public:
	// Accessing elements
	TYPE GetAt(INT_PTR nIndex) const
	{ return (TYPE)CSortableObArray::GetAt(nIndex); }
	TYPE& ElementAt(INT_PTR nIndex)
	{ return (TYPE&)CSortableObArray::ElementAt(nIndex); }
	void SetAt(INT_PTR nIndex, TYPE ptr)
	{ CSortableObArray::SetAt(nIndex, ptr); }
	
	// Potentially growing the array
	void SetAtGrow(INT_PTR nIndex, TYPE newElement)
	{ CSortableObArray::SetAtGrow(nIndex, newElement); }
	INT_PTR Add(TYPE newElement)
	{ return CSortableObArray::Add(newElement); }
// 	int Append(const CTypedPtrArray< CSortableObArray, TYPE >& src)
// 	{ return CSortableObArray::Append(src); }
// 	void Copy(const CTypedPtrArray< CSortableObArray, TYPE >& src)
// 	{ CSortableObArray::Copy(src); }
	
	// Operations that move elements around
	INT_PTR InsertSorted(TYPE newElement)
	{return CSortableObArray::InsertSorted(newElement);}
	void InsertAt(INT_PTR nIndex, TYPE newElement, INT_PTR nCount = 1)
	{ CSortableObArray::InsertAt(nIndex, newElement, nCount); }
	void InsertAt(INT_PTR nStartIndex, CTypedSortableObArray< TYPE >* pNewArray)
	{ CSortableObArray::InsertAt(nStartIndex, pNewArray); }
	
	// overloaded operator helpers
	TYPE operator[](INT_PTR nIndex) const
	{ return (TYPE)CSortableObArray::operator[](nIndex); }
	TYPE& operator[](INT_PTR nIndex)
	{ return (TYPE&)CSortableObArray::operator[](nIndex); }
	void SetSortFunction(int (*pSortFunction)(TYPE , TYPE , WPARAM ), WPARAM P)
	{ CSortableObArray::SetSortFunction((int (*)(CObject*, CObject*, WPARAM)) pSortFunction, P);}
};



#endif // !defined(AFX_SORTABLEOBARRAY_H__DD07497E_3DB8_468D_B629_3C9F7351BD63__INCLUDED_)
