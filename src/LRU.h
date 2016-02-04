#ifndef __LRU__
#define __LRU__
#include <assert.h>

// create a linked list whose 
//  head is least recent used and 
//  tail is most recent used
// T: object pointer type
// ID: space id type
template<class T, class ID>
struct LRUUnit{
	T obj;
	ID id;
	LRUUnit<T,ID> *next, *pre;
	LRUUnit(ID &id) {LRUUnit::id = id; obj=0;}
	LRUUnit() {}

	LRUUnit<T,ID> *getNextLeastUsed() {
		if (next==NULL || next->next==NULL )
			return NULL;	// skip preHead or afterTail
		return next;
	}
};

template<class T, class ID>
class LRU {
	LRUUnit<T,ID> *prehead, *tailUsed, *afterTail;
	int capasity;
public:
	LRU() {
		prehead = new LRUUnit<T,ID>();
		afterTail = new LRUUnit<T,ID>();
		prehead->pre = NULL;
		prehead->next = afterTail;
		afterTail->pre = prehead;
		afterTail->next = NULL;
		tailUsed = prehead;	
		capasity = 0;
	}
	~LRU() {
		LRUUnit<T,ID>  *p, *next;
		for (p = prehead; p!=afterTail; p=next) 
		{
			next = p->next;
			delete p;
		}
		delete afterTail;
	}

	// for init
	inline void addUnit(ID id) {
		LRUUnit<T, ID> *unit = new LRUUnit<T, ID>(id);
		unit->next = afterTail;
		unit->pre = afterTail->pre;
		afterTail->pre->next = unit;
		afterTail->pre = unit;
		capasity++;

	}

	inline void removeUnit(LRUUnit<T,ID> *unit)
	{
		if (unit==tailUsed)
			tailUsed = unit->pre;
		unit->next->pre = unit->pre;
		unit->pre->next = unit->next;
		delete unit;
	}

	inline bool isFull() {return tailUsed->next==afterTail;}
	inline bool isEmpty() {return tailUsed==prehead;}

	inline LRUUnit<T,ID> *getNextUnused()  
	{
		return tailUsed->next;
	}

	inline LRUUnit<T,ID> *getLeastUsed() 
	{
		return prehead->next;
	}

	// set unit to most recent used
	inline void touch(LRUUnit<T,ID> *unit){
		if (unit!=tailUsed) {
			// unlink unit
			unit->pre->next = unit->next;
			unit->next->pre = unit->pre;
			// relink unit
			// tailUsed-><unit>->...
			unit->pre = tailUsed;
			unit->next = tailUsed->next;
			unit->pre->next = unit;
			unit->next->pre = unit;
			tailUsed = unit;
		}

	}

	inline int getCapasity() 
	{
		return capasity;
	}
};

#endif
