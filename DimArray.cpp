#define CLOCK_TEST(cmd) {clock_t start = clock();\
	cmd;\
	clock_t end = clock();\
    printf("\t%s cost %f s\n",#cmd,(double)(end - start)/CLOCKS_PER_SEC);}

#include "DimArray.h"

void DimArray::add_cluster(int wid, const Cluster& c) {
	m_dims[wid].insert(c.id());
}


// only used for sequential clusters intialization
void DimArray::add_cluster_seq(const Cluster& c) {
    const set<Word>& c_dims = c.get_words();
        
    for(set<Word>::const_iterator it = c_dims.begin();
        it != c_dims.end();
        ++it) {
		set<size_t>& cids = m_dims[it->id];
		if ( cids.empty() )
			cids.insert(c.id());
		else {
			//optimization: insert from back
			cids.insert(cids.end()--, c.id());
		}
    }
}

// add cluster c in dimensons ws
//void DimArray::add_cluster(const set<int>& ws, const Cluster& c) {
//    for(set<int>::const_iterator it = ws.begin();
//        it != ws.end();
//        ++it) {
//        m_dims[*it].insert(c.id());
//    }
//}

void DimArray::erase_cluster(const Cluster& c) {
    const set<Word>& c_dims = c.get_words();
    
    for (set<Word>::const_iterator p_dim = c_dims.begin();
         p_dim != c_dims.end();
         ++p_dim) {
        // remove c from p_dim linked nodes
        m_dims[p_dim->id].erase(c.id());
    }
}

void DimArray::print() const {
    for(size_t i=0; i < m_dims.size(); ++i){
        printf("%ld\t", i);
        for(set<size_t>::const_iterator it = m_dims[i].begin();
            it != m_dims[i].end();
            ++it) {
            printf("%ld ", *it);
        }
        printf("\n");
    }
}
