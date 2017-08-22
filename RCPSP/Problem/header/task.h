#ifndef DEF_TASK
#define DEF_TASK

#include "interval.h"
#include<vector>



struct task{
	task(int, int);
	task(type,const interval&,const std::vector<type>&,const std::vector<int>&);
	task(const task&);

	type p; //duration
	interval tw; //time window
	std::vector<type> b; //demand
	std::vector<int> succ;
};

#endif
