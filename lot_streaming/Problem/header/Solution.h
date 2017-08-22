#ifndef solution
#define solution

#include <vector>
#include "LotProblem.h"

struct Solution{
  //start time of batch q of job i (-1 batch q is empty)
  std::vector < std::vector <double> > st;
  ///end time of batch q of job i (-1 batch q is empty)
  std::vector < std::vector <double> > ft;
  //machine on which batch q of job i is executed(-1 batch q is empty)
  std::vector < std::vector <int> > a;
  //start time of maintenance operation
  std::vector <double > dm;

  Solution(int n,int Q,int m){
    st.resize(n);
    ft.resize(n);
    a.resize(n);
    dm.assign(m,-1.0);
    for (int i=0;i<n;++i){
      st[i].assign(Q,-1.0);
      ft[i].assign(Q,-1.0);
      a[i].assign(Q,-1);
    }
  }
  
  //checker
  int isValid(const LotProblem&) const;
  
  void printSol(const LotProblem &) const;
  
  //this function does not work
  double peak(const LotProblem &) const;	
  //compute the total consumption
  double conso(const LotProblem &) const;
  //does not work (because it uses peak)
  double totalObj(const LotProblem &,double,double,double) const;
  //compute tardiness
  double tardiness(const LotProblem&) const;

  };

#endif
