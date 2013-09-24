/*
 *  QueueImplements.h
 *  parallelStreamline
 *
 *  Created by MiGi on 6/13/11.
 *  Copyright 2011 OSC. All rights reserved.
 *
 */

#ifndef QUEUE_IMPLEMENTS_H
#define QUEUE_IMPLEMENTS_H
#include <limits.h>

class FifoQueue : public QueueI
{
protected:
    std::list<Job *> q;
    size_t length; // list<>::size() is not a good implementation
public:
    FifoQueue(): length(0) {}
    virtual bool empty() { return q.empty(); }
    virtual size_t size() {return length; }
    //virtual Job *top() { return q.front(); }
    virtual void push(Job *obj) { q.push_back(obj); length++; }
    virtual Job *pop() { Job *job = (Job *)q.front(); q.pop_front(); length--; return job;}
    virtual void push(std::list<Job *> &q1) { length+=q1.size(); q.splice(q.end(), q1);  }
    virtual const char * getName() {return "FIFO";}
};

// smaller order, higher priority
struct FileOrderComparer {
    bool operator()(Job *job1, Job *job2) { return job1->forder > job2->forder; }
};
class PriorityQueue : public QueueI
{
protected:
    std::priority_queue<Job *, std::vector<Job *>, FileOrderComparer > q;
public:
    virtual bool empty() { return q.empty(); }
    virtual size_t size() {return q.size(); }
    virtual Job *top() { return (Job *)q.top(); }
    virtual void push(Job *obj) {q.push(obj); }
    virtual Job *pop() {Job *job = (Job *)q.top(); q.pop(); return job; }
    virtual void push(std::list<Job *> &q1) {
        while(!q1.empty()) {
            q.push(q1.front());
            q1.pop_front();
        }
    }
    virtual const char * getName() { return "Priority"; }
};

class SimpleTimePriorityQueue : public PriorityQueue
{
	int blocksPerTime, npart;
public:
    SimpleTimePriorityQueue(int npart_, int blocksPerTime_): npart(npart_), blocksPerTime(blocksPerTime_) {}
    virtual void push(Job *obj) {
    	obj->forder= (obj->partID / blocksPerTime) ;
    	q.push(obj);
    }
    virtual const char * getName() { return "SimpleTimePriority"; }
};

class TimePriorityQueue : public PriorityQueue
{
	int blocksPerTime, npart;
public:
    TimePriorityQueue(int npart_, int blocksPerTime_): npart(npart_), blocksPerTime(blocksPerTime_) {}
    virtual void push(Job *obj) {
    	obj->forder+=(obj->partID / blocksPerTime) * npart;
    	q.push(obj);
    }
    virtual const char * getName() { return "TimePriority"; }
};

class TimeInvPriorityQueue : public PriorityQueue
{
	int blocksPerTime, npart;
public:
    TimeInvPriorityQueue(int npart_, int blocksPerTime_): npart(npart_), blocksPerTime(blocksPerTime_) {}
    virtual void push(Job *obj) {
    	obj->forder=(npart -1 - obj->forder) + (obj->partID / blocksPerTime) * npart;
    	q.push(obj);
    }
    virtual const char * getName() { return "TimeInvPriority"; }
};


class Forwarder : public QueueI {
    typedef std::map<int, Job *> JobMapType;
    JobMapType jobMap;
    int pos;
    const int look_behind;
    
    inline JobMapType::iterator next() { 
        JobMapType::iterator itr = jobMap.begin();
        
        for ( ; itr!=jobMap.end(); ++itr)
        {
            if (itr->first >= pos-look_behind) {
                if (itr->first > pos) pos = itr->first;
                return itr;
            }
        }
        // cycle back
        pos = jobMap.begin()->first;
        return jobMap.begin();
    }
    
public:
    Forwarder(int look_behind_=0): look_behind(look_behind_), pos(0) {}
    virtual bool empty() { return jobMap.empty(); }
    virtual size_t size() { return jobMap.size(); }
    virtual Job *top() {
        if (!jobMap.empty()) {
            JobMapType::iterator itr_pos = next();
#ifdef _DEBUG
            for (JobMapType::iterator tmp = jobMap.begin(); tmp!=jobMap.end(); ++tmp)
                printf("%d ", tmp->first);
            printf("->%d (pos=%d)\n", itr_pos->first, pos);
#endif
            
            return itr_pos->second;
        }
        return NULL;
    }
    virtual void push(Job *job) {
        assert( jobMap.find( job->forder ) == jobMap.end() );/*no duplicated job*/
        jobMap[job->forder] = job;
#ifdef _DEBUG
        printf("Push job %d\n", job->forder);
#endif
    }
    virtual Job *pop() {
        Job *job = top();
        if (!jobMap.empty()) {
            JobMapType::iterator itr;
            if ( (itr=jobMap.find( pos )) != jobMap.end()) { 
                jobMap.erase(itr);
            } else {
                // should not go here
                jobMap.erase(next()); 
            }
            //printf("itr_pos=%d\n", itr_pos->first);
        }
        return job;
    }
    virtual void push(std::list<Job *> &q1) {
        while(!q1.empty()) {
            push(q1.front());
            q1.pop_front();
        }   
    }
    virtual const char * getName() { return "Forwarder"; }
};

