#ifndef QUEUE_STAGE_H
#define QUEUE_STAGE_H

#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <functional>
#include "thread/ThreadClass.h"
#include "thread/JobQueue.h"

// profiler
#include "thread/tlog.h"
#include "thread/tlogDefs.h"

#include "thread/MinPathQueue.h"


using namespace JCLib;
using namespace std;


// for inter-queuestage data share and control
class MTControl 
{
	bool _bFinished;
	Event evtAllFinished;

protected:
    
public:
	bool keepThreadAfterFinish;

    map<long, long long> myWaitTime;
	MTControl() : _bFinished(false), keepThreadAfterFinish(false)
    {
#ifdef _PROFILE
        logLoadingData = tlog->createEventID("Loading Data", 0, 0, 255);
        logCreatingFlowfield = tlog->createEventID("CreateStaticFlowField()", 0, 255, 0);
        logGenStreamlines = tlog->createEventID("GenStreamlines()", 0, 255, 0);
#endif
    }
    ~MTControl()
    {
    }
	void setFinished() {  
		if (!keepThreadAfterFinish)
			_bFinished = true;
		evtAllFinished.set();
	}
	inline bool isFinished() { return _bFinished; }
	inline void waitFinish() {	evtAllFinished.wait(); }
	// wait or return in msec. return true if finished
	inline bool waitFinish(unsigned int msec) { return evtAllFinished.wait(msec); }
    virtual void restart() {
		_bFinished = false; 
		evtAllFinished.reset();
	}
    
};

////////////////////////////////////////////////////////////////////////////



class QueueStage
{    
    class QueueThread : public Thread
    {
        QueueStage *parent;
        int threadID;
        bool _computing; // used to track whether any job is in process
        
        // execute thread
        unsigned execute();
        
    public:
        QueueThread(int threadID_, QueueStage *parent_, const char *stageName_)
        : threadID(threadID_), parent(parent_), _computing(false) {}

        inline bool isComputing() {return _computing;}
    };
    
private:
    int logComputingID, logWaitingID;
    int threads;
	QueueThread ** threadArray;
	const char *stageName;

	// queue:
	Mutex queueMutex;
	QueueI *queue;
    

	Event evtJobPushed;

	Job *_popJob();

	// get job from previous stage
	void _pushJob( std::list<Job *> &jobList );
    

protected:
	MTControl *control;
	// pipeline:
	QueueStage *nextStage;

    // return non-null pointer of job list for next stage (skipping computing part)
	virtual void i_critical_gotJob(QueueI &queue,  std::list<Job *> &jobList, /*output*/ std::list<Job *> &jobList_nextStage)
	{
        queue.push(jobList);
	}

	virtual void i_critical_jobPopped(Job *job)
	{}

	virtual void i_computeJob(Job *job, std::list<Job *> &jobOutList) 
		= 0;

    //< Added 2011-03-26
    inline void pushNextStage( std::list<Job *> &jobOutList )
    {
        // push jobs to next stage
        assert(nextStage);
        nextStage->_pushJob(jobOutList);
    }
    //> Added 2011-03-26
public:
    //enum EnumQueueType {FIFO_QUEUE=0, PRIORITY_QUEUE=1, FORWARDER=2, NEAREST=3} queueType;

	QueueStage(QueueI *queue_, int threads_, MTControl *control_, QueueStage *nextStage_, const char *stageName_="" ) 
		: queue(queue_), threads(threads_), control(control_), nextStage(nextStage_), threadArray(0), stageName(stageName_)
	{
		// init threads
		int i;
		threadArray = new QueueThread *[threads];
		for (i=0; i<threads; i++)
			threadArray[i] = new QueueThread(i, this, stageName);
        
        printf("Stage %s: %s queue\n", stageName_, queue->getName());

#ifdef _PROFILE
        char s[1024];
        sprintf(s, "%s_Running", stageName);
        logComputingID = tlog->createEventID(s, 0, 128, 0);
        sprintf(s, "%s_Waiting", stageName);
        logWaitingID = tlog->createEventID(s, 128, 0, 0);
#endif
	}

	void initJobs(std::list<Job *> &jobList)
	{
		_pushJob( jobList );
	}

	virtual ~QueueStage() {
		int i;
		for (i=0; i<threads; i++)
			delete threadArray[i];
		delete[] threadArray;
        delete queue;
	}

	inline void setNextStage(QueueStage *nextStage_) {nextStage = nextStage_;}

	// start or continue stopped threads
	void startThreads();

	//inline const list<Job *> &getQueue() {return *queue;}

    inline int getQueueSize() {
        //queueMutex.lock();
        int qs = queue->size();
        //queueMutex.unlock();
        return qs;
    }

    inline QueueStage *getNextStage() {return nextStage;}

    /* Termination detection
     * Checks whether any job still in the queue or in the threads
     * Added 12/2/2012
     */
    inline bool hasAnyJob() {
    	// check whether any job in the queue
    	if (queue->size() > 0)
    		return true;

    	// check whether any job in the threads => isComputing
    	int i;
		for (i=0; i<threads; i++)
			if (threadArray[i]->isComputing())
				return true;
		return false;
    }

    // should not use this normally
    // use control->setFinished()
    inline void stopAllThreads(bool wait) {
    	int i;
    	for (i=0; i<threads; i++)
    		threadArray[i]->stopThread(wait);
    }
};

///////////////////////
#endif

