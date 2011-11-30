#define CLOCK_TEST(cmd) {clock_t start = clock();\
	cmd;\
	clock_t end = clock();\
	double d = (double)(end - start);\
	if (d>0.0) printf("\t%s cost %f s\n",#cmd, d/CLOCKS_PER_SEC);}

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <set>
#include <sstream>
#include <iterator>
#include <ctime>
#include "Controller.h"

Controller::Controller(const char* config_file, double threshold = -1):
    m_conf(config_file), m_sim_thres(threshold){
	cout << "Parsing config file:" << config_file << endl;
    // set default value
    m_factor = 1.2;
    m_max_cluster_size = 100000;
}

// read configurations and initialize data structures
bool Controller::initialize() {
	if (!m_conf.parse()) 
		EXIT_ERROR("Parse error in config.in!");

    // read threshold
    if (m_sim_thres == -1) {           // threshold not given by argc[1]
		m_sim_thres = strtod(m_conf.get("sim_thres"), NULL);
		if (m_sim_thres == -1)
			EXIT_ERROR("threshold is not set in config.in!");
    }
    cout << "set similarity threshold = " << m_sim_thres << endl;

    // read min_sim
    if (!m_conf.has_key("min_sim"))
		EXIT_ERROR("min_sim is not set in config.in!");
	m_min_sim = strtod(m_conf.get("min_sim"), NULL);
	cout << "set min_sim = " << m_min_sim << endl;    

    // read factor
    if (!m_conf.has_key("factor")) 
		EXIT_ERROR("factor is not set in config.in!");
	m_factor = strtod(m_conf.get("factor"), NULL);
	cout << "set clustering similarity threshold increasing factor = " << m_factor << endl;

    // read short_penalizer
    if (!m_conf.has_key("short_penalizer")) 
		EXIT_ERROR("short_penalizer is not set in config.in!");
	double short_penalizer = strtod(m_conf.get("short_penalizer"), NULL);
    Cluster::short_penalizer = short_penalizer;
	cout << "set short_penalizer = " << short_penalizer << endl;

	// read weight of Jaccard smilarity
    if (!m_conf.has_key("w_Jaccard")) 
		EXIT_ERROR("w_Jaccard is not set in config.in!");
	double w_Jaccard = strtod(m_conf.get("w_Jaccard"), NULL);
	Cluster::w_Jaccard = w_Jaccard;
	cout << "set Jaccard similarity weight = " << w_Jaccard << endl;

    // read max_cluster_size
    if (!m_conf.has_key("max_cluster_size"))
		EXIT_ERROR("max_cluster_size is not set!");
	m_max_cluster_size = atoi(m_conf.get("max_cluster_size"));
	cout << "set max cluster size = " << m_max_cluster_size << endl;    

	// read data_pt
    if (!m_conf.has_key("data_pt"))
		EXIT_ERROR("data_pt is not set!");
    string data_pt = m_conf.get("data_pt");
    if (data_pt.empty()) 
		EXIT_ERROR("data_pt is not valid data file!");
        
    read_docs(data_pt.c_str());
    if (m_docs.empty())
        EXIT_ERROR2("no documents read from ", data_pt.c_str());
    cout << "readed documents, size = " << m_docs.size() <<endl;

    // initialize dimArray
    size_t dim_size = atol( m_conf.get("feature_dimension")); 
    m_dims.resize(dim_size);
    if (m_dims.empty())
        EXIT_ERROR("Bad init cluster data format, feature dimension is zero!");
    cout << "DimArray initialized, size = " << m_dims.size() <<endl;
    
	// read result pt
	if (!m_conf.has_key("result_pt"))
		EXIT_ERROR("result_pt is not set!");

    printf("initialization ended\n");
    return true;
}

void Controller::read_docs(const char* data_pt) {
    ifstream inf(data_pt);
	if (!inf) 
	  EXIT_ERROR2("Cannot open data file:", data_pt);	

    string line;
    size_t i = 0;
    while (getline(inf, line)) {
		util::trim(line);
		if (line.empty()) {
			cout<< "[Warn] empty line:" << i<<endl;
            continue;
        }
        m_docs.push_back(line);
        ++i;
    }
}

void Controller::run(){
    printf("docs init cluster\n");
    doc_clustering_init();
    printf("docs init cluster end\n");
    
    int looper = 0;
    size_t k = m_clusters.size();
    
    // clusers merging pass by pass, until thres_sim less than min_sim
    while ( m_sim_thres > m_min_sim ) {
        printf("%d-th merge, m_sim_thres = %f, clusters num = %d\n", looper++, m_sim_thres, k);
        k = cluster_merging(m_sim_thres);
        m_sim_thres *= m_factor;
        //    write_clusters();
    }
    
    write_clusters();

	printf("last clusters size = %d\n", k);
}
    
/*
 * @Function
 *    initialize each document as a cluster
 */
void Controller::doc_clustering_init() {
	clock_t start = clock(); 
    // one traversal
    size_t n_docs = m_docs.size();
    
    m_cid_ps.resize(n_docs);
    size_t looper = 0;
    for (list<Doc>::const_iterator it_d = m_docs.begin();
         it_d != m_docs.end();
         ++it_d) {
		//  if (looper > 140000) {
		//	// assign a document('s iterator) to a cluster
		//	m_clusters.push_back(Cluster(m_clusters.size()));
		//	Cluster& c= m_clusters.back();

		//	CLOCK_TEST(c.insert(it_d);)

		//	pCluster it_c = m_clusters.end();
		//	it_c--;//get the iterator before the last

		//	// update cid-cluster_iterator table
		//	CLOCK_TEST(m_cid_ps[c.id()] = it_c;)
  //      
		//	// update dimAarray
		//	CLOCK_TEST(m_dims.add_cluster_seq(*it_c);)
		//}
		//else {
			// assign a document('s iterator) to a cluster
			m_clusters.push_back(Cluster(m_clusters.size()));
			Cluster& c= m_clusters.back();

			c.insert(it_d);

			pCluster it_c = m_clusters.end();
			it_c--;//get the iterator before the last

			// update cid-cluster_iterator table
			m_cid_ps[c.id()] = it_c;
        
			// update dimAarray
			m_dims.add_cluster_seq(*it_c);
		//}

		// time counter
		if (++looper % 10000 == 0) {
			clock_t finish = clock();
			double duration = (double)(finish - start) / CLOCKS_PER_SEC; 
			printf("finish num:%d, cost %f s\n", looper, duration);
			start = finish;
		}
    }
}


/*
 * @Function
 *    travel all clusters sequentially, merge two clusters if similar enough
 * @para
 *    - sim_thres    if two clusters similarity larger than it, then merging
 *    - max_cluster_size   if a cluster's size bigger than it, don't merging
 * @Return
 *   the clusters number after merging
 */
size_t Controller::cluster_merging(double sim_thres) {
    pCluster p = m_clusters.begin();
    int looper = 0;
    
    // forward search, find all the enough similar clusters, and merge them
    while (p != m_clusters.end()){
		// debug
		size_t cur_id = p->id();

        // retrieval neighbor clusters from dimArray
        list<pCluster> p_nc;  // neighbor clusters of cluster p
        get_cluster_neighbors(p, sim_thres, p_nc);

        // merge other clusters in filtered neibours
		if (!p_nc.empty())
			merge_cluster_neighbors(p, p_nc);
		
		++p;
        
        if (looper % 100 ==0) 
            cout<< looper++ << " loop, cluster size:"<< m_clusters.size() <<endl;
    } // end for it

    return m_clusters.size();
}

// find clusters close to c with a similarity bigger than sim_thres
// TIME REDUCE: since our similarity measure encourage to math more words,
// the neighbors search process first compute similarity with clusters which
// have more common words, until reach sim_thres. Remaining clusters will not computed.
void Controller::get_cluster_neighbors(const pCluster& c,
                             double sim_thres,
                             list<pCluster>& nc) {
    // record the number of common words for each neighbors
    map<size_t, size_t> com_words;//<cid, common_words_num>

	const set<Word>& c_words = c->get_words();
    // collect clusters which have common words with nc, and the number
    for (set<Word>::const_iterator p = c_words.begin();
         p != c_words.end();
         ++p){
        const set<size_t>& dim_cids = m_dims.at(p->id);
            
        for (set<size_t>::const_iterator pc = dim_cids.begin();
             pc != dim_cids.end();
             ++pc){
            assert(m_cid_ps[*pc] != m_clusters.end());
            
			// Only look forward, since there is no need to compute 
			// similary between two clusters twice.
			if ((*pc) <= c->id()) continue;
            
            // whether already recorded
            if ( com_words.find(*pc) != com_words.end() )
                com_words[*pc] += 1;
            else
                com_words[*pc] = 1;
        }// end for pc
    }// end for p

    // sort cids by number of common words in decreasing order
    multimap<size_t, size_t> v_cids;//<common_words_num, cid>
    for (map<size_t, size_t>::iterator it = com_words.begin();
         it != com_words.end();
         ++it) {
        v_cids.insert( pair<size_t, size_t>((*it).second, (*it).first) );
    }

    int loop = 0;
    // filter neighbors not similar enough
    size_t min_com_words = 1;   // if common words less than it, stop compare
    for (multimap<size_t, size_t>::iterator it = v_cids.begin();
         it != v_cids.end();
         ++it) {
        size_t n_com_words = it->first;
        // stop for assert no more neighbors
        if (n_com_words < min_com_words) break;

        // similarity test
        size_t cur_cid = it->second;
        const pCluster& cur_cluster = cid2cp(cur_cid);
        assert(cur_cluster != m_clusters.end());
        
		double cur_sim;
        cur_sim = c->similarity(*cur_cluster);
        
        if ( cur_sim > sim_thres) {
            nc.push_back(cur_cluster);
            // debug
            //            cout <<"merge "<< c->id() << ":" << c->words_num() << " + " << cur_cluster->id() <<":"<< cur_cluster->words_num() << "\t sim = " << cur_sim << endl;
        }
        else
			// early stop when not match enough, maybe relaxed
            min_com_words = n_com_words;        
    }
}

/* 
 * @Function
 *    merge clusters, update m_clusters and m_dims
 * NOTICE: The cluster ids will nerver be changed
 */
void Controller::merge_cluster_neighbors(pCluster p, list<pCluster>& nc) {
	set<Word> nc_words;// words are absorbed from neighbors
	set<Word> c1_words = p->get_words();

    // merge each neighbor and update dimArray
    for (list<pCluster>::iterator it_nc = nc.begin();
         it_nc != nc.end();
         ++it_nc) {
		// collect neighbor's words
		const set<Word>& c2_words = (*it_nc)->get_words();
		nc_words.insert(c2_words.begin(), c2_words.end());

        // cluster merging
        p->merge(**it_nc);

        // erase nc from DimArray
		m_dims.erase_cluster(**it_nc);
        // remove nc from m_clusters
        m_cid_ps[(*it_nc)->id()] = m_clusters.end();
        m_clusters.erase(*it_nc);
    }

	// compute difference words between neighbors and p
	set<Word> diff_words;
	set_difference(nc_words.begin(), nc_words.end(),
		c1_words.begin(), c1_words.end(),
		inserter(diff_words, diff_words.end())
		);

	// update dimArray where words newly added
	for (set<Word>::const_iterator it_diff = diff_words.begin();
		it_diff != diff_words.end();
		++it_diff)
		m_dims.add_cluster(it_diff->id, *p);
}

void Controller::write_clusters() {
	ostringstream oss;
	oss << m_conf.get("result_pt") << "k" << m_clusters.size() << ".txt";
    
	string pt = oss.str();
    cout << "clustering result write to: " << pt <<endl;

    ofstream wf(pt.c_str());
	if (!wf) {
		// if failed, write the default file
		cout << "[Warn]Cannot write result file:" << pt << endl;
		wf.open("result.txt");
	}

    for (list<Cluster>::iterator it = m_clusters.begin();
         it != m_clusters.end();
         ++it) {
        if (it->id() != -1)
          wf << it->toString(true) <<endl;  // for test
    }
    wf.close();
}
