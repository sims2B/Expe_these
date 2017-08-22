#include "task.h"
#include <vector>
#include <iostream>


task::task(int nbRes, int nbSucc){
  p=0;
  b.resize(nbRes);
  succ.resize(nbSucc);
}

task::task(type _p,const interval& _tw,const std::vector<type>& _b,const std::vector<int>& _succ):
  p(_p),tw(_tw),b(_b),succ(_succ){}

task::task(const task& t){
  uint i;
  p=t.p;
  tw=t.tw;
  b.resize(t.b.size());
  succ.resize(t.succ.size());
  for (i=0; i<t.b.size();++i)
    b[i]=t.b[i];
  for (i=0; i<t.succ.size();++i)
    succ[i]=t.succ[i];
}

