#include "pre_proc_MIP.h"

//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
bool intervalComp(const interval &i,const interval &j) { return (i.t1<j.t1 || (i.t1==j.t1 && i.t2<j.t2)); }


int boundEvts(const problem& P, std::vector<type>& bd){
  const int n=P.nbTask;
  std::vector<type> I(n);
  for (int i=0;i<n;++i)
    I[i]=P.A[i].tw.t2;
  std::sort(I.begin(),I.end());

  bd.resize(n);
  for (uint i=0;i<bd.size();++i) 
    bd[i]=I[i];
  //bd[n-1] = I[n-1]; // we have to make sure that the last index is set correctly
  return 0;
}

int boundSepEvts(const problem& P,std::vector<type>& bound){
  std::vector<interval> I(P.nbTask);
  for (int i=0;i<P.nbTask;++i)
    I[i]=P.A[i].tw;
  std::sort(I.begin(),I.end(),intervalComp);

  bound.resize(P.nbTask-1);
  for (uint i=0;i<bound.size();++i)
    bound[i]=std::max(I[i+1].t2-I[i].t1,I[i].t2-I[i+1].t1);
  return 0;
}

type maxSepEvts(const std::vector<type>& bound){
  type max=0;
  for (uint i=0;i<bound.size();++i)
    max=std::max(max,bound[i]);
  return max;
}
