#ifndef THREAD_CLASS_WIN_H
#define THREAD_CLASS_WIN_H

#include <assert.h>
#include <process.h>
#include <windows.h>
#include <WinBase.h>


#ifdef ___DEBUG
#define show(s)   printf(s "\n")
#define show(s,x) printf(s "\n",x)
#define show_error(s,x) fprintf(stderr, "Error: " s "\n", x)
#else
#define show(s)
#define show(s,x)
#define show_error(s,x)
#endif

namespace JCLib {
    
    typedef struct 
    {
        HANDLE h;
        DWORD id;
    }_sthread_t;
    
    class Thread{
    private:
        _sthread_t *thread;
        bool bTerminated;
        bool bThreadRunning;
        bool locked;
        
    protected:
        inline bool isTerminated()			{ return bTerminated; }
    public:
        bool bDestroyWhenStop;
        
        inline bool isThreadRunning()		{ return bThreadRunning; }
        // lock: set deletable
        void lock()					{ locked = true; show("** [Thread::lock] this=%p",this);}
        void unlock()				{ locked = false; show("** [Thread::unlock] this=%p",this);}
        bool isLocked()				{ return locked; }
        
        Thread() { 
            show("** [Thread::Thread] this=%p",this);
            bThreadRunning = false; 
            bDestroyWhenStop = false;
            locked = false;
            thread = NULL;
            /*threadHandle = NULL;*/ 
        }
        
        int startThread(){
            show("** [Thread::startThread] this:%p",this);
            if (isThreadRunning()){
                show_error("cannot start thread: thread running",0);
                return -1;
            }
            if (thread == NULL)
                thread = new _sthread_t;
            thread->h = (HANDLE) CreateThread (NULL, /* default security attr */ 
                                               0, /* use default*/ 
                                               _execute, 
                                               (void *)this, 0,
                                               &(thread->id));
            if (thread->h == 0)	return -1;	// fail
            return 0;
        }
        
        //static DWORD WINAPI execute( LPVOID pOp);	// the thread
        static DWORD WINAPI _execute(LPVOID lpv_this)
        {
            int r;
            Thread *_this = (Thread *)lpv_this;
            show("** [Thread::_execute] _this=%p", _this);
            _this->bThreadRunning = true;
            _this->bTerminated = false;
            r = _this->execute();
            _this->bThreadRunning = false;
            CloseHandle( _this->thread->h );
            if (_this->bDestroyWhenStop){
                _this->destroy();
                delete _this;
            }
            _endthreadex( 0 );
            return r;
        }
        
        virtual unsigned execute() =0;
        
        virtual void destroy() 
        {}
        
        // set stop signal
        inline virtual /*NOT varified*/ void stopThread()
        {
            show("** [Thread::stopThread] this=%p\n",this); /// debug
            bTerminated = true; 
        }
        
        virtual ~Thread() {
            show("** [Thread::~Thread] this=%p",this);
            stopThread(); 
            if (thread!=NULL)
                WaitForSingleObject( thread->h, INFINITE );
            while (locked){
                show_error("cannot destroy: waiting for unlock...",0);
                Sleep(10);
            }
            /*CloseHandle(threadHandle);*/
        }
        
        // priolity
        inline bool set_priority(int nPriolity)
        {
            return (SetThreadPriority(thread->h, nPriolity)==TRUE);
        }
        /* parameters:
         THREAD_PRIORITY_ABOVE_NORMAL 	Priority 1 point above the priority class.
         THREAD_PRIORITY_BELOW_NORMAL 	Priority 1 point below the priority class.
         THREAD_PRIORITY_HIGHEST 	Priority 2 points above the priority class.
         THREAD_PRIORITY_IDLE 	Base priority of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 16 for REALTIME_PRIORITY_CLASS processes.
         THREAD_PRIORITY_LOWEST 	Priority 2 points below the priority class.
         THREAD_PRIORITY_NORMAL 	Normal priority for the priority class.
         THREAD_PRIORITY_TIME_CRITICAL 	Base priority of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 31 for REALTIME_PRIORITY_CLASS processes.
         */
    };
    
    class Event
    {
        HANDLE hEvent;
    public:
        Event(bool bInitState = false, bool manualReset = false ) {
            hEvent = CreateEvent(NULL, manualReset , bInitState, 0);
            assert(hEvent);
        }
        
        inline DWORD wait(unsigned int msecs = INFINITE) {
            return WaitForSingleObject(hEvent, msecs);
        }
        
        inline bool set() {
            return SetEvent(hEvent);
        }
        
        inline bool reset() {
            return ResetEvent(hEvent);
        }
        
        ~Event(){
            CloseHandle(hEvent);
        }
    };
    
    class Mutex
    {    
        friend class Lock;
        
        int count;
        CRITICAL_SECTION _critSection;
        void Acquire () { EnterCriticalSection (& _critSection); }
        void Release () { LeaveCriticalSection (& _critSection); }
    public:
        Mutex() : count(0) { InitializeCriticalSection(& _critSection); }
        ~Mutex() { DeleteCriticalSection(& _critSection); }
        
        inline void lock() {
            //if (count++ == 0)
			Acquire();
        }
        inline void unlock() {
            //if (--count == 0)
			Release();
        }
    };
    
    
    class AtomicLong
    {
        volatile long x;
    public:
        AtomicLong() {x=0;}
        AtomicLong(long x_) {x=x_;}
        inline void increment() {InterlockedIncrement(&x);}
        inline void decrement() {InterlockedDecrement(&x);}
        inline long getValue() {return x;}
    };
    
    
}; // namespace


#endif