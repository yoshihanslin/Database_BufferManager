#ifndef _LRU_H
#define _LRU_H

#include "db.h"

#include "replacer.h"
#include "DLinkedList.h"

// LRU Buffer Replacement
class LRU : public Replacer {
private:
	DLinkedList frameNo; //ith frame starting from 0
public:
	LRU();
	virtual ~LRU();

	virtual int PickVictim();
	virtual void AddFrame(int f);
	virtual void RemoveFrame(int f); 
};



#endif // LRU