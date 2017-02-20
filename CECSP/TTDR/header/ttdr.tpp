#include "ttdr.h"
#include <algorithm>
#include <limits.h>


template<>
int restrictiveLeft(Problem<int>& P);

template<typename type>
int restrictiveLeft(Problem<type>& P){
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) <= P.smax(i)){
      for (int j=0;j<P.nbTask;++j){
	if (P.bmin(j)!=0){
	  int emax=P.r(j)+P.W(j)/P.A[j].Fi(P.bmin(j));
	  if (j!=i && P.bmin(i)+P.bmin(j)>P.B && P.emin(i)>P.r(j) && P.smax(i)< emax)
	    P.A[j].ri = std::max(P.r(j),P.emin(i));
	}
      }
    }
  }
  return 0;
}


template<>
int restrictiveRight(Problem<int> &P);

template<typename type>
int restrictiveRight(Problem<type> &P){
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) <= P.smax(i)){
      for (int j=0;j<P.nbTask;++j){
	if (P.bmin(j)!=0){
	  int smin=P.d(j)-P.W(j)/P.A[j].Fi(P.bmin(j));
	  if (j!=i && P.bmin(i)+P.bmin(j)>P.B && P.emin(i)> smin && P.smax(i) < P.d(j))
	    P.A[j].di = std::min(P.d(j),P.smax(i));
	}
      }
    }
  }
  return 0;
}

template<typename type>
int restrictive_DR(Problem<type> &P){
  return (restrictiveRight(P) || restrictiveLeft(P));
}

template<typename type>
bool operator <(const mytuple<type>& l,const mytuple<type>& r){
  return (std::get<1>(l) < std::get<1>(r));
}

template<typename type>
int computeTimeTable(const Problem<type> &P, std::vector<fonction<type>>& TT){
  std::vector<mytuple<type>> timePoint;
  //timePoint.reserve(2*P.nbTask);
  int i;
  uint j;
  for (i=0;i<P.nbTask;++i){
    if (P.smax(i) < P.emin(i)) { 
      timePoint.push_back(std::make_tuple(i,P.smax(i), P.bmin(i)));
      timePoint.push_back(std::make_tuple(i,P.emin(i),-P.bmin(i)));
    }
  }
  std::sort(timePoint.begin(),timePoint.end());
  
  if (timePoint.size() > 0 )
    TT[0]=std::make_tuple(std::get<1>(timePoint[0]),std::get<2>(timePoint[0]));
  for (j=1;j<timePoint.size();++j){
    TT[j]=std::make_tuple(std::get<1>(timePoint[j]),std::get<1>(TT[j-1])+std::get<2>(timePoint[j]));
  }
  return 0;
}


template<typename type>
int timeTableCheck(const Problem<type>& P,std::vector<fonction<type>>& TT){
  for (uint i=0;i<TT.size(); ++i)
    if (std::get<1>(TT[i])  - P.B > POSITIVE_ZERO) return 0;
  return 1;
}

template<>
int consumption(const Problem<int> &P,int i, std::vector<fonction<int>> TT);

template<typename type>
type consumption(const Problem<type> &P,int i, std::vector<fonction<type>> TT){
  type min=std::numeric_limits<type>::max();
  uint j=0;
  uint H=TT.size();
  //init min
  if (H>0){
   if (std::get<0>(TT[0]) >= P.emin(i) - POSITIVE_ZERO && 
       std::get<1>(TT[0]) < min )     min = 0.0;
     
   while (j<H&&  std::get<0>(TT[j]) <=P.smax(i)){
     if (std::get<0>(TT[j]) >= P.emin(i) - POSITIVE_ZERO && 
	 std::get<1>(TT[j]) < min )
       min = std::get<1>(TT[j]);
     ++j;
  }

   if (std::get<0>(TT[H-1]) <= P.emin(i) - POSITIVE_ZERO && 
       std::get<1>(TT[H-1]) < min )     min = 0.0;
  }
  return min;
}


template<>
int computeSet(const Problem<int>& P,Problem<int>& Pf, std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed);


template<typename type>
int computeSet(const Problem<type>& P,Problem<type>& Pf, std::vector<fonction<type>>& TT, std::vector<type>& pushing,std::vector<type>&pushed){
  type D = std::numeric_limits<type>::min();
  type G= std::numeric_limits<type>::min();
  int i;
  std::vector<type> pushing_s;
  for (i=0;i<P.nbTask;++i){
    D=std::max(D,Pf.W(i)/Pf.A[i].Fi(Pf.bmin(i)));//peut etre bmax
    G=std::max(G,Pf.bmin(i));
  }
  for (i=0;i<P.nbTask;++i){
    if (Pf.smax(i)-(Pf.emin(i)-POSITIVE_ZERO) <=D )
      pushing_s.push_back(i);
  }
  for (uint j=0;j<pushing_s.size();++j){
    type gap=P.B-P.bmin(j)-consumption(P,j,TT);
    if (gap - POSITIVE_ZERO <= G )
      pushing.push_back(j);
  }
  type S=std::numeric_limits<type>::max();
  type E=std::numeric_limits<type>::max();
  for (uint j=0;j<pushing.size();++j){
    S=std::min(S,Pf.smax(j));
    E=std::min(E,Pf.emin(j));
  }
  for (i=0;i<P.nbTask;++i){
    if (Pf.r(i) - POSITIVE_ZERO < E && S < Pf.emin(i)+POSITIVE_ZERO)
      pushed.push_back(i);
  }
  return 0;
}

template<typename type> 
int computeFreepart(const Problem<type>& P, Problem<type>& Pf){
  Pf=P;
  for (int i=0;i<P.nbTask;++i){
    if (P.emin(i) + POSITIVE_ZERO > P.smax(i)) 
      Pf.A[i].Wi=std::max(0.0,P.W(i)-P.A[i].Fi(P.bmax(i))*(P.emin(i)-P.smax(i)));
    Pf.A[i].emin=Pf.r(i)+Pf.W(i)/Pf.A[i].Fi(Pf.bmax(i));
    Pf.A[i].smax=Pf.d(i)-Pf.W(i)/Pf.A[i].Fi(Pf.bmax(i)); 
  }
  return 0;
}

template<> 
int computeFreepart(const Problem<int>& P, Problem<int>& Pf);

template<>
int TTDRLeft(Problem<int> &P,Problem<int>& Pf,std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed);

template<typename type>
int TTDRLeft(Problem<type> &P,Problem<type>& Pf,std::vector<fonction<type>>& TT, std::vector<type>& pushing,std::vector<type>&pushed){
  for (uint i=0 ; i<pushing.size() ; ++i){
    type gap=P.B-P.bmin(pushing[i])-consumption(Pf,pushing[i],TT);
    for (uint j=0;j<pushed.size();++j){
      if (Pf.bmin(pushed[j])!=0){
	type emax=Pf.r(pushed[j])+Pf.W(pushed[j])/Pf.A[pushed[j]].Fi(Pf.bmin(pushed[j]));
	if (pushed[j]!=pushing[i] && Pf.emin(pushing[i])-Pf.r(pushed[j])>POSITIVE_ZERO && Pf.smax(pushing[i]) - emax< NEGATIVE_ZERO && Pf.bmin(pushed[j])-gap > POSITIVE_ZERO){

	  P.A[pushed[j]].ri= std::max(P.r(pushed[j]),Pf.emin(pushing[i]));
	  return 1;
	}
      }
    }
  }
  return 0;
}


template<>
int TTDRRight(Problem<int> &P,Problem<int>& Pf,std::vector<fonction<int>>& TT, std::vector<int>& pushing,std::vector<int>&pushed);

template<typename type>
int TTDRRight(Problem<type> &P,Problem<type>& Pf,std::vector<fonction<type>>& TT, std::vector<type>& pushing,std::vector<type>&pushed){
  
  //std::cout << "tentavit ajust\n";
  for (uint i=0 ; i<pushing.size() ; ++i){
    type gap=P.B-P.bmin(pushing[i])-consumption(Pf,pushing[i],TT);
    for (uint j=0;j<pushed.size();++j){
      if (Pf.bmin(pushed[j])!=0){
	type smin=Pf.d(pushed[j])-Pf.W(pushed[j])/Pf.A[pushed[j]].Fi(Pf.bmin(pushed[j]));
	if (pushed[j]!=pushing[i] && Pf.emin(pushing[i]) -smin>POSITIVE_ZERO && Pf.smax(pushing[i])- Pf.d(pushed[j]) < NEGATIVE_ZERO && Pf.bmin(pushed[j]) -gap >POSITIVE_ZERO)  {
	  /* std:: cout << "ajust Right "<<pushed[j]<< " " <<pushing[i]<< " " << 
	     Pf.emin(pushing[i]) << " " <<smin << " " <<Pf.smax(pushing[i]) << 
	     " " << Pf.d(pushed[j]) << " " << Pf.bmin(pushed[j])  << " " << gap<< 
	     " " <<P.A[pushed[j]].di << "\n";*/
	  P.A[pushed[j]].di= std::min(P.d(pushed[j]),Pf.smax(pushing[i]));
	  return 1;
	}
      }
    }
  }
  return 0;
}

template<typename type>
int TTDR(Problem<type> &P){
  Problem<type> Pf(P.nbTask);
  computeFreepart(P,Pf);
  std::vector<fonction<type>> TT;
  TT.resize(2*P.nbTask);
  computeTimeTable(P,TT);
  std::vector<type> pushing,pushed;
  computeSet(P,Pf,TT,pushing,pushed);
  if (timeTableCheck(P,TT)){
    if (TTDRLeft(P,Pf,TT,pushing,pushed) || TTDRRight(P,Pf,TT,pushing,pushed))
      return 2;
    else 
      return 1;
  }
  return 0;
}








