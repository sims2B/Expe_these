#include "ttdr.h"
#include <algorithm>
#include <limits.h>


template<>
int restrictiveLeft(Problem<int>& P){
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) <= P.smax(i)){
      for (int j=0;j<P.nbTask;++j){
	if (P.bmin(j)!=0){
	  int emax=(int) ceil(P.r(j)+P.W(j)/(double)(P.A[j].Fi(P.bmin(j))));
	  if (j!=i && P.bmin(i)+P.bmin(j)>P.B && P.emin(i)-1>=P.r(j) && P.smax(i)< emax)
	    P.A[j].ri = std::max(P.r(j),P.emin(i));
	}
      }
    }
  }
  return 0;
}

template<>
int restrictiveRight(Problem<int> &P){
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) <= P.smax(i)){
      for (int j=0;j<P.nbTask;++j){
	if (P.bmin(j)!=0){
	  int smin=(int) ceil(P.d(j)-P.W(j)/(double)(P.A[j].Fi(P.bmin(j))));
	  if (j!=i && P.bmin(i)+P.bmin(j)>P.B && P.emin(i)-1 >= smin && P.smax(i) < P.d(j))
	    P.A[j].di = std::min(P.d(j),P.smax(i));
	}
      }
    }
  }
  return 0;
}



template<>
int consumption(const Problem<int> &P,int i, std::vector<fonction<int>> TT){
  int min=std::numeric_limits<int>::max();
  uint j=0;
  uint H=TT.size();
  //init min
  if (H>0){
  if (std::get<0>(TT[0]) >= P.emin(i) && 
      std::get<1>(TT[0]) < min )     min = 0;
  
  while (j<H &&  std::get<0>(TT[j]) <=P.smax(i)){
    if (std::get<0>(TT[j]) >= P.emin(i)  && 
	std::get<1>(TT[j]) < min )
      min = std::get<1>(TT[j]);
    ++j;
  }

  if (std::get<0>(TT[H-1]) <= P.emin(i) && 
      std::get<1>(TT[H-1]) < min )     min = 0;
  } return min;
 
}


template<>
int TTDRLeft(Problem<int> &P,Problem<int>& Pf,std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed){
  for (uint i=0 ; i<pushing.size() ; ++i){
    int gap=P.B-P.bmin(pushing[i])-consumption(Pf,pushing[i],TT);
    for (uint j=0;j<pushed.size();++j){
      if (Pf.bmin(pushed[j])!=0){
	int emax=(int)ceil(Pf.r(pushed[j])+Pf.W(pushed[j])/(double)Pf.A[pushed[j]].Fi(Pf.bmin(pushed[j])));
	if (pushed[j]!=pushing[i] && Pf.emin(pushing[i])-1>=Pf.r(pushed[j]) && Pf.smax(pushing[i])< emax && Pf.bmin(pushed[j]) > gap){
	  P.A[pushed[j]].ri= std::max(P.r(pushed[j]),Pf.emin(pushing[i]));
	  return 1;
	}
      }
    }
  }
  return 0;
}


template<>
int TTDRRight(Problem<int> &P,Problem<int>& Pf,std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed){
  for (uint i=0 ; i<pushing.size() ; ++i){
    int gap=P.B-P.bmin(pushing[i])-consumption(Pf,pushing[i],TT);
    for (uint j=0;j<pushed.size();++j){
      if (Pf.bmin(pushed[j])!=0){
	int smin=(int)ceil(Pf.d(pushed[j])-Pf.W(pushed[j])/(double)Pf.A[pushed[j]].Fi(Pf.bmin(pushed[j])));
	if (pushed[j]!=pushing[i] && Pf.emin(pushing[i])-1>=smin && Pf.smax(pushing[i])< Pf.d(pushed[j]) && Pf.bmin(pushed[j]) > gap)  {
	  P.A[pushed[j]].di= std::min(P.d(pushed[j]),Pf.smax(pushing[i]));
	  return 1;
	}
      }
    }
  }
  return 0;
}



template<>
int computeSet(const Problem<int>& P,Problem<int>& Pf, std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed){
  int D = std::numeric_limits<int>::min();
  int G= std::numeric_limits<int>::min();
  int i;
  std::vector<int> pushing_s;
  for (i=0;i<P.nbTask;++i){
    D=std::max(D,(int)ceil(Pf.W(i)/(double)Pf.A[i].Fi(Pf.bmin(i))));//peut etre bmax
    G=std::max(G,Pf.bmin(i));
  }
  for (i=0;i<P.nbTask;++i){
    if (Pf.smax(i)-Pf.emin(i) <=D )
    pushing_s.push_back(i);
  }
  for (uint j=0;j<pushing_s.size();++j){
    int gap=P.B-P.bmin(j)-consumption(P,j,TT);
    if (gap <=G )
    pushing.push_back(j);
  }
  int S=std::numeric_limits<int>::max();
  int E=std::numeric_limits<int>::max();
  for (uint j=0;j<pushing.size();++j){
    S=std::min(S,Pf.smax(j));
    E=std::min(E,Pf.emin(j));
  }
  for (i=0;i<P.nbTask;++i){
    if (Pf.r(i)<E && S< Pf.emin(i))
      pushed.push_back(i);
  }
  return 0;
}

template<> 
int computeFreepart(const Problem<int>& P, Problem<int>& Pf){
  Pf=P;
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) > P.smax(i)) 
      Pf.A[i].Wi=std::max(0,P.W(i)-P.A[i].Fi(P.bmax(i))*(P.emin(i)-P.smax(i)));
    Pf.A[i].emin=(int)ceil(Pf.r(i)+Pf.W(i)/(double)Pf.A[i].Fi(Pf.bmax(i)));
    Pf.A[i].smax=Pf.d(i)-Pf.W(i)/Pf.A[i].Fi(Pf.bmax(i));  
  }
  return 0;
}

template bool operator <(const mytuple<int>&,const mytuple<int>&);
template int restrictive_DR(Problem<int>&);
////////////////////////////
//////////TIME TABLE////////
///////////////////////////

template int computeTimeTable(const Problem<int>&, std::vector<fonction<int>>&);
template int timeTableCheck(const Problem<int>&,std::vector<fonction<int>>& TT);

template int TTDR(Problem<int>&);
