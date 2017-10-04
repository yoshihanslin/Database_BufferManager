#include <sstream>
#include <iostream>

#include "DLinkedList.h"

// TODO: Fill in the implementation of the DDLinkedList class.
DLinkedList::DLinkedList() {
	_first = NULL;
	_last=NULL;
	_size = 0;
	
}

DLinkedList::DLinkedList(const DLinkedList& original) {
		_size=original._size;
	if(_size>0){
	_first=new NodeDL(original._first->getValue()); //copy first node, set first node value
	_last= _first; //copy last node
	NodeDL* traverser = original._first-> getNext();
	NodeDL *currentNode = _first;	
	while(traverser!= NULL){ //traverse down list while making new nodes and previous and next pointers	
		NodeDL *nextNode = new NodeDL(traverser->getValue());
		currentNode-> setNext(nextNode);
		nextNode -> setPrevious(currentNode);
		_last= nextNode;
		currentNode = nextNode;
		traverser = traverser ->getNext();
	}
	}else{
	_first = NULL;
	_last=NULL;
	_size = 0;
	}
}

//traverse down list while deleting node pointers
DLinkedList::~DLinkedList() {
	NodeDL *traverser=_first;
	while(traverser!= NULL){
		NodeDL *tmp=traverser->getNext();
		delete traverser;
		traverser=tmp; //last temp is null
	}
}

bool DLinkedList::insert(int value, int offset) {
	
	if( (((unsigned int) offset)>(_size)) || offset<0 ) { //offset can be added after last node
		return false;
	}
	else {NodeDL *newnode=new NodeDL(value);
   // Insert new node by traversal from the first node
		
		if(offset==0){ //if offset is first node
			if (_size==0) { //original size is 0
				_first=newnode;
				_last=newnode;
				++_size;
				return true;
			}
			else if(_size>0){ //original size is >0, add node and make prev and next pointers
				newnode->setNext(_first);
				_first->setPrevious(newnode);
				_first=newnode;
				++_size; //_last unchanged
				return true;
			}
		}
		else if ((unsigned int)offset == (_size-1)) { //if offset is the last node
			newnode -> setPrevious(_last->getPrevious());
			newnode -> setNext(_last);
			_last->getPrevious()->setNext(newnode);
			_last->setPrevious(newnode);
			++_size; //_last unchanged
			return true;
		}
		else if ((unsigned int)offset == _size) { //if offset is a new node after the last node
			_last->setNext(newnode);
			newnode->setPrevious(_last);
			_last=newnode;
			++_size;
			return true;
		}
		else{ //if offset is between first and last node
			NodeDL *traverser=_first;
			int count=offset;
			while(count>0){
				traverser=traverser->getNext();
				--count;
			}	
			newnode->setPrevious(traverser->getPrevious());
			newnode->setNext(traverser);
			traverser->getPrevious()->setNext(newnode);
			traverser->setPrevious(newnode);
			
		} //_last unchanged
		++_size;
		return true;
	}

	return true;

}

std::string DLinkedList::toString() const {
	std::ostringstream result;
	NodeDL* traverser = _first;

	while (traverser != NULL) {
		result << traverser->getValue();
		
		if (traverser->getNext() != NULL) {
			result << " ";
		}
		traverser = traverser->getNext();
	}

	return result.str();
}

DLinkedList* DLinkedList::getReverse() const {
	
	int position=_size-1; 
	DLinkedList* result=new DLinkedList(); //need to reverse and return a new list
	if (_size<=0) return result; //empty list
	if (_size==1) { //list has 1 node
		NodeDL* newnode = new NodeDL(_first->getValue());
		result->_first= newnode;
		result->_size= _size;
		result->_last= result->_first;
		return result;
	}
	//original list size >1
	result -> _size = _size;

	NodeDL* traverserBackward = _last;
	NodeDL *currentNodeDL = new NodeDL(traverserBackward->getValue());
	result->_first= currentNodeDL;
	result->_last= currentNodeDL;
	traverserBackward= traverserBackward-> getPrevious();
	
	while(traverserBackward != NULL){
	NodeDL *nextNodeDL = new NodeDL(traverserBackward->getValue());
	currentNodeDL-> setNext(nextNodeDL);
	nextNodeDL-> setPrevious(currentNodeDL);
	result-> _last = nextNodeDL;
	currentNodeDL= nextNodeDL;
	traverserBackward= traverserBackward-> getPrevious();
	}
	return result;
}


// Remove from the list every node whose value is equal to "value".
bool DLinkedList::erase(int value) {
	if(_size==NULL || _size<1){
		return false;
	}else{
		bool result=false;
		NodeDL *traverser=_first;
		while(traverser!=NULL){
//traverse down list. Deal first with corner cases of needing to erase list with 1 node, or first or last node	
			if(traverser->getValue()==value){
				result=true;
				if(_size==1){ //list with 1 node to erase
					delete _first;
					_first=NULL;_last=NULL;
					traverser=NULL;
				}else if(traverser==_first){ //erase first node in list with size >1
					NodeDL *tmp=_first;
					_first=_first->getNext();
					_first->setPrevious(NULL);
					delete tmp;
					traverser=_first;
				}else if(traverser==_last){ //erase last node in list with size >1
					NodeDL *tmp=_last;
					_last=_last->getPrevious();
					_last->setNext(NULL);
					delete tmp;
					traverser=NULL;
				}else{ //erase node in the middle in list with size >1
					NodeDL *p=traverser->getPrevious();
					NodeDL *n=traverser->getNext();
					p->setNext(n);
					n->setPrevious(p);
					delete traverser;
					traverser=n;
				}
				_size--;


			}else{ //node holds not the value to erase
				traverser=traverser->getNext();
			}

		}
	
	return result;
	}
	
}

int DLinkedList::size() const {
	return _size;
}

void DLinkedList::sort() {
	// Bubble sort. Change node values, keep node pointers same
	if(_size>1){
		bool sorted=false;
		while(!sorted){
			sorted=true;
			NodeDL *traverser=_first;
			while(traverser->getNext()!=NULL){
				int a=traverser->getValue();
				int b=traverser->getNext()->getValue();
				if(a>b){ //next node value smaller than current node
					traverser->setValue(b);
					traverser->getNext()->setValue(a);
					sorted=false;
				}
				traverser=traverser->getNext();
			}
		}
	}
}

int DLinkedList::firstValue() {
	if(_size==NULL || _size<1) return -1;
	return _first->getValue();
}

int DLinkedList::lastValue() {
	if(_size==NULL || _size<1) return -1;
	return _last->getValue();
}