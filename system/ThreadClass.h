#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#ifdef WIN32
#include "ThreadClass_win.h"
#else //unix
#include "ThreadClass_unix.h"
#endif

namespace JCLib {

// reader/writer: non-block
template <class T>
class ReaderWriter
{
    int readers;
    Mutex mutex;
    bool writing;
    T *obj;
    bool dataReady;
public:
    ReaderWriter(T *obj_) : readers(0), writing(false), dataReady(false), obj(obj_) {
    }
    virtual ~ReaderWriter() {delete obj;}
    inline void increaseReader() {
        assert(!writing && dataReady); mutex.lock(); readers++; mutex.unlock();}
    inline void decreaseReader() {assert(!writing && dataReady); mutex.lock(); readers--; mutex.unlock();}
    inline const T *getObj() {assert(!writing && readers>0 && dataReady); return obj;}
    // test if data not being read and lock.  Return true if ready to write 
    bool attemptWrite(T **obj_) {
        assert(!writing);
        mutex.lock(); 
        if (readers > 0) {
            mutex.unlock();
            return false;
        }
        writing = true;
        *obj_ = obj;
        return true;
    }
    inline void releaseWrite() {writing = false; mutex.unlock();}
    inline int getReaders() {return readers;}
    
    inline bool isDataReady() {return dataReady;}
    inline bool isDataReady_increaseReader() {
        mutex.lock(); 
        if (!dataReady) {
            mutex.unlock();
            return false;
        }; 
        readers++; 
        mutex.unlock();
        return true;
    }
    inline void setDataReady(bool r) {dataReady = r;}
};

};//namespace

#endif