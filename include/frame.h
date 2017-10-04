#ifndef FRAME_H
#define FRAME_H

#include "page.h"
#include "db.h"

#define INVALID_FRAME -1

class Frame 
{
	private :
	
		PageID pid; //PageID is an int type: typedef int PageID;
		Page   *data;
		int    pinCount;
		bool    dirty;
		
	public :
		
		Frame();
		~Frame();
		void Pin();
		void Unpin();
		int GetPinCount();
		void EmptyIt();
		void DirtyIt();
		void SetPageID(PageID pid);
		bool IsDirty();
		bool IsValid();
		Status Write();
		Status Read(PageID pid);
		bool NotPinned();
		PageID GetPageID();
		Page *GetPage();

};

#endif