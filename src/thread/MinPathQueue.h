/*
 *  MinPathQueue.h
 *  parallelStreamline
 *
 *  Created by MiGi on 6/5/11.
 *  Copyright 2011 OSC. All rights reserved.
 *
 */
#ifndef MIN_PATH_QUEUE_H
#define MIN_PATH_QUEUE_H
#include <vector>
#include <list>
#include <algorithm>
#include "GraphLoader.h"
#include "QueueStage.h"


struct JobWeight{
    int id;
    float weight;
    Job *pJob;
    bool operator<(const JobWeight &e) const {return weight<e.weight;}
    bool operator>(const JobWeight &e) const {return weight>e.weight;}
    JobWeight(int id_, float w_, Job *pjob_) : id(id_), weight(w_), pJob(pjob_) {}
};

class MinPathQueue : public QueueI
{
    GraphLoader *graph;
    
    std::vector<JobWeight> queue;
    char *in_use;
    int **matrix;
public:
    ~MinPathQueue() {
        delete []in_use;
        for (size_t i=0; i<graph->npart; i++)
            delete[] matrix[i];
        delete[] matrix;
        delete graph;
    }
    
    MinPathQueue(const char *graph_filename, int K=-1)
    {
        int i,j,k;
        graph = new GraphLoader(graph_filename);
        int n=graph->npart;
        if (K<0) K=n;
        
        printf("Preparing MinPathQueue...");
        in_use = new char[n];
        for (i=0; i<n; i++)
            in_use[i] = 0;
        
        // todo: can load data
        
        
        matrix = new int*[n];
        int big_num=n*n;
        for (i=0; i<n; i++)
        {
            matrix[i] = new int[n];
            // init
            for (j=0; j<n; j++) 
                matrix[i][j] = big_num;
            for (j=0; j<graph->edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = graph->edgeTableAry[i][j];
                matrix[i][ew.neighborID] = 1;
            }
            matrix[i][i] = 0;
        }

        // all pair shortest path
        for (k=0; k<K; k++) 
        {
            printf("k=%d\n", k);
            for (i=0; i<n; i++)
                for (j=0; j<n; j++)
                    matrix[i][j] = std::min( matrix[i][j], matrix[i][k]+matrix[k][j] );
        }
     
        printf("Done\n");
#ifdef _DEBUG
        // show
        for (i=0; i<n; i++)
        {
            printf("%4d ", i);
            for (j=0; j<n; j++)
                printf("%3d", matrix[i][j]);
            printf("\n");
        }
#endif
     
    }
    
    virtual bool empty() 
    {
        return queue.size()==0;
    }
    virtual size_t size() 
    {
        return queue.size();
    }
    virtual Job *top() 
    {
#ifdef _DEBUG
        printf("Output job: %d (w=%f)\n", queue.back().id, queue.back().weight);
#endif
        return queue.back().pJob;
    }
    
    virtual void push(Job *pJob) 
    {
        int id = pJob->partID;
        int npart = graph->npart;
        
        if (in_use[id]) {
            printf("ID already in queue\n");
            return;
        }
        
        int i;
        float w=0;
        for (i=0; i<npart; i++)
        {
            if (in_use[i]) {
                w += matrix[id][i];
            }
        }
        // update queue element
        for (i=0; i<queue.size(); i++)
        {
            JobWeight &jw = queue[i];
            jw.weight += matrix[jw.id][id];
        }
        
        queue.push_back(JobWeight(id,w, pJob));
        in_use[id] = 1;   
        sort(queue.begin(), queue.end(), std::greater<JobWeight>());
        
#ifdef _DEBUG
        // show
        for (i=0; i<queue.size(); i++)
        {
            JobWeight &jw = queue[i];
            printf("%d:%g  ", jw.id, jw.weight);
        }
        printf("\n");
#endif
    }
    virtual void push(std::list<Job *> &jobs) 
    {
        for (std::list<Job *>::iterator it = jobs.begin(); it!=jobs.end(); ++it)
            push(*it);
    }
    
    virtual Job *pop() 
    {
        if (queue.size()) {
            Job *job = top();
            JobWeight jw_out = queue.back();
            
            // remove job
            queue.pop_back();
            in_use[jw_out.id] = 0;
            
            // update each element
            for (int i=0; i<queue.size(); i++)
            {
                JobWeight &jw = queue[i];
                jw.weight -= matrix[jw.id][jw_out.id];
            }
            return job;
        }else {
            return NULL;
        }

        
    }
    
    const char * getName() { return "MinPath"; }
};

//////////////////////////////////////////////////

class MinPathToSeedQueue : public QueueI
{
    GraphLoader *graph;
    
    std::vector<JobWeight> queue; // store jobs
    char *in_use; // where seeds are
    int **matrix;
    
    bool pop_ready;
public:
    ~MinPathToSeedQueue() {
        delete []in_use;
        for (size_t i=0; i<graph->npart; i++)
            delete[] matrix[i];
        delete[] matrix;
        delete graph;
    }
    
    MinPathToSeedQueue(const char *graph_filename)
    : pop_ready(false)
    {        
        int i,j,k;
        graph = new GraphLoader(graph_filename);
        int n=graph->npart;
        
        printf("Preparing MinPathQueue...");
        in_use = new char[n];
        for (i=0; i<n; i++)
            in_use[i] = 0;
        
        matrix = new int*[n];
        int big_num=n*n;
        for (i=0; i<n; i++)
        {
            matrix[i] = new int[n];
            // init
            for (j=0; j<n; j++) 
                matrix[i][j] = big_num;
            for (j=0; j<graph->edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = graph->edgeTableAry[i][j];
                matrix[i][ew.neighborID] = 1;
            }
            matrix[i][i] = 0;
        }
        
        // all pair shortest path
        for (k=0; k<n; k++)
            for (i=0; i<n; i++)
                for (j=0; j<n; j++)
                    matrix[i][j] = std::min( matrix[i][j], matrix[i][k]+matrix[k][j] );
        
        printf("Done\n");
#ifdef _DEBUG
        // show
        for (i=0; i<n; i++)
        {
            printf("%4d ", i);
            for (j=0; j<n; j++)
                printf("%3d", matrix[i][j]);
            printf("\n");
        }
#endif
        
    }
    
    virtual bool empty() 
    {
        return queue.size()==0;
    }
    virtual size_t size() 
    {
        return queue.size();
    }
    virtual Job *top() 
    {
        if (queue.size()==0)
            return NULL;
        if (!pop_ready) {
            float min;
            int min_idx=-1;
            for (int i=0; i<queue.size(); i++)
                if (min_idx==-1 || queue[i].weight<min) {
                    min = queue[i].weight;
                    min_idx = i;
                }
            std::swap(queue.back(), queue[min_idx]);
            
            pop_ready = true;
            
        }
        
#ifdef _DEBUG
        printf("Output job: %d (w=%f)\n", queue.back().id, queue.back().weight);
#endif
        return queue.back().pJob;
    }
    
    virtual void push(Job *pJob) 
    {
        int id = pJob->partID;
#ifdef _DEBUG
        int ii;
        for (ii=0; ii<queue.size(); ii++)
	        if (queue[ii].id==id)  {
                printf("push job already in MinPathQueue\n");
        		exit( 1 );
            }
#endif
        
        //addSeededBlock(id);
        
        int npart = graph->npart;
        int i;
        float w=0;
        for (i=0; i<npart; i++)
        {
            if (in_use[i]) {
                w += matrix[id][i];
            }
        }
        
        queue.push_back(JobWeight(id,w, pJob));
        
    }
    virtual void push(std::list<Job *> &jobs) 
    {
        for (std::list<Job *>::iterator it = jobs.begin(); it!=jobs.end(); ++it)
            push(*it);
    }
    
    virtual Job *pop() 
    {
        Job *job;
        if (!pop_ready) {
            job = top();
        }
        if (queue.size())
	        queue.pop_back();
        pop_ready = false;
        return job;
    }

    void addSeededBlock(int id)
    {
#ifdef _DEBUG
        printf("add seed block %d\n", id);
#endif
     	if (in_use[id]) return;
        
        in_use[id]=1;
   
        // update queue element
        for (int i=0; i<queue.size(); i++)
        {
            JobWeight &jw = queue[i];
            jw.weight += matrix[jw.id][id];
        }
    }
    
    void removeSeededBlock(int id) 
    {   
#ifdef _DEBUG
        printf("remove seed block %d\n", id);
        for (int ii=0; ii<graph->npart; ii++)
            printf("%d ", in_use[ii]);
        printf("\n");
#endif
        if (!in_use[id]) return;

        assert(in_use[id]);
        in_use[id] = 0;
        
        // update each element
        for (int i=0; i<queue.size(); i++)
        {
            JobWeight &jw = queue[i];
            jw.weight -= matrix[jw.id][id];
        }
                
    }
    
    const char * getName() { return "MinPathToSeed"; }
};

#endif //MIN_PATH_QUEUE_H