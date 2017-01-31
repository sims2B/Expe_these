#ifndef DEF_SOLUTION
#define DEF_SOLUTION

#include "Problem.h"
#include <vector>

//TODO !!!! Fonction Solution<int,typename> isValid 
template<typename type,typename type2=type>
struct Solution{
  Solution(int,int);
  void clear();
  void reAlloc(int,int);
  void printSol();
  int isValid(const Problem<type> &) const;
	
  std::vector<type> st;
  std::vector<type> ft;
  std::vector<type> event;
  std::vector<std::vector<type2>> b;
};
#include "Solution.tpp"
#endif

