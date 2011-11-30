// main clustering process
#ifndef CLUATERING_H_
#define CLUSTERING_H_

#define EXIT_ERROR( s ) {printf("[Error] %s\n", s); \
        exit(EXIT_FAILURE);}
#define EXIT_ERROR2( s1, s2 ) {printf("[Error] %s%s\n", s1, s2);    \
        exit(EXIT_FAILURE);}
#define MSG_FINISH( s ) printf("[Finish] %s\n", s); \

#include <vector>
#include <list>
#include <iostream>
#include <fstream>

#include "Cluster.h"
#include "Config.h"
#include "DimArray.h"


using namespace std;
class Controller {
    typedef list<Cluster>::iterator pCluster;
private:
    // minimal similarity for two cluster to concatenate a edge
    Config m_conf;
    double m_sim_thres;   
	double m_min_sim;
    double m_factor;           // increament factor of threshold
	//size_t m_K;                    // max clusters number
	size_t m_max_cluster_size;    // upper bound of a cluster size

    list<Cluster> m_clusters;
    // keep the random access property of clusters
    vector<pCluster> m_cid_ps;
    
    DimArray m_dims;

private:
    // read clusters into vector 'clusters' from data_pt
    // each line is a cluster:k:v,k:v
    void read_docs(const char* data_pt);

    void init_tool();

    // clustering clusters
    size_t cluster_merging(double sim_thres);

    // retrival similar clusters for cluster c using dimArray
    void get_cluster_neighbors(const pCluster& c, 
		double sim_thres, 
		list<pCluster>& nc);

	//merge clusters, update m_clusters and m_dims
	void merge_cluster_neighbors(pCluster p, list<pCluster>& nc);

    // transform cid to list<Cluster>::iterator
    pCluster& cid2cp(size_t id) {return m_cid_ps[id];}
    
public:
    Controller(const char* config_file, double threshold);
    
    // prepare data
    bool initialize();

    // main clustering process
    void run();

    // write clustering result
    void write_clusters();
    
};

#endif
