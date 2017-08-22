#ifndef DEF_SOLUTION
#define DEF_SOLUTION


#include "problem.h"
#include <vector>

struct solution
{
  solution(int);
  int printSol();
  int isValid(const problem &) const;
  void clear();
  void reAlloc(int );  
  std::vector<type> st;
};


#endif
