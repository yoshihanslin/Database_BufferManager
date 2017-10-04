#include <sstream>
#include <iostream>
#include <list>
#include "lru.h"

LRU::LRU() {}
LRU::~LRU() {}

int LRU::PickVictim() {
	if (frameNo.size() <=0) return -1;
	int temp= frameNo.lastValue();
	frameNo.erase(temp);
	return temp;
}

void LRU::AddFrame(int f) {
	frameNo.insert(f,0);
}
void LRU::RemoveFrame(int f) {
	frameNo.erase(f);
}

