#include <iostream>
#include "Controller.h"
#include <Windows.h>

//test
int main(int argv, char* argc[]) {
	const char* conf_pt = "config.in";
	//const char* conf_pt = "Z:\\webRec\\hierarchialClustering\\config.in_win";

    double threshold = -1;
	if (argv == 2) {
      threshold = atof(argc[1]);
      //conf_pt = argc[1];
    }
	Controller job(conf_pt, threshold);
    if (job.initialize()) {
        job.run();
    }
	system("pause");
	return 0;
}
