
#include "bufmgr.h"
#include "frame.h"
#include "lru.h"
#include "mru.h"

//--------------------------------------------------------------------
// Constructor for BufMgr
//
// Input   : bufSize  - number of frames(pages) in the this buffer manager
//           replacementPolicy - a replacement policy, either LRU or MRU            
// Output  : None
// PostCond: All frames are empty.
//           the "replacer" is initiated to LRU or MRU according to the
//           replacement policy.
//--------------------------------------------------------------------
BufMgr::BufMgr(int bufSize, const char* replacementPolicy)
{
	//TODO: add your code here
	int i=0; Frame* frameTraverser; 
	numFrames= bufSize; //includes empty and occupied frames
	frames = new Frame[bufSize]; //calls  constuctor Frame();

/*
	frames= (Frame*) malloc (sizeof(Frame)*bufSize);
	frameTraverser= frames;
	for(i=0; i<numFrames;i++) {
		frameTraverser-> EmptyIt();
		frameTraverser++;
	}
*/
	totalCall=0;		//total number of pin requests 
	totalHit=0;		//total number of pin requests that result in a hit
						//(i.e., the page is in the buffer when the pin request is made).
	numDirtyPageWrites=0; //total number of dirty pages written back to disk
	//int strncmp ( const char * str1, const char * str2, size_t num );
	if(!(strncmp(replacementPolicy,"LRU",3))) { replacer=  new LRU();}
	if(!(strncmp(replacementPolicy,"MRU",3))) { replacer = new MRU();}

}


//--------------------------------------------------------------------
// Destructor for BufMgr
//
// Input   : None
// Output  : None
//--------------------------------------------------------------------
BufMgr::~BufMgr()
{   
	//TODO: add your code here
	int i=0;
	Frame* frameTraverser= frames;
	for(i=0; i<numFrames;i++) {
		if(frameTraverser->IsValid() && frameTraverser->IsDirty()) {
			//Status DB::WritePage(PageID pageno, Page* pageptr) // writes out the given page to disk.
			MINIBASE_DB->WritePage(frameTraverser->GetPageID(), frameTraverser->GetPage());
			numDirtyPageWrites++;
		}
	//frameTraverser->~Frame(); if used malloc
	frameTraverser++;
	}
	//free(frames); //if used: frames= (Frame*) malloc (sizeof(Frame)*bufSize);
	delete [] frames; //calls ~Frame(); if used: frames = new Frame[bufSize];
	delete replacer;
}

//--------------------------------------------------------------------
// BufMgr::PinPage
//
// Input    : pid     - page id of a particular page 
//            isEmpty - (optional, default to false) if true indicate
//                      that the page to be pinned is an empty page.
// Output   : page - a pointer to a page in the buffer pool. (NULL
//            if fail)
// Purpose  : Pin the page with page id = pid to the buffer.  
//            Read the page from disk unless isEmpty is true or unless
//            the page is already in the buffer.
// Condition: Either the page is already in the buffer, or there is at
//            least one frame available in the buffer pool for the 
//            page.
// PostCond : The page with page id = pid resides in the buffer and 
//            is pinned. The number of pin on the page increase by
//            one.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------
Status BufMgr::PinPage(PageID pid, Page*& page, bool isEmpty)
{
	//TODO: add your code here
	int i=0; bool pageInBufferAlready=false; bool pagePinned=false; 
	int frameSlot; Frame* currentFrame;
	Frame* frameTraverser=frames;

	totalCall++;
	for(i=0; i<numFrames;i++) { 
		if(frameTraverser->GetPageID() ==pid) {
			frameTraverser->Pin();
			replacer->RemoveFrame(i);
			page=frameTraverser->GetPage();
			 pageInBufferAlready=true;
			 pagePinned=true;
			 totalHit++; //# of PinPage requests that result in a hit 
						//(i.e., the page is in the buffer when the pin request is made).
			break;
		}
		frameTraverser++;
	}
	if (pageInBufferAlready==true) return OK;
	
	//PinPage request is a miss, find free frame
	frameTraverser=frames;
	if (pageInBufferAlready==false) {
		for(i=0; i<numFrames;i++) { 
			if(!(frameTraverser->IsValid())) {//If is an empty frame (not just unpinned(pinCount==0))
			frameTraverser->Pin();
			replacer->RemoveFrame(i);
			frameTraverser->SetPageID(pid);
			MINIBASE_DB->ReadPage(pid, frameTraverser->GetPage());
			page= frameTraverser->GetPage();
			pagePinned=true;
			break;
			}
		frameTraverser++;
		}
	}
	
	if(pagePinned==true) return OK;

	// no empty frame, find a victim frame to evict: frame's pinCount must==0
	//if(pagePinned==false) 
	frameSlot= replacer->PickVictim(); // also calls replacer->RemoveFrame(frameSlot);
	// if no unpinned frame (frame's pinCount must==0)
	if (frameSlot<0 || frameSlot>= numFrames) return FAIL;
	// else
	currentFrame = frames+frameSlot;
	if((frames+frameSlot)->IsValid() && (frames+frameSlot)->IsDirty()) {
		MINIBASE_DB->WritePage((frames+frameSlot)->GetPageID(), (frames+frameSlot)->GetPage());
		numDirtyPageWrites++;
	}
	(frames+frameSlot)->EmptyIt();
	(frames+frameSlot)->Pin();
	(frames+frameSlot)->SetPageID(pid);

	if (isEmpty == false) {
		MINIBASE_DB->ReadPage(pid, (frames+frameSlot)->GetPage());
	} //if page is empty (true), do not read page from disk to buffer memory

	page= (frames+frameSlot)-> GetPage();
	if(page==NULL) page = new Page();
	return OK;
	
} 

