#ifndef DEF_SOLUTION
#define DEF_SOLUTION

#include "Problem.hpp"
#include <algorithm>
#include <vector>
// !!!!! instantaneous resource consumption

///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

//struct solution defined by to type (the first one stand for the start 
// and end time, the second for the resource consumption)
// contains a vector of start time, a vector of end time, "n" vectors of resource
// consumption and a vector of events
template<typename type,typename type2=type>
struct Solution{
  //construct a vector of start time of size nbTask initialize with value 0, a 
  //vector of end time of size nbTask initialize with value 0, nbTask vector 
  //of size nbSlot initialize with value 0 , a vector of event of capacity nbSlot
  Solution(int,int);
  //empty all vector of the solution (use popback)
  void clear();
  //resize all vector to their original size (typically used after a call to clear())
  void reAlloc(int,int);

  void displaySolution();
  //check the validity of the solution
  int isValid(const Problem<type> &) const;
	
  std::vector<type> st;
  std::vector<type> ft;
  std::vector<type> event;
  std::vector<std::vector<type2>> b;
};

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type,typename type2>
Solution<type,type2>::Solution(int nbTask,int nbSlot){
  st.resize(nbTask);
  ft.resize(nbTask);
  event.reserve(nbSlot);
  b.resize(nbTask);
  for (int i=0;i<nbTask;++i)
    b[i].resize(nbSlot,0);
}


template<typename type,typename type2> 
void Solution<type,type2>::clear(){
  const uint n=st.size();
  const int E=event.size();
  uint e,i;
  for (i=0;i<n;++i){
    st.pop_back();
    ft.pop_back();
    for (e=0;e<E;++e)
      b[i].pop_back();
  }
  for (e=0;e<E;++e)
    event.pop_back();
}

template<typename type,typename type2>
void Solution<type,type2>::reAlloc(int nbTask,int nbSlot){
  st.resize(nbTask);
  ft.resize(nbTask);
  event.resize(nbSlot);
  b.resize(nbTask);
  for (int i=0;i<nbTask;++i)
    b[i].resize(nbSlot-1);
}

template<typename type,typename type2>
void Solution<type,type2>::displaySolution(){
  std::cout << "sol vaut : " << std::endl;
  for (uint i=0;i<st.size();++i){
    std::cout << "st["<<i<<"]="<<st[i]<< std::endl;
      std::cout << "ft["<<i<<"]="<<ft[i]<< std::endl;
      std::cout << "b["<<i<<"] =(";
      for(unsigned int t=0;t<b[i].size();++t){
	std::cout << b[i][t] << "," ;
      }
      std::cout <<")"<< std::endl;
  }
  std::cout << "event= ";
    for (unsigned int i=0;i<event.size();++i)
    std::cout <<event[i]<< ", ";
  
  std::cout <<  std::endl;
}


template<typename type,typename type2> 
int Solution<type,type2>::isValid(const Problem<type> &P) const{
  const int nbTask=P.nbTask;
  const int E=event.size();
  int e,i;
  type sum=0.0;
  type energy=0.0;
  //time window constraints
  for (i=0;i<nbTask;++i) {
    if (st[i] < P.r(i) + NEGATIVE_ZERO) return 0;
    if (ft[i] > P.d(i) + POSITIVE_ZERO) return 0;
  }

  // b_ie = 0 if the task is not in process
  for (i=0;i<nbTask;++i) {
    for (e=0;e<E-1;++e){
      if (event[e] < st[i] + NEGATIVE_ZERO || event[e] >= ft[i] + POSITIVE_ZERO){
	if (b[i][e] > POSITIVE_ZERO && P.bmin(i)!=0.0) return 0;
      }
    }
  }

  //energy and max/min consumption
  for (i=0;i<nbTask;++i) {
    energy=0.0; 
    for (e=0;e<E-1;++e){
      //max/min consumption
      if (!isEqual(event[e+1],event[e]) && 
	  (b[i][e] < P.bmin(i) + NEGATIVE_ZERO || 
	   b[i][e] > P.bmax(i) + POSITIVE_ZERO))
	return 0;
      //energy 
      if (!isEqual(b[i][e],0.0))
	energy+=(P.A[i].Fi.F[0].f.a*b[i][e]+P.A[i].Fi.F[0].f.c)*(event[e+1]-event[e]);
    }
    if (energy < P.W(i) + NEGATIVE_ZERO) return 0;	 
  }
  
  //cumulative constraint
  for (e=0;e<E-1;++e) {
    sum=0.0;
    for (i=0;i<nbTask;++i)
      sum+=b[i][e];
    if (sum > P.B + POSITIVE_ZERO) return 0;
  }
  return 1;
}


template<> int Solution<int,double>::isValid(const Problem<int> &P) const;

template<> int Solution<int,int>::isValid(const Problem<int> &P) const;


#endif

