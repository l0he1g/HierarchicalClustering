#ifndef WORD_H
#define WORD_H

#include <string>
#include <cstdio>
#include <sstream>

using namespace std;

class Word
{
public:
	Word():id(-1),v(0) {}
	Word( int id, double v):id(id),v(v){}
	// construct from string : id:v
	Word(const string& s){
		istringstream ss(s);
		char tmp;
		ss >> id >> tmp >> v;
//		sscanf(s.c_str(), "%d:%lf", &id, &v);
		assert(id >=0 && v > 0);
	}

    Word( const Word& w) {id = w.id; v = w.v;}

	bool operator < (const Word& w) const {return id < w.id;}    
public:    
	int id;
	double v;
};

#endif
