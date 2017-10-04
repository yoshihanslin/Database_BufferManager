#ifndef DLINKEDLIST_H_
#define DLINKEDLIST_H_
#include <string>

using namespace std;

// TODO: Define any auxiliary data structures you need to define a doubly
// linked list
class NodeDL {
public:
	NodeDL(int value) {
		_value = value;
		_next = NULL;
		_previous=NULL;
	}
	
	int getValue() const {
		return _value;
	}
	void setValue(int value) {
		_value = value;
	}
	
	NodeDL* getNext() const {
		return _next;
	}
	void setNext(NodeDL* next) {
		_next = next;
	}
	NodeDL* getPrevious() const{
		return _previous;
	}
	void setPrevious(NodeDL* previous){
		_previous=previous;
	}
	
private:
	int _value;
	NodeDL *_next;
	NodeDL *_previous;
};
// Implementation of a doubly linked list.
class DLinkedList {
public:
	// Create a new empty linked list
	DLinkedList();
	
	// Create a new linked list whose elements are copies of those in the
	// list "original". Insertions or deletions to the original list should
	// not affect the copied list, and insertions or deletions to the copied
	// list should not affect the original list.
	DLinkedList(const DLinkedList& original);

	// List destructor. Free the memory that's used by nodes in the list.
	~DLinkedList();

	// Generate a string containing human-readable versions ofthe elements of
	// the list, with a space between adjacent elements in the list.
	// For instance, if the list contains the elements [1, 2, 3] then the
	// output of this function should be "1 2 3" (without the quotes).
	string toString() const;

	// Create a new list whose elements are the same as those in the original
	// list, but which appear in the opposite order. For instance, if the
	// original list l contains the elements [1, 2, 3] then l.getReverse()
	// will return a new list which contains the elements [3, 2, 1].
	DLinkedList* getReverse() const;

	// Insert a new node into the list. That node should have the value "value"
	// and should be located at position "offset". For instance, if offset = 0
	// then the node should be inserted into the list at the very beginning
	// of the list. If offset = 1 then the node should be inserted just after
	// what was previously the first element of the list. And so on.
	//
	// This function should return true if the insertion was successful, or
	// false otherwise. (This can happen if "offset" is greater than the
	// length of the list.)
	bool insert(int value, int offset);

	// Remove from the list every node whose value is equal to "value". If no
	// such nodes exist, the contents of the list will not be changed by this
	// function.
	bool erase(int value);

	// Get the number of elements in the list.
	int size() const;

	// Sort the elements in the list in ascending order
	void sort();

	int firstValue(); //get first value from start of list
	int lastValue(); //get last value of list

private:
	NodeDL *_last;
	NodeDL *_first;
	unsigned int _size;
};
#endif // DLINKEDLIST_H