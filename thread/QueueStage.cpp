#include <map>
#include "thread/QueueStage.h"


using namespace std;

unsigned QueueStage::QueueThread::execute()
{
#ifdef _PROFILE
    char s[1024];
    if(tlog) {
    	sprintf(s, "%s%d", parent->stageName, threadID);
    	tlog->regThread(s);
    }
#endif
 
    list<Job *> jobOutList;
    Job *job;
    /*
     * popJob: keeps pooling until terminated => returns false
     */
    printf("Thread %s #%d started\n", parent->stageName, threadID);
    while( (job = parent->_popJob())!=NULL && !this->isTerminated())
    {	
        _computing = true;
        jobOutList.clear();
        
#ifdef _PROFILE
        if(tlog) {
        	sprintf(s, "Qs:%d", parent->getQueueSize());
            tlog->startEvent(parent->logComputingID, s);
        }
#endif
        // main computation
        parent->i_computeJob(job, jobOutList);
        
#ifdef _PROFILE
        if(tlog) tlog->endEvent(parent->logComputingID);
#endif
        
        // push jobs to next stage
        if (parent->nextStage && !jobOutList.empty())
            parent->nextStage->_pushJob(jobOutList);
        _computing = false;
    }
    
    printf("Thread %s #%d finished\n", parent->stageName, threadID);
#ifdef _PROFILE
    if(tlog) tlog->logMessage("Thread finished");
#endif
    return 0;
}

//////////////////////////////////////////////
Job *QueueStage::_popJob()
{
    if (control->isFinished())
        return NULL;
    
    queueMutex.lock();

    Timer t;
    t.start();

    while (queue->empty() ) {
        queueMutex.unlock();
        
#ifdef _PROFILE
        if(tlog) tlog->startEvent(logWaitingID);
#endif
        
#ifdef _DEBUG
        printf("[Stage %s: Waiting for job...]\n", stageName);
#endif
        bool r = evtJobPushed.wait(10000);

        if (!r) {
        	printf("[Stage %s: Waiting for job...]\n", stageName);
        }

        if (control->isFinished() ) return NULL;
        
#ifdef _PROFILE
        if(tlog) tlog->endEvent(logWaitingID);
#endif
        
        queueMutex.lock();
    }
    t.end();
    control->myWaitTime[(long)pthread_self()]+=t.getElapsedUS();
    
    // return new seed group
    Job *job = queue->pop();
    i_critical_jobPopped(job);
    
    queueMutex.unlock();
    return job;
}

//////////////////////////////////


// get job from previous stage
void QueueStage::_pushJob( std::list<Job *> &jobList )
{
    if (!jobList.empty()) {
        list<Job *> outJobList_nextStage; // joblist for next stage (skip computation in this stage)
        queueMutex.lock();
        {
            i_critical_gotJob(*queue, jobList, outJobList_nextStage);
        }
        queueMutex.unlock();
        
        evtJobPushed.set();
        
        if (!outJobList_nextStage.empty()) 
            nextStage->_pushJob( outJobList_nextStage );
    }
}


void QueueStage::startThreads()
{
    int i;
    for (i=0; i<threads; i++)
        if (!threadArray[i]->isThreadRunning()){
            threadArray[i]->startThread();
            threadArray[i]->set_priority(THREAD_PRIORITY_HIGHEST);
        }
}
    
