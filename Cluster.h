#ifndef _CLUSTER_H
#define _CLUSTER_H

#include <cassert>
#include <iostream>
#include <sstream>
#include <list>
#include <set>
#include <vector>
#include "string_util.h"
#include "Word.h"

using namespace std;

// cluster is a vector of Words
class Cluster
{
  private:
    size_t m_id;
	set<Word> m_words;   //ordered by word id
    list<size_t> m_docs;    // unordered doc id

  private:
    void add_words(const set<Word>& c2_words);

	double Jaccard_sim(const Cluster& c2) const;

	double cos_sim(const Cluster& c2) const;

    double a_sim(const Cluster& c2) const;
    
public:
	static double w_Jaccard;       // weight of Jaccard similarity
    static double short_penalizer;    // penalizer in Jarccard similarity for very short text

  public:
    Cluster (const string& s);

    bool empty() const { return m_docs.empty();}
    
    size_t id()  const {return m_id;}
	size_t size() const {return m_docs.size();}
    size_t words_num() const {return m_words.size();}
    
    const set<Word>& get_words() const {return m_words;}
    void get_effect_words(set<Word>& ewords) const;

    list<size_t> get_docs() const {return m_docs;}
 
    double similarity(const Cluster& c2);
    
    // insert into word list, keep increasing order
    //void insert(const size_t& doc);--del
    

    string toString(bool detail) const;

    void merge(const Cluster& c2);

};

#endif
