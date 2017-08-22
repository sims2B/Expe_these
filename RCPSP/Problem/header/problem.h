#ifndef DEF_PROBLEM
#define DEF_PROBLEM

#include <vector>
#include <iostream>
#include <fstream>

#include "resource.h"
#include "task.h"

typedef std::vector<task> tasklist;
typedef std::vector<resource> reslist;

struct problem{
	problem(int,int);
	problem(int,const tasklist&, const reslist&);
	problem(const problem&);
	
	inline int nbRes() {return R.size();}


	int readFromFile(std::ifstream&); 
	//La lecture de la ligne 1 (nbTask et nbRes) se fait 
	//dans le main (construction vector ~optimisée)
	int displayProblem();
	int timeWindows(std::ifstream&);

	int nbTask;
	tasklist A; 
	reslist R;
};

#endif
