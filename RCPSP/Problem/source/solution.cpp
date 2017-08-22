#include "solution.h"

solution::solution(int nbTask) {st.resize(nbTask);}


int solution::printSol(){
  std::cout << "sol vaut : " << std::endl;
  for (uint i=0;i<st.size();++i)
    std::cout << "st["<<i<<"]="<<st[i]<< std::endl;
  return 0;
}


int solution::isValid(const problem &P) const{
  uint i,j,k;
  uint n=P.nbTask;

  for (i=0;i<n;++i){
    for (j=0;j<P.A[i].succ.size();++j)
      if (st[P.A[i].succ[j]]-st[i]<P.A[i].p) return 0;
  }
 
  type conso=0;
  for (k=0;k<P.R.size();++k){
    for (i=0;i<n;++i){
      conso=0;
      for (j=0;j<n;++j)
	conso =(st[j]<=st[i] && st[i]<st[j]+P.A[j].p) ? 
	  conso+P.A[j].b[k] : conso;
      if (conso > P.R[k].B) return 0;
    }      
  }
  return 1;
}
  
void solution::clear(){
  const int n=st.size();
  int i;
  for (i=0;i<n;++i)
    st.pop_back();
}


void solution::reAlloc(int nbTask){
  st.resize(nbTask);
}