//--------------------------------------------------------------------
// BufMgr::UnpinPage
//
// Input    : pid     - page id of a particular page 
//            dirty   - indicate whether the page with page id = pid
//                      is dirty or not. (Optional, default to false)
// Output   : None
// Purpose  : Unpin the page with page id = pid in the buffer. Mark 
//            the page dirty if dirty is true.  
// Condition: The page is already in the buffer and is pinned.
// PostCond : The page is unpinned and the number of pin on the
//            page decrease by one. 
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------
Status BufMgr::UnpinPage(PageID pid, bool dirty)
{
	//TODO: add your code here
	int i=0; bool pageInBufferAlready=false; bool pagePinned=false; 
	Frame* frameTraverser=frames;
	for(i=0; i<numFrames;i++) { 
		if(frameTraverser->GetPageID() ==pid) {
			if (frameTraverser->GetPinCount()<=0) return FAIL; //if already unpinned, fail
			frameTraverser->Unpin();	//add to replacement list if pinCount==0 (frame unpinned)
			if (frameTraverser->GetPinCount()==0) replacer->AddFrame(i);

			if (dirty==true) frameTraverser->DirtyIt();

			 pageInBufferAlready=true;
			 pagePinned=true;
			break;
		}
		frameTraverser++;
	}
	if (pageInBufferAlready==false) return FAIL;

	//UNPINNED SUCCESSFULLY
	return OK;
}

//--------------------------------------------------------------------
// BufMgr::NewPage
//
// Input    : howMany - (optional, default to 1) how many pages to 
//                      allocate.
// Output   : firstPid  - the page id of the first page (as output by
//                   DB::AllocatePage) allocated.
//            firstPage - a pointer to the page in memory.
// Purpose  : Allocate howMany number of pages, and pin the first page
//            into the buffer. 
// Condition: howMany > 0 and there is at least one free buffer space
//            to hold a page.
// PostCond : The page with page id = pid is pinned into the buffer.
// Return   : OK if operation is successful.  FAIL otherwise.
// Note     : You can call DB::AllocatePage() to allocate a page.  
//            You should call DB:DeallocatePage() to deallocate the
//            pages you allocate if you failed to pin the page in the
//            buffer.
//--------------------------------------------------------------------
Status BufMgr::NewPage (PageID& firstPid, Page*& firstPage, int howMany)
{
	//TODO: add your code here
	int i=0; bool pageInBufferAlready=false; bool freeFrame=false; 

	Frame* frameTraverser= frames;
	if (howMany<=0 ) return FAIL;

	for(i=0; i<numFrames;i++) { 
			//if(!(frameTraverser->IsValid())) { //If is a free frame
			if(frameTraverser->GetPinCount() <=0) {
				freeFrame=true; //can be new, unpinned (can evict), or emptied frame
			break;
			}
		frameTraverser++;
		}

	if(freeFrame==false) return FAIL;

	//AllocatePage(PageID& start_page_num, int run_size_int)
	MINIBASE_DB->AllocatePage(firstPid, howMany); 
	if(PinPage(firstPid, firstPage, true) == FAIL) {//if pin page failed, deallocate page
		MINIBASE_DB->DeallocatePage(firstPid, howMany);
		//DeallocatePage(PageID start_page_num, int run_size)
		return FAIL;
	}
	return OK;

}

//--------------------------------------------------------------------
// BufMgr::FreePage
//
// Input    : pid     - page id of a particular page 
// Output   : None
// Purpose  : Free the memory allocated for the page with 
//            page id = pid  
// Condition: Either the page is already in the buffer and is pinned
//            no more than once, or the page is not in the buffer.
// PostCond : The page is unpinned, and the frame where it resides in
//            the buffer pool is freed.  Also the page is deallocated
//            from the database. 
// Return   : OK if operation is successful.  FAIL otherwise.
// Note     : You can call MINIBASE_DB->DeallocatePage(pid) to
//            deallocate a page.
//--------------------------------------------------------------------
Status BufMgr::FreePage(PageID pid)
{
	//TODO: add your code here
	int i=0; bool pageInBufferAlready=false; 

	Frame* frameTraverser=frames;
	for(i=0; i<numFrames;i++) { 
		if(frameTraverser->GetPageID()==pid) { 
			pageInBufferAlready=true;
			if (frameTraverser-> GetPinCount() >1 
				|| frameTraverser-> GetPinCount()<0) return FAIL;
			if (frameTraverser-> GetPinCount() ==1 ||
				frameTraverser-> GetPinCount() ==0){//page pinCount must==1(or 0) to unpin, deallocate
				if (frameTraverser-> IsDirty()) {
					MINIBASE_DB->WritePage(frameTraverser->GetPageID(), frameTraverser->GetPage());
					numDirtyPageWrites++;
				}//You don't have to write the page back as it will be deallocated anyway
			frameTraverser-> Unpin();		//(it's just redundant)
			frameTraverser-> EmptyIt(); // do last b/c destructive of Frame members
			//MINIBASE_DB->DeallocatePage(pid);
			break;			
			}
			
		}
	frameTraverser++;
	}
	MINIBASE_DB->DeallocatePage(pid); //deallocate page from database even if not in buffer pool
	return OK;
}


