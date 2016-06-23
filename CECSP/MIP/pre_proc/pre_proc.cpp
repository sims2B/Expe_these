#include "pre_proc.h"

//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
bool intervalComp(const Interval<double> &i,const Interval<double> &j) { 
  return (i.t1<j.t1 || (i.t1==j.t1 && i.t2<j.t2)); 
}


int boundEvts(const Problem<double>& P, std::vector<double>& bd){
  const int n=P.nbTask;
  std::vector<double> I(2*n);
  for (int i=0;i<n;++i){
    I[2*i]=P.smax(i);
    I[2*i+1]=P.d(i);
  }
  std::sort(I.begin(),I.end());

  bd.resize(2*n);
  for (uint i=0;i<bd.size()-1;++i) 
    bd[i]=I[i+1];
  bd[2*n-1]=P.D;
  return 0;
}

int boundSepEvts(const Problem<double>& P,std::vector<std::vector<double>>& bound, const int& sep){
  std::vector<Interval<double>> I(2*P.nbTask);
  for (int i=0;i<P.nbTask;++i){
    I[2*i]=Interval<double>(P.r(i),P.smax(i));
    I[2*i+1]=Interval<double>(P.emin(i),P.d(i));
  }
  std::sort(I.begin(),I.end(),intervalComp);

  bound.resize(2*P.nbTask-1);
  for (uint i=0;i<bound.size();++i){
    bound[i].resize(std::min(sep,2*P.nbTask-(int)i));
    for (int j=1;j<=sep && i+j < I.size() ; j++)
      bound[i][j-1]=std::max(I[i+j].t2,I[i].t2)-std::min(I[i].t1,I[i+j].t1);
  }
  return 0;
}

