/*
 * @Function
 *     DimArray is a auxiliary data structure to speed up the neighbors finding for
 *     a long list objects, which is high dimensional.
 * @ Describle
 *     DimArray stores the iterators of the list objects by their dimensions.
 *     Both word and clusters are represent by id(size_t type)
 * @ Author
 *     Yan, Xiaohui(l0he1g@gmail.com)  2011-11-5
 * @ Resource
 *     DimArray is derived by Cao, Huanhuan in paper:
 *     'Context-aware query suggestion by mining click-through and sesssion data', kdd08
 */
#ifndef _DIMARRAY_H
#define _DIMARRAY_H
#include <vector>
#include <list>
#include <set>
#include "Cluster.h"
#include "Doc.h"

class DimArray {
private:
    vector< set<size_t> > m_dims;
    
public:
	DimArray(){}

    void resize(size_t size){m_dims.resize(size);}

    bool empty() const {return m_dims.empty();}
    
    const size_t size() const {return m_dims.size();}
    
    const set<size_t>& at(size_t i) const {return m_dims[i];}
    
    void add_cluster_seq(const Cluster& c);
	void add_cluster(int wid, const Cluster& c);
	//void add_cluster(const set<int>& ws, const Cluster& c);
    
    // erase cluster c in DimArray
    void erase_cluster(const Cluster& c);
            
    // for test
    void print() const;
    
};
#endif
