#ifndef THREAD_CLASS_UNIX_H
#define THREAD_CLASS_UNIX_H

#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <unistd.h>

#ifndef show
#ifdef __DEBUG
//#define show(s)   printf(s "\n")
#define show(s,x) printf(s "\n",x)
#define show_error(s,x) fprintf(stderr, "Error: " s "\n", x)
#else
//#define show(s)
#define show(s,x)
#define show_error(s,x)
#endif
#endif


namespace JCLib {
    
    class Thread{
    private:
        pthread_t thread;
        bool bTerminated;
        bool bThreadRunning;
        bool locked;
        
    protected:
        inline bool isTerminated()			{ return bTerminated; }
    public:
        bool bDestroyWhenStop;
        
        inline bool isThreadRunning()		{ return bThreadRunning; }

        // prevent thread terminated before being deleted
        inline void setLock(bool l) {locked = l;} 
        inline bool isLocked() {return locked;}
        
        Thread() { 
            show("** [Thread::Thread] this=%p",this);
            bThreadRunning = false; 
            bDestroyWhenStop = false;
            thread = 0;
            locked = true;
            /*threadHandle = NULL;*/ 
        }
        
        int startThread(){
            show("** [Thread::startThread] this:%p",this);
            if (isThreadRunning()){
                show_error("cannot start thread: thread running",0);
                return -1;
            }
            return pthread_create(&thread, NULL, _execute, (void *)this);
            // return 0: success
        }
        
        //static DWORD WINAPI execute( LPVOID pOp);	// the thread
        static void * _execute(void *lpv_this)
        {
            static int r;
            Thread *_this = (Thread *)lpv_this;
            show("** [Thread::_execute] _this=%p", _this);
            _this->bThreadRunning = true;
            _this->bTerminated = false;
            // run
            r = _this->execute();
            _this->bThreadRunning = false;
            //? _this->thread = NULL;

            if (_this->bDestroyWhenStop){
                delete _this;
            }
            return &r;
        }
        
        virtual unsigned execute() =0;
        
        // set stop signal
        inline virtual void stopThread(bool waitThread=false)
        {
            show("** [Thread::stopThread] this=%p\n",this); /// debug
            bTerminated = true; 
            if (waitThread && bThreadRunning) 
                pthread_join(thread, NULL);            
        }
        
        virtual ~Thread() {
            show("** [Thread::~Thread] this=%p",this);
            stopThread(true); 
            // we can't force thread termination outside the thread...
        }
        
        // priolity 
        // not implemented
#define THREAD_PRIORITY_HIGHEST 0
        inline bool set_priority(int nPriolity)
        {
            //return (SetThreadPriority(thread->h, nPriolity)==TRUE);
            return false;
        }
    };
    //////////////////////////////////////////////////////////
    
    //http://stackoverflow.com/questions/2401168/pthread-windows-event-equivalent-question
    class Event
    {    
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
        bool _signalled;
        bool _auto;
    public:
        Event( bool bInitState = false, bool manualReset = false ) :
            _auto( !manualReset ),
            _signalled( bInitState ) 
        {
            pthread_mutex_init( &_mutex, NULL );
            pthread_cond_init( &_cond, NULL );
        }
        ~Event() {
            pthread_cond_destroy( &_cond );
            pthread_mutex_destroy( &_mutex );
        }
        
        inline bool wait() {
            pthread_mutex_lock( &_mutex );
            
            while ( _signalled == false )
                pthread_cond_wait(&_cond, &_mutex);
                
                // if we're an autoreset event, auto reset
                if ( _auto )
                    _signalled = false;
            
            pthread_mutex_unlock( &_mutex );
            return true;
        }

        // returns false if timeout
        inline bool wait(unsigned int msecs) {
            int r=0;
            pthread_mutex_lock( &_mutex );
            
            while ( _signalled == false && r!=ETIMEDOUT ) {
                struct timespec   ts;
                struct timeval    tp;
                gettimeofday(&tp, NULL);
                /* Convert from timeval to timespec */
                ts.tv_sec  = tp.tv_sec;
                long usecs = tp.tv_usec + msecs*1000;
                ts.tv_sec+=usecs/(1000000);
                ts.tv_nsec=(usecs%(1000000))*1000;
                
                r = pthread_cond_timedwait(&_cond, &_mutex, &ts);
                
                
            }
            
            // if we're an autoreset event, auto reset
            if ( _auto && r!=ETIMEDOUT)
                _signalled = false;
            
            pthread_mutex_unlock( &_mutex );
            return r!=ETIMEDOUT;
        }
        
        inline bool set() {
            pthread_mutex_lock( &_mutex );
            
            // only set and signal if we are unset
            if ( _signalled == false )
            {
                _signalled = true;
                
                pthread_cond_signal( &_cond );
            }
            
            pthread_mutex_unlock( &_mutex );
            return true;
        }
        
        inline bool reset() {
            pthread_mutex_lock( &_mutex );
            
            _signalled = false;
            
            pthread_mutex_unlock( &_mutex );
            return true;
        }
        
    };
    
    class Mutex
    {    
        pthread_mutex_t _mutex;
    public:
        Mutex() { pthread_mutex_init(&_mutex, NULL); }
        ~Mutex() { pthread_mutex_destroy(&_mutex); }
        
        inline void lock() {
			pthread_mutex_lock(&_mutex);
        }
        inline void unlock() {
			pthread_mutex_unlock(&_mutex);
        }
    };
    
//#ifdef __sync_add_and_fetch
#if  __GNUC__ >= 4

    class AtomicLong
    {
        volatile long x;
    public:
        AtomicLong() {x=0;}
        AtomicLong(long x_) {x=x_;}
        inline long increment() {return __sync_add_and_fetch(&x,1L);}
        inline long decrement() {return __sync_sub_and_fetch(&x,1L);}
        inline long increment(long v) { return __sync_add_and_fetch(&x,v);}
        inline long decrement(long v) { return __sync_sub_and_fetch(&x,v);}
        inline long getValue() {return x;}
        inline long operator+=(long v) { return increment(v); }
        inline long operator-=(long v) { return decrement(v); }
    };
#else
#warning "Your gcc does not support atomic operation.  mutex is used"
    class AtomicLong
    {
        long x;
	Mutex mutex;
        inline long plus(long v) {long y; mutex.lock(); y=x=x+v; mutex.unlock(); return y;}
    public:
        AtomicLong() {x=0;}
        AtomicLong(long x_) {x=x_;}
        inline long increment() { return plus(1);}
        inline long decrement() { return plus(-1);}
        inline long increment(long v) { return plus(v);}
        inline long decrement(long v) { return plus(-v);}
        inline long getValue() {return x;}
    };
#endif    
    inline void Sleep(long long us)
    {
    	struct timespec ts;
		ts.tv_sec = us/1000;
		ts.tv_nsec= (us%1000)*1000;
		nanosleep(&ts, NULL);
    }
}; // namespace

#if 0
#define Sleep(us) { \
		struct timespec ts;     \
		ts.tv_sec = us/1000;    \
		ts.tv_nsec= (us%1000)*1000; \
		nanosleep(&ts, NULL);   \
}
#endif


#endif