//--------------------------------------------------------------------
// BufMgr::FlushPage
//
// Input    : pid  - page id of a particular page 
// Output   : None
// Purpose  : Flush the page with the given pid to disk.
// Condition: The page with page id = pid must be in the buffer,
//            and is not pinned. pid cannot be INVALID_PAGE.
// PostCond : The page with page id = pid is written to disk if it's dirty. 
//            The frame where the page resides is empty.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------
Status BufMgr::FlushPage(PageID pid)
{
	//TODO: add your code here
	int i=0; bool pageInBufferAlready=false; 

	Frame* frameTraverser=frames;
	for(i=0; i<numFrames;i++) { 
		if(frameTraverser->GetPageID()==pid) { 
			pageInBufferAlready=true;
			if (frameTraverser-> GetPinCount() >=1) return FAIL;
			if (frameTraverser-> IsDirty()) {
				MINIBASE_DB->WritePage(frameTraverser->GetPageID(), frameTraverser->GetPage());
				numDirtyPageWrites++;
				}
				frameTraverser-> Unpin();
				frameTraverser-> EmptyIt(); // do last b/c destructive of Frame members
					
			break;
		}
		frameTraverser++;
	}
	return OK;	
} 

//--------------------------------------------------------------------
// BufMgr::FlushAllPages
//
// Input    : None
// Output   : None
// Purpose  : Flush all pages in this buffer pool to disk.
// Condition: All pages in the buffer pool must not be pinned.
// PostCond : All dirty pages in the buffer pool are written to 
//            disk (even if some pages are pinned). All frames are empty.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------

Status BufMgr::FlushAllPages()
{
	//TODO: add your code here
	int i=0; 
	Frame* frameTraverser= frames;
	for(i=0; i<numFrames;i++) { 
		if (frameTraverser-> GetPinCount() >=1) return FAIL;
		if (frameTraverser-> IsDirty()) {
				MINIBASE_DB->WritePage(frameTraverser->GetPageID(), frameTraverser->GetPage());
				numDirtyPageWrites++;
			}
		frameTraverser-> Unpin();
		frameTraverser-> EmptyIt(); // do last b/c destructive of Frame members
			//MINIBASE_DB->DeallocatePage(pid);	

	frameTraverser++;
	}
	return OK;
}


//--------------------------------------------------------------------
// BufMgr::GetNumOfUnpinnedFrames
//
// Input    : None
// Output   : None
// Purpose  : Find out how many unpinned locations are in the buffer
//            pool.
// Condition: None
// PostCond : None
// Return   : The number of unpinned buffers in the buffer pool.
//--------------------------------------------------------------------
unsigned int BufMgr::GetNumOfUnpinnedFrames()
{
	//TODO: add your code here
	unsigned int c=0;
	int i=0; 
	Frame* frameTraverser= frames;
	for(i=0; i<numFrames;i++) { 
		if (frameTraverser-> GetPinCount() ==0) c++;
		frameTraverser++;
	}
	return c;
}

//--------------------------------------------------------------------
// BufMgr::FindFrame
//
// Input    : pid - a page id 
// Output   : None
// Purpose  : Look for the page in the buffer pool, return the frame
//            number if found.
// PreCond  : None
// PostCond : None
// Return   : the frame number if found. INVALID_FRAME otherwise.
//--------------------------------------------------------------------
int BufMgr::FindFrame( PageID pid )
{
	//TODO: add your code here
	int i=0; 
	Frame* frameTraverser= frames;
	for(i=0; i<numFrames;i++) { 
		if (frameTraverser-> GetPageID()==pid) return i;
		frameTraverser++;
	}
	return INVALID_FRAME;
}



void BufMgr::ResetStat() { 
	totalHit = 0; 
	totalCall = 0; 
	numDirtyPageWrites = 0;
}

void  BufMgr::PrintStat() {
	cout<<"**Buffer Manager Statistics**"<<endl;
	cout<<"Number of Dirty Pages Written to Disk: "<<numDirtyPageWrites<<endl;
	cout<<"Number of Pin Page Requests: "<<totalCall<<endl;
	cout<<"Number of Pin Page Request Misses "<<totalCall-totalHit<<endl;
}

