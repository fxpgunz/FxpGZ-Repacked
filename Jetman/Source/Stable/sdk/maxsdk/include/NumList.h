//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Tab.h"

// Simple list of numbers
/*! \sa  Template Class Tab.\n\n
\par Description:
This class maintains a simple list of integer numbers. All methods of this
class are implemented by the system.  */
class NumList: public Tab<int> {
	 public:
			/*! \remarks Adds the number to the list.
			\par Parameters:
			<b>int j</b>\n\n
			The number to add.\n\n
			<b>BOOL checkUnique=TRUE</b>\n\n
			If TRUE the number will be added only if it is not in the table already.
			\return  The index in the table of the added number, or the index where
			it was found already if <b>checkUnique</b> is used. */
			int Add(int j, BOOL checkUnique=TRUE);
			/*! \remarks Returns the index in the table of the number passed.
			\par Parameters:
			<b>int j</b>\n\n
			The number to find. */
			int Find(int j);
	 };

inline int NumList::Find(int j) 
{
    int n = Count();
    for (int i=0; i<n; i++) {
        if ((*this)[i]==j) return i;
    }
    return -1;
}

inline int NumList::Add(int j, BOOL checkUnique) 
{
    if (checkUnique) {
        int n = Find(j);
        if (n>=0) return n;
    }
    return Append(1,&j,5); 
}


