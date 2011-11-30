#include <sstream>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "Cluster.h"

double Cluster::w_Jaccard = 0.5;
double Cluster::short_penalizer = 0.0;

// initilize a clustering by words weight string
// Notice:must make sure s is ascending order
Cluster::Cluster(const string& s) {
	vector<string> tmps = util::split(s, '\t');
	assert(tmps.size() == 2);
    // set id
	m_id = atoi(tmps[0].c_str());
    // set centroid words
    vector<string> wvs = util::split(tmps[1], ',');
    for (vector<string>::iterator it = wvs.begin();
         it != wvs.end();
         ++it) {			 
        m_words.insert(*it);
    }
    // set document ids
    m_docs.push_back(m_id);
}

// merge words in l2 to l1, NOT empty l2
// if word already exists in l1, summize their value
void Cluster::add_words(const set<Word>& c2_words) {
    assert(!m_words.empty());
	// since in STL the elements value in set cannot be changed,
	// here need recreate a words set for accumulating weights.
    set<Word> tmp_words = m_words;
    m_words.clear();
    set<Word>::iterator p0 = m_words.begin(); // insert position
    
    set<Word>::iterator p1 = tmp_words.begin();
    set<Word>::const_iterator p2 = c2_words.begin();

	while (p1 != tmp_words.end() && p2 != c2_words.end()) {
		if (p1->id < p2->id) {
            p0 = m_words.insert(p0, *p1);
			++p1;
        }
		else if (p1->id > p2->id) {
			p0 = m_words.insert(p0, *p2);
			++p2;
		}else {
			// p1->id == p2->id
            Word w(p1->id, p1->v + p2->v);
            p0 = m_words.insert(p0, w);
            ++p1;
            ++p2;
		}
	}
    // remain words in doc, just insert before end
	if (p1 == tmp_words.end() && p2 != c2_words.end())
		m_words.insert(p2, c2_words.end());
    
	if (p1 != tmp_words.end() && p2 == c2_words.end())
		m_words.insert(p1, tmp_words.end());
}

// only return words with p(w|z) > 0.01 as effective words
void Cluster::get_effect_words(set<Word>& ewords) const {
    // sum of v
    double s = 0;
    double a = Cluster::w_Jaccard;
    
    for (set<Word>::iterator it = m_words.begin();
         it != m_words.end();
         ++it)
        s += pow(it->v, a);

    // select words with p(w|z) > 0.01
    for (set<Word>::iterator it = m_words.begin();
         it != m_words.end();
         ++it) {
        if (pow(it->v, a) / s > 0.01)
            ewords.insert(*it);
    }
}

// compute similarity between two clusters by linear combining 
// jaccard similarity and cosine similarty
double Cluster::similarity(const Cluster& c2){
	// double c = Cluster::w_Jaccard;
	// double s1 = Jaccard_sim(c2);
	// double s2 = cos_sim(c2);
	// double s = c * s1 + (1 - c)*s2;
	// return s;
    return a_sim(c2);
}

// Jaccard coefficient by words set
double Cluster::Jaccard_sim(const Cluster& c2) const {
	const set<Word>& c2_words = c2.get_words();
	set<Word> tmp;
	set_intersection(m_words.begin(), m_words.end(),
		c2_words.begin(), c2_words.end(),
		inserter(tmp, tmp.end()));

	// minus 0.5 to penalize short title
	size_t s = tmp.size();
    double sim = (s - Cluster::short_penalizer)/(m_words.size() + c2_words.size() - s);
	return max(sim, 0.0);
}

double Cluster::cos_sim(const Cluster& c2) const {
    double s1 = 0, s2 =0, sc = 0;

    const set<Word>& c2_words = c2.get_words();
    set<Word>::const_iterator it1 = m_words.begin();
    set<Word>::const_iterator it2 = c2_words.begin();
    
    // 2. add words in doc
    for (; it1 != m_words.end(); ++it1) {
        s1 += pow(it1->v, 2);
        while (it2 != c2_words.end() && it2->id < it1->id) {
            // word not in centroid, insert word
            s2 += pow(it2->v, 2);
            ++it2;
        }
        // word in centroid, add the v
        if (it2 != c2_words.end() && it2->id == it1->id) {
            s2 += pow(it2->v, 2);
            sc += (it1->v) * (it2->v);
            ++it2;
        }
    }
    // remain words in doc, just insert before end
    while (it2 != c2_words.end()) {
		s2 += pow(it2->v, 2);
        ++it2;
    }

    double result = (sc - Cluster::short_penalizer)/sqrt(s1*s2);    
    return max(result, 0.0);
}

double Cluster::a_sim(const Cluster& c2) const {
    double s1 = 0, s2 =0, sc = 0;
    double a = Cluster::w_Jaccard;

    set<Word> this_words, c2_words;
    get_effect_words(this_words);
    c2.get_effect_words(c2_words);
    
    set<Word>::const_iterator it1 = this_words.begin();
    set<Word>::const_iterator it2 = c2_words.begin();
    
    // 2. add words in doc
    for (; it1 != this_words.end(); ++it1) {
        s1 += pow(it1->v, 2*a);
        while (it2 != c2_words.end() && it2->id < it1->id) {
            // word not in centroid, insert word
            s2 += pow(it2->v, 2*a);
            ++it2;
        }
        // word in centroid, add the v
        if (it2 != c2_words.end() && it2->id == it1->id) {
            s2 += pow(it2->v, 2*a);
            sc += pow((it1->v) * (it2->v), a);
            ++it2;
        }
    }
    // remain words in doc, just insert before end
    while (it2 != c2_words.end()) {
		s2 += pow(it2->v, 2*a);
        ++it2;
    }

    double result = (sc - Cluster::short_penalizer)/sqrt(s1*s2);    
    return max(result, 0.0);
}

// merge two cluster
void Cluster::merge(const Cluster& c2) {
    // merge words
    const set<Word>& words = c2.get_words();
    
    add_words(words);
    // merge docs
    list<size_t> c2_docs = c2.get_docs();
	// each doc only belong to one cluster, so can directly splice
    m_docs.splice(m_docs.end(), c2_docs);
}

// output:centroid    docs
//        w1:weight,w2:weight,...   doc1, doc2, ...
string Cluster::toString(bool detail) const {
    ostringstream os;
    os << m_id << '\t';
    // normalzie
    // double s = 0.0;
    // for(set<Word>::const_iterator it = m_words.begin();
    //     it != m_words.end();
    //     ++it)
    //     s += it->v;    
    // for(set<Word>::const_iterator it = m_words.begin();
    //     it != m_words.end();
    //     ++it)
    //     it-> = it->v/s;    

    for(set<Word>::const_iterator it = m_words.begin();
        it != m_words.end();
        ++it){
        os << it->id << ":" << it->v << ",";
    }

    if ( detail ) {
        os <<'\t';
        for (list<size_t>::const_iterator it = m_docs.begin();
             it != m_docs.end();
             ++it)
            os << (*it) << ",";
    }
    
    return os.str();
}
