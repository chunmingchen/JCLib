#ifndef C11_HELPER_H
#define C11_HELPER_H

#if 0 
//#ifdef _____ //__APPLE__ // modified 12/28/12 : xcode 4.5 not need
#include <tr1/memory>
using namespace std::tr1;
#endif

#include <memory>
using namespace std;

#ifndef nullptr
#define nullptr (NULL)
#endif

// smart pointer
template <class T>
class shared_ary : public shared_ptr<T>
{
    class DeleteArray 
    {
    public:
        void operator () (T* d) const
        {
#ifdef MEM_DEBUG
        	printf("Array %p released\n", d);
#endif
            delete [] d;
        }
    };

public:
    //shared_ary( const shared_ary<T> &ary ) : shared_ptr<T>(ary) {}
    shared_ary(T *p = 0): shared_ptr<T>(p, DeleteArray()) {
#ifdef MEM_DEBUG
    	printf("Array %p allocated\n", p);
#endif
    }
    T & operator[] (int i) const{ return this->get()[i]; }
};



#endif