class Nearest : public QueueI {
    typedef std::map<int, Job *> JobMapType;
    JobMapType jobMap;
    int pos;
    JobMapType::iterator itr_pos;
    int R_PENALTY; // reverse penalty
    int RESTART_COST;
    
    inline JobMapType::iterator next() { 
        JobMapType::iterator itr_l ,itr_r;
        int rcost, lcost;
        itr_l = itr_r = (itr_pos!=jobMap.end()) ? itr_pos : jobMap.begin();
        for (  ; itr_r!=jobMap.end(); ++itr_r)
        {
            if (itr_r->first > pos) 
                break;
            itr_l = itr_r;
        }
        rcost = (itr_r==jobMap.end())?9999999: itr_r->first - pos;
        lcost = (itr_l==jobMap.end())?9999999: R_PENALTY * abs(pos - itr_l->first) ;
        //printf("lcost=%d, rlost=%d\n", lcost, rcost);
        if (lcost<rcost)
            if (lcost > RESTART_COST)
                return jobMap.begin();
            else
                return itr_l;
            else
                if (rcost > RESTART_COST)
                    return jobMap.begin();
                else
                    return itr_r;
    }
    
public:
    Nearest(): R_PENALTY(4), RESTART_COST(128) { pos = 0; itr_pos = jobMap.end();}
    bool empty() { return jobMap.empty(); }
    size_t size() { return jobMap.size(); }
    Job *top() {
        if (!jobMap.empty()) {
            itr_pos = next();
            pos = itr_pos->first;
#ifdef _DEBUG
            for (JobMapType::iterator tmp = jobMap.begin(); tmp!=jobMap.end(); ++tmp)
                printf("%d ", tmp->first);
            printf("->%d \n", pos);
#endif
            
            return itr_pos->second;
        }
        return NULL;
    }
    void push(Job *job) {
        assert( jobMap.find( job->forder ) == jobMap.end() );
        jobMap[job->forder] = job;
#ifdef _DEBUG
        printf("Push job %d\n", job->forder);
#endif
    }
    Job * pop() {
        Job *job = top();
        if (!jobMap.empty()) {
            if (itr_pos!=jobMap.end()) { 
                if (itr_pos!=jobMap.begin())
                    jobMap.erase(itr_pos--);
                else {
                    jobMap.erase(itr_pos);
                    itr_pos = jobMap.end();
                }
                
            } else {
                // should not go here
                jobMap.erase(next()); 
            }
            //printf("itr_pos=%d\n", itr_pos->first);
        }
        return job;
    }
    void push(std::list<Job *> &q1) {
        while(!q1.empty()) {
            push(q1.front());
            q1.pop_front();
        }   
    }
    
    const char * getName() { return "Nearest"; }
};




#define CACHE_DEPTH 10
class PrefetchQueue : public FifoQueue
{
    //std::list<Job *> q;
    int mru[CACHE_DEPTH]; // most recently used
    int mru_replace_idx;
    std::list<Job *>::iterator top_it;
    bool pop_ready;
public:
    PrefetchQueue() {mru_replace_idx = 0; mru[0]=mru[1]=0; pop_ready = false;}
    virtual bool empty() { return q.empty(); }
    virtual size_t size() {return q.size(); }
    virtual Job *top() 
    {
        if (pop_ready) return *top_it;
        
        int min_dist=INT_MAX;
        top_it = q.end();
        for (std::list<Job *>::iterator it = q.begin(); it != q.end(); ++it)
        {
            for (int i=0; i<CACHE_DEPTH; i++)
            {
                int dist = abs((*it)->forder - mru[i]) ;
                if (dist < min_dist || top_it == q.end() ) {
                    top_it = it;
                    min_dist = dist;
                }
            }   
        }        
        pop_ready = true;
        
        return *top_it;
    }
    virtual void push(Job *obj) {
#ifdef _DEBUG
        printf("queue:");
        for (std::list<Job *>::iterator it = q.begin(); it != q.end(); ++it)
        {
            printf("%d ", (*it)->forder);
            assert ((*it)->forder != obj->forder);
        }
        printf("\n");
#endif
        return q.push_back(obj); 
    }
    virtual Job * pop() {
        Job *job = top();
        if (q.size()) {
            // refresh mru
            mru[mru_replace_idx] = (*top_it)->forder;
            mru_replace_idx = (mru_replace_idx +1 ) % CACHE_DEPTH;
            
            top();
            q.erase( top_it );
            pop_ready = false;
            
#ifdef _DEBUG
            printf("Pop: %d (forder=%d) mru:", (*top_it)->partID, (*top_it)->forder);
            for (int i=0; i<CACHE_DEPTH; i++)
                printf("%d ", mru[i]);
            printf("\n");
#endif
            
        }
        return job;
    }
    virtual void push(std::list<Job *> &q1) { q.splice(q.end(), q1); }
    virtual const char * getName() {return "Prefetch";}
    
    
};




#endif
