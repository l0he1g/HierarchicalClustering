// Copyright [2011-03-30] <yanxiaohui@software.ict.ac.cn>
#ifndef CONFIG_H_
#define CONFIG_H_
#include <cstdlib>
#include <string>
#include <fstream>
#include <map>
#include <utility>
#include "string_util.h"

using namespace std;

class Config {
private:
    const char* m_file;
    map<string, string> _map;

public:
    Config(const char* file): m_file(file) {}
    bool parse() {
        ifstream inf(m_file);
		if (!inf) {
			cerr << "[Error] Cannot open config initial clusters file: " << m_file <<endl;
			exit(EXIT_FAILURE);
		}
        string line;

        while (getline(inf, line)) {
			util::trim(line);
			if (line.empty()) continue;
            if (line.at(0) == '#') continue;   // filter comment

			vector<string> tmps = util::split(line, '=');
            string k = tmps[0];
            string v = tmps[1];
            util::trim(k);
            util::trim(v);
            _map.insert(make_pair(k, v));
        }

        return (_map.size() > 0);
    }

	const char* get(const string& k) {
        return _map[k].c_str();
    }

	bool has_key(const string& k) {
        return (_map.count(k) > 0);
    }
    
    void set(const string& k, const string& v) {
        _map.insert(make_pair(k, v));
    }

    //test
    void print() {
        for( map<string, string>::iterator it = _map.begin();
             it != _map.end();
             ++it){
            printf("k=%s, v=%s\n",  it->first.c_str(), it->second.c_str());
        }
    }    
};

#endif
