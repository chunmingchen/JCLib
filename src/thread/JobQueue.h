/*
 *  JobQueue.h
 *  parallelStreamline
 *
 *  Created by MiGi on 3/1/11.
 *  Copyright 2011 OSC. All rights reserved.
 *
 */


#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <vector>
#include <queue>

struct Job {	
	int partID;	// partition id
    int forder; // file order
    
    Job () {forder = 0; }
    virtual ~Job() {}  
};


class QueueI{
public:
    virtual ~QueueI() {}
    virtual bool empty() =0;
    virtual size_t size() =0;
    //virtual Job *top() = 0;
    virtual void push(Job *) = 0;
    virtual void push(std::list<Job *> &) = 0;
    virtual Job *pop() = 0;
    virtual const char * getName() = 0;
};

#endif //JOB_QUEUE_H


