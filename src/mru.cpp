#include "mru.h"

MRU::MRU() {}
MRU::~MRU() {}

int MRU::PickVictim() {
	if (frameNo.size() <=0) return -1;
	int temp= frameNo.firstValue();
	frameNo.erase(temp);
	return temp;

}
void MRU::AddFrame(int f) {
	frameNo.insert(f,0);
}
void MRU::RemoveFrame(int f) {
	frameNo.erase(f);
}