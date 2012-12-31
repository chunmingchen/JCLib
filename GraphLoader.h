/*
 *  GraphLoader.h
 *  parallelStreamline
 *
 *  Created by MiGi on 1/27/11.
 *  Copyright 2011 OSC. All rights reserved.
 *
 */

#ifndef GRAPH_LOADER_H
#define GRAPH_LOADER_H

#include <functional> // greater<>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>

struct EdgeWeight{
    int neighborID;
    float weight;
     bool operator<(const EdgeWeight &e) const {return weight<e.weight;}
    bool operator>(const EdgeWeight &e) const {return weight>e.weight;}
};

typedef std::vector<EdgeWeight> EdgeTable;


class GraphLoader{
    
    
    void loadFile_bin(const char *filename)
    {
        FILE *fp = fopen(filename, "rb");

        if (!fp) throw "Graph file not found.";
        fread(&npart, sizeof(int), 1, fp);
        fread(&nedge, sizeof(int), 1, fp);
        int i,j;
        edgeTableAry = new EdgeTable[npart];
        vertexTotalWeight = new float[npart];
        totalWeight = 0;
        for (i=0; i<npart; i++)
        {
            int edges;
            vertexTotalWeight[i] = 0;
            //fread(&id, sizeof(int), 1, fp);
            fread(&edges, sizeof(int), 1, fp);
            
            for (j=0; j<edges; j++)
            {
                EdgeWeight ew;
                int dum;
                fread(&ew.neighborID, sizeof(int), 1, fp);
                fread(&dum, sizeof(int), 1, fp);
                ew.weight = (float)dum;
                totalWeight+=ew.weight;
                vertexTotalWeight[i] += ew.weight;
                edgeTableAry[i].push_back(ew);
#ifdef __DEBUG
                printf("%d->%d:%g ", i, ew.neighborID, ew.weight);
#endif
            }
#ifdef __DEBUG
            printf("\n");
#endif
        }
        fclose(fp);
        //printf("Total Weight=%llf\n", totalWeight);
    }
    
    void loadFile_txt(const char *filename)
    {
        FILE *fp = fopen(filename, "rt");
            
        assert(fp);
        fscanf(fp, "%d %d", &npart, &nedge);
        int i,j;
        edgeTableAry = new EdgeTable[npart];
        vertexTotalWeight = new float[npart];
        totalWeight = 0;
        while(!feof(fp))
        {
            int  edges;
            fscanf(fp, "%d %d", &i, &edges);
            vertexTotalWeight[i] = 0;
            
            for (j=0; j<edges; j++)
            {
                EdgeWeight ew;
                fscanf(fp, "%d %f", &ew.neighborID, &ew.weight);
                totalWeight+=ew.weight;
                edgeTableAry[i].push_back(ew);
#ifdef _DEBUG
                printf("%d->%d:%f ", i, ew.neighborID, ew.weight);
#endif
                // statistics
                vertexTotalWeight[i] += ew.weight;
            }
#ifdef _DEBUG
            printf("\n");
#endif
        }
        fclose(fp);
        //printf("Graph total Weight=%f\n", totalWeight);
    }
    

    void loadFile_rmf(const char *filename)
    {  
        // Save
        FILE *fp = fopen(filename, "rt");
        if (!fp) throw std::string("Cannot load file:")+filename;

        // start
        char dum[1024];
        fscanf(fp, "%s %s %d %d\n", dum, dum, &npart, &nedge);
        edgeTableAry = new EdgeTable[npart];
        vertexTotalWeight = new float[npart];
        while(!feof(fp))
        {
            EdgeWeight ew; int v;
            fscanf(fp, "%s %d %d %f\n", dum, &v, &ew.neighborID, &ew.weight);
            v--; ew.neighborID--;
            edgeTableAry[v].push_back(ew);

            // statistics
            vertexTotalWeight[v] += ew.weight;
#ifdef _DEBUG
		printf("%d->%d:%f ", v, ew.neighborID, ew.weight);
#endif
        }
#ifdef _DEBUG
		printf("\n");
#endif
        fclose(fp);
    }



    double * getSteadyState(int blockSeeds)
    {
        int i;
        int cur=0, next=1;
        double *pr[2];
        pr[0] = new double[npart];
        pr[1] = new double[npart];
        for (i=0; i<npart; i++)
            pr[cur][i] = 1.f;
        
        int v, e;
        printf("Get Ready State");
        for (i=0; i<1000; i++)
        {
            for (v=0; v<npart; v++)
                pr[next][v] = 0;
            // for each vertex
            for (v=0; v<npart; v++)
            {
                // for each edge
                for (e=0; e<(int)edgeTableAry[v].size(); e++)
                {
                    EdgeWeight &ew = edgeTableAry[v][e];
                    pr[next][ew.neighborID] += pr[cur][v] * ew.weight / (double)blockSeeds;
                }
            }
#ifdef _DEBUG
            printf("pr[0] = %le  %%=%lf\n", pr[cur][0], pr[next][0]/pr[cur][0]);
#endif
            cur = (cur+1)%2;
            next = (next+1)%2;
            
            // sum=1.0
            {
                double sum=0;
                int v;
                for (v=0; v<npart; v++)
                    sum+=pr[cur][v];
                for (v=0; v<npart; v++)
                {
                    pr[cur][v] /= sum;
                    // printf("pr[%d]=%le\n", v, pr[cur][v]);
                }            
            }
        }        
        delete []pr[next];
        return pr[cur];
    }
    
public:
    EdgeTable *edgeTableAry; 
    int npart;
    
    // statistics
    int nedge;
    long double totalWeight;
    float *vertexTotalWeight;    
    
    GraphLoader(int npart_, EdgeTable *edgeTableAry_=NULL) {
        npart = npart_;
        if (edgeTableAry_==NULL)
            edgeTableAry = new EdgeTable[npart_]; // empty graph
        else
            edgeTableAry = edgeTableAry_;
        init_nedge();
    }
    GraphLoader(const char *filename) {
        assert(filename);
        if (strcmp(".txt", filename+(strlen(filename)-4))==0)
            loadFile_txt(filename);
        else if (strcmp(".rmf", filename+(strlen(filename)-4))==0) 
            loadFile_rmf(filename);
       else
            loadFile_bin(filename);
        init_nedge();

    }
    
    // normalize the sum of outgoing weights to be 1
    void normalize() {
        int i;
        for (i=0; i<npart; i++)
        {
            for (int j=0; j<(int)edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                ew.weight /= vertexTotalWeight[i];
//                printf("%f\t", ew.weight);
            }
            vertexTotalWeight[i] = 1.;
        }
    }
    void operator/(float x) {
        int i;
        for (i=0; i<npart; i++)
        {
            for (int j=0; j<(int)edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                ew.weight /= x;
                //                printf("%f\t", ew.weight);
            }
            vertexTotalWeight[i] = 1.;
        }
    }
    
    float weight(int u, int v)
    {
        int i;
        for (i=0; i<edgeTableAry[u].size(); i++)
            if (edgeTableAry[u][i].neighborID==v)
                return edgeTableAry[u][i].weight;
        return 0;
    }
    EdgeWeight *getEdge(int u, int v)
    {
        int i;
        for (i=0; i<edgeTableAry[u].size(); i++)
            if (edgeTableAry[u][i].neighborID==v)
                return &edgeTableAry[u][i];
        return NULL;
    }        
    void setEdge(int u, int v, float weight)
    {
        EdgeWeight *pew;
        if ((pew=getEdge(u, v))==NULL) {
            EdgeWeight ew;
            ew.neighborID = v;
            ew.weight = weight;
            edgeTableAry[u].push_back(ew);
        }else
            pew->weight = weight;
    }
    void addWeight(int u, int v, float weight)
    {
        EdgeWeight *pew;
        if ((pew=getEdge(u, v))==NULL) {
            EdgeWeight ew;
            ew.neighborID = v;
            ew.weight = weight;
            edgeTableAry[u].push_back(ew);
        }else
            pew->weight += weight;
    }
    
    void sortEdges()
    {
        int i;
        for (i=0; i<npart; i++)
        {
            std::sort( edgeTableAry[i].begin(), edgeTableAry[i].end(), std::greater<EdgeWeight>() );
            
        }
    }
    
    void showall()
    {
        int i;
        for (i=0; i<npart; i++)
        {
            int j;
            for (j=0; j<edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                printf("%d->%d: %f\n", i,ew.neighborID,ew.weight);
            }
        }
    }
    
    void weightedSteadyState() 
    {
        double *steadyState;    
        steadyState = getSteadyState(16*16*6);
        int i,j;
        for (i=0; i<npart; i++)
        {
            EdgeTable &edgeTable = edgeTableAry[i];
            // for each edge in the vertex
            for (j=0; j<(int)edgeTable.size(); j++)
            {
                EdgeWeight &ew = edgeTable[j];
                ew.weight *= steadyState[i];
            }
        }
        delete[] steadyState;
    }
    
    void init_nedge()
    {
        int i;
        // statistics
        nedge=0;
        for (i=0; i< npart; i++)
            nedge+=edgeTableAry[i].size();
    }
    
    float averageDegree()
    {
        init_nedge();
        return (float)nedge/npart;
    }
    
    void operator/=(const float div)
    {
        for (int i=0; i<npart; i++)
        {
            for (int j=0; j<edgeTableAry[i].size(); j++)
                edgeTableAry[i][j].weight /= div;
            vertexTotalWeight[i] /= div;
        }
    }
    void operator*=(const float mul)
    {
        for (int i=0; i<npart; i++) {
            for (int j=0; j<edgeTableAry[i].size(); j++)
                edgeTableAry[i][j].weight *= mul;
            vertexTotalWeight[i] *= mul;
        }
    }
    
    
    void saveFile_bin(const char *filename)
    {
        // Save
        FILE *fp=NULL;
        printf("Writing file: %s\n", filename);
        fp = fopen(filename, "wb");
        if (!fp) throw std::string("Cannot save file:")+filename;
        
        // npart
        //printf("# <number of vertices> <number of edges>\n");
        //printf("%d %d\n", npart, nedge);
        fwrite(&npart, sizeof(int), 1, fp);
        fwrite(&nedge, sizeof(int), 1, fp);
        //printf("# graph edges \n"
        //       "# format: <block id> <number of edges> <edge block id> <edge weight> ...\n");
        for (int i=0; i<npart; i++)
        {
            int size = (int)edgeTableAry[i].size();
            //printf("%d %u", i, size);
            //fwrite(&i, sizeof(int), 1, fp);
            fwrite(&size, sizeof(int), 1, fp);
            for (int j=0; j<size; j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
#ifdef _DEBUG
                printf(" %d %g", ew.neighborID, ew.weight);
#endif
                fwrite(&ew.neighborID, sizeof(int), 1, fp);
                int dum = ew.weight; // precision loss!
                fwrite(&dum, sizeof(int), 1, fp);
            }
            //printf("\n");
        }
        
        fclose(fp);
    }
    
    void saveFile_rmf(const char *filename)
    {
        // Save
        FILE *fp=NULL;
        printf("Writing file: %s\n", filename);
        fp = fopen(filename, "wt");
        if (!fp) throw std::string("Cannot save file:")+filename;
        
        // start
        fprintf(fp, "p ghct %d %d\n", npart, nedge);
        for (int i=0; i<npart; i++)
        {
            for (int j=0; j<(int)edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                fprintf(fp, "e %d %d %f\n", i+1, ew.neighborID+1, ew.weight);
            }
        }
        fclose(fp);
    }
    
    
    inline void propagate_add(std::vector<long double> &x)
    {
        int i,j;
        std::vector<long double> y(npart,0);
        
        for (i=0; i<npart; i++)
            for (j=0; j<edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                y[ew.neighborID]+=x[i]*ew.weight;
            }
        for (i=0; i<npart; i++)
            x[i] = y[i];
    }
    
    inline void add(std::vector<long double> &x, std::vector<long double> &adder)
    {
        for (int i=0 ; i<npart; i++)
            x[i]+=adder[i];
    }
    
    // x = 1+Ax+A*Ax+A*A*Ax+...
    void getVertexWeight()
    {
        std::vector< double> x(npart, 1.);
        std::vector< double> sum(npart, 1.);
        std::vector< double> y(npart,0);
        int c;
        int i,j;
        for (c=0; c<100; c++)
        {
            if (c%10==0)
                printf("%d, sum[0]=%lf\n", c, sum[0]);
            y.clear();

            // propagate
            for (i=0; i<npart; i++) 
            {
                for (j=0; j<edgeTableAry[i].size(); j++)
                {
                    EdgeWeight &ew = edgeTableAry[i][j];
                    y[ew.neighborID]+=x[i]*ew.weight;
                }
            }
            // add
            for (i=0; i<npart; i++) {
                x[i] = y[i];
                sum[i] += y[i];
            }
        }
        long double total=0;
        for (i=0; i<npart; i++)
            total += sum[i];
        for (i=0; i<npart; i++) 
            sum[i]/=total;
    
#ifdef _DEBUG
        printf("vertex weight: ");
        for (i=0; i<npart; i++)
            printf("%lf ", sum[i]);
        printf("\n");
#endif
    
        // times original edge
        
        for (i=0; i<npart; i++)
            for (j=0; j<edgeTableAry[i].size(); j++)
            {
                EdgeWeight &ew = edgeTableAry[i][j];
                ew.weight = (ew.weight*sum[i]*32768);
            }
    
    }
    
    
};

#endif
