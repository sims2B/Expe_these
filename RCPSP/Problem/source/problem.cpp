#include <vector>
#include <iostream>
#include <fstream>
#include "problem.h"

using namespace std;

problem::problem(int _nbTask,int nbRes) {
  nbTask=_nbTask;
  task t(0,0);
  A.resize(_nbTask,t);
  R.resize(nbRes);
}

problem::problem(int _nbTask,const tasklist& _A, const reslist& _R): nbTask(_nbTask),A(_A), R(_R){}

problem::problem(const problem& P){
  int i;
  nbTask=P.nbTask;
  for (i=0;i<nbTask;++i)
    A[i]=P.A[i];
  for (i=0;i<(int)P.R.size();++i)
    R[i]=P.R[i];
}

int problem::readFromFile(std::ifstream& instance){
  type val,p;
  uint k;
  uint nbRes=this->nbRes();
  int nbSucc,j,i;

  for (k=0;k<nbRes;++k){
    instance >> val;
    R[k]=resource(val);
  }
  std::vector<type> demand;
  std::vector<int> succesor;
  for (i=0;i<nbTask;++i){
    demand.resize(nbRes);
    instance >> val;
    p=val;
    for (k=0;k<nbRes;++k){
      instance >> val;
      demand[k]=val;
    }
    instance >> nbSucc;
    succesor.resize(nbSucc);
    for (j=0;j<nbSucc;++j){
      instance >> val;
      succesor[j]=val-1;
    }
    A[i]=task(p,interval(),demand,succesor);
  }
  return 0;
}

int problem::displayProblem(){
  cout<<"  + Data Display"<<endl;
  uint i,j,k;
  cout<<"i"<<'\t'<<"p[i]"<<'\t'<< "ES" <<'\t' << "LS"<<'\t';
  for(k=0;k<R.size();k++) 
    cout<<"ri"<<k<<'\t';
  cout<<"Nbprec"<<'\t'<<"Succ"<<endl;
  for(i=0;i<A.size();i++) {
    cout<<""<<i<<'\t'<<A[i].p<<'\t'<<A[i].tw.t1 << '\t' << A[i].tw.t2<<'\t';
    for(k=0;k<R.size();k++) 
      cout<<A[i].b[k]<<'\t';
    cout<<A[i].succ.size()<<'\t';
    for(j=0;j<A[i].succ.size();j++) 
      cout<<A[i].succ[j]<<'\t';
    cout<<endl;
  }
  cout<<"B[k] = "<<'\t';
  for(i=0;i<R.size();i++) 
    cout<<R[i].B<<'\t';
  cout<<endl;
  return 0;
}

int problem::timeWindows(std::ifstream& fileTW){
  int i,ES,LS;
  fileTW.ignore(10,'\n');
  while (!fileTW.eof()){
    fileTW >> i >> ES >> LS;
    A[i].tw=interval(ES,LS);
  }
  return 0;
}
