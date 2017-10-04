#include "frame.h"

Frame::Frame() {
		pid=INVALID_PAGE;
		data= new Page();
		pinCount=0;
		dirty=false;
}
Frame::~Frame() {delete data;}
void Frame::Pin() {
	pinCount++;
}
void Frame::Unpin() {
pinCount--;
if(pinCount<0) pinCount=0;
}
int Frame::GetPinCount() {return pinCount;}
void Frame::EmptyIt() { 
		pid=INVALID_PAGE;
		if (data==NULL) {
			//cout << "  data is null" << endl;
			data= new Page();
		}
		pinCount=0;
		dirty=false;
}
void Frame::DirtyIt() {dirty=true;}
void Frame::SetPageID(PageID p) {pid= p;}
bool Frame::IsDirty() { return (dirty==true);}
bool Frame::IsValid() { return (pid!=INVALID_PAGE);}
Status Frame::Write() {
			//Status DB::WritePage(PageID pageno, Page* pageptr) // writes out the given page to disk.
			return(MINIBASE_DB->WritePage(pid,data));
			}
Status Frame::Read(PageID pid) {
	return (MINIBASE_DB->ReadPage(pid, data));
}
bool Frame::NotPinned() {return (pinCount==0);}
PageID Frame::GetPageID() {return pid;}
Page * Frame::GetPage() {return data;}