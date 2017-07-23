#ifndef DEF_LISTINTERVAL
#define DEF_LISTINTERVAL
#include <vector>
#include <cassert>
#include "Problem.hpp"


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type>
using IntervalList=std::vector<Interval<type>>;

//projection of point H and I on t1 or t2 axis
template<typename type,typename type2=type>
inline type Ht1(const Problem<type,type2> &P,int i);
template<typename type,typename type2=type>
inline type Ht2(const Problem<type,type2> &P,int i);
template<typename type,typename type2=type>	 
inline type It1(const Problem<type,type2> &P,int i) ;
template<typename type,typename type2=type> 
inline type It2(const Problem<type,type2> &P,int i);

//Compute bound U,U',D and D' (also known as delta, delta', gamma and gamma')
template<typename type,typename type2=type>
type Dp(const Problem<type,type2>&,int i,type val);
template<typename type,typename type2=type>
type U(const Problem<type,type2>&,int i,type val);
template<typename type,typename type2=type>
type D(const Problem<type,type2>&,int i,type val);
template<typename type,typename type2=type> 
type Up(const Problem<type,type2>&,int i,type val);


template<typename type,typename type2=type>
int interNul_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i);
template<typename type,typename type2=type>
int interNul_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i);
template<typename type,typename type2=type>
int interInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i);
template<typename type,typename type2=type>
int interInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i);
template<typename type,typename type2=type>
int interSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i);
template<typename type,typename type2=type>
int interSup_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i);


template<typename type,typename type2=type>
int computeCheckInterval(IntervalList<type>& list,const Problem<type,type2>& P);

template<typename type,typename type2=type>
int interLSInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i);
template<typename type,typename type2=type>
int interLSInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i);
template<typename type,typename type2=type>
int interLSSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i);
template<typename type,typename type2=type>
int interLSSup_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i);

template<typename type,typename type2=type>
int interRSInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i);
template<typename type,typename type2=type>
int interRSInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i);
template<typename type,typename type2=type>
int interRSSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i);
template<typename type,typename type2=type>
int interRSSup_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i);


//Pour faire tous les ajustement à gauche il faut aussi faire avec checkInterval!! et pareil à droite!
template<typename type,typename type2=type>
int computeLSInterval(IntervalList<type>& list,const Problem<type,type2>& P,int i);
template<typename type,typename type2=type>
int computeRSInterval(IntervalList<type>& list,const Problem<type,type2>& P,int i);

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////
template<typename type,typename type2>
inline type Ht1(const Problem<type,type2> &P,int i) {  
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),2*P.A[i].Fi(P.bmin(i))));
  return (P.r(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.d(i)*P.A[i].Fi(P.bmin(i))+P.W(i))
    /(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i)));
}

template<typename type,typename type2>
type Ht2(const Problem<type,type2> &P,int i) {
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),2*P.A[i].Fi(P.bmin(i))));
  return (P.d(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
    /(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i)));

}
int Ht2(const Problem<int,int> &P,int i);
int Ht2(const Problem<int,double> &P,int i);

template<typename type,typename type2>	 
inline type It1(const Problem<type,type2> &P,int i) { 
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),P.A[i].Fi(P.bmin(i))));
  return ((P.r(i)*P.A[i].Fi(P.bmax(i))-P.d(i)*P.A[i].Fi(P.bmin(i))+P.W(i))
	  /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));
}
	
template<typename type,typename type2> 
type It2(const Problem<type,type2> &P,int i) { 
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),P.A[i].Fi(P.bmin(i))));
  return ((P.d(i)*P.A[i].Fi(P.bmax(i))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
	  /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));
}
int It2(const Problem<int,int> &P,int i);
int It2(const Problem<int,double> &P,int i);


//gamma(t_1)
template<typename type,typename type2>
type U(const Problem<type,type2>& P,int i,type val){
  assert(!isEqual(P.bmin(i),(type2)0.0));
  return (P.W(i)+val*(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))+P.r(i)*P.A[i].Fi(P.bmax(i)))
    /P.A[i].Fi(P.bmin(i));
}


template<typename type,typename type2>
type Dp(const Problem<type,type2>& P,int i,type val){
  assert(!isEqual(P.bmin(i),(type2)0.0));
  return (val*(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))+ P.d(i)*P.A[i].Fi(P.bmax(i))-P.W(i))
    /P.A[i].Fi(P.bmin(i)); //bimin !=0 (car ce cas est traité a part)
}

//delta(t_1)
template<typename type,typename type2>
type D(const Problem<type,type2> &P,int i,type val){
  if (isEqual(P.bmin(i),P.bmax(i)))
    return P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i));
  else return (-P.W(i)- val*P.A[i].Fi(P.bmin(i))+P.d(i)*P.A[i].Fi(P.bmax(i)))
	 /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)));
}

template<typename type,typename type2>
type Up(const Problem<type,type2> &P,int i,type val){
  if (isEqual(P.bmin(i),P.bmax(i)))
    return P.d(i) - P.W(i) /P.A[i].Fi(P.bmax(i));
  else  return (P.W(i)-val*P.A[i].Fi(P.bmin(i))+P.r(i)*P.A[i].Fi(P.bmax(i)))
	  /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)));
}

template<typename type, typename type2>
type eet(const Problem<type,type2> &P, int i){
  return P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i));
}
int eet(const Problem<int,int> &P, int i);
int eet(const Problem<int,double> &P, int i);

template<typename type, typename type2>
type lst(const Problem<type,type2> &P, int i){
  return P.d(i) - P.W(i)/P.A[i].Fi(P.bmax(i));
}
int lst(const Problem<int,int> &P, int i);
int lst(const Problem<int,double> &P, int i);

template<typename type,typename type2>
int interNul_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO < eet(P,i))
    list.push_back(Interval<type>(t1,P.d(i)));
  return 0;
} 

template<typename type,typename type2>
int interNul_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i){
  if (t2  > lst(P,i) + NEGATIVE_ZERO)
    list.push_back(Interval<type>(P.r(i),t2));
  return 0;
} 


template<typename type,typename type2>
int interInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO <= P.r(i))
    list.push_back(Interval<type>(t1,P.d(i)));
  else if (t1 +NEGATIVE_ZERO <= eet(P,i)){
    if (eet(P,i) +NEGATIVE_ZERO <= lst(P,i)){  
      if (P.r(i)+P.d(i) - t1  > t1 +NEGATIVE_ZERO)
	list.push_back(Interval<type>(t1,P.r(i)+P.d(i)-t1));
    }
    else {
      if (t1 + NEGATIVE_ZERO <= Ht1(P,i)){
	if (P.r(i)+P.d(i) - t1  > t1 +NEGATIVE_ZERO)
	  list.push_back(Interval<type>(t1,P.r(i)+P.d(i)-t1));
      }
      else if (t1 + NEGATIVE_ZERO >=  lst(P,i)) {
	if (t1 +NEGATIVE_ZERO < U(P,i,t1))
	  list.push_back(Interval<type>(t1,U(P,i,t1))); 
      }
      else if (t1 +NEGATIVE_ZERO > Ht1(P,i) && eet(P,i) +POSITIVE_ZERO < lst(P,i) ){	
	if (t1 +NEGATIVE_ZERO<U(P,i,t1))
	  list.push_back(Interval<type>(t1,U(P,i,t1)));
	if (t1 +NEGATIVE_ZERO<D(P,i,t1)) 
	  list.push_back(Interval<type>(t1,D(P,i,t1)));
      }
    }
  }
  return 0;
} 
  
template<typename type,typename type2>
int interInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i){
  if (t2 + POSITIVE_ZERO >= P.d(i))
    list.push_back(Interval<type>(P.r(i),t2));
  else if (t2 + POSITIVE_ZERO >= lst(P,i)){
    if (eet(P,i) + NEGATIVE_ZERO <= lst(P,i)) {
      if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	list.push_back(Interval<type>(P.r(i)+P.d(i)-t2,t2));
    }
    else{
      if (t2 +POSITIVE_ZERO >= Ht2(P,i)){
	if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	  list.push_back(Interval<type>(P.r(i)+P.d(i)-t2,t2));
      }
      else if (t2 + POSITIVE_ZERO <= eet(P,i)) {
	if (Up(P,i,t2) +POSITIVE_ZERO<t2)
	  list.push_back(Interval<type>(Up(P,i,t2),t2)); 
      }
      else if (eet(P,i) + NEGATIVE_ZERO < lst(P,i) && t2 +POSITIVE_ZERO < Ht2(P,i)) {
	if (Up(P,i,t2) +POSITIVE_ZERO<t2)
	  list.push_back(Interval<type>(Up(P,i,t2),t2));
	if (Dp(P,i,t2) +POSITIVE_ZERO<t2)
	  list.push_back(Interval<type>(Dp(P,i,t2),t2));
      }
    }
  }
  return 0;
}
  

template<typename type,typename type2>
int interSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO <= P.r(i))
    list.push_back(Interval<type>(t1,P.d(i)));
  else if (t1 +NEGATIVE_ZERO <= eet(P,i)){
    if (t1 +NEGATIVE_ZERO <= It1(P,i)){
      if (P.d(i) > t1 + POSITIVE_ZERO)
	list.push_back(Interval<type>(t1,P.d(i)));
      if (t1 +POSITIVE_ZERO<D(P,i,t1))
	list.push_back(Interval<type>(t1,D(P,i,t1)));
    }
    else if (eet(P,i)+POSITIVE_ZERO < lst(P,i)){
      if (t1 + NEGATIVE_ZERO >= Ht1(P,i)){
	if (P.r(i) +P.d(i) -t1 +NEGATIVE_ZERO>t1)
	  list.push_back(Interval<type>(t1,P.r(i)+P.d(i)-t1));
      }
    }
    else if (t1 + NEGATIVE_ZERO >= lst(P,i)) {
      if (t1 +POSITIVE_ZERO<U(P,i,t1))
	list.push_back(Interval<type>(t1,U(P,i,t1))); 
    }
    else {
      if (t1 +POSITIVE_ZERO<U(P,i,t1))
	list.push_back(Interval<type>(t1,U(P,i,t1))); 
      if (t1 +POSITIVE_ZERO<D(P,i,t1))
	list.push_back(Interval<type>(t1,D(P,i,t1)));
    }
  }
  return 0;
} 


template<typename type,typename type2>
int interSup_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2,int i){
  if (t2 + POSITIVE_ZERO >= P.d(i))
    list.push_back(Interval<type>(P.d(i),t2));
  else if (t2 + POSITIVE_ZERO >= lst(P,i)){
    if (t2 + POSITIVE_ZERO >= It2(P,i)){
      if (t2 +NEGATIVE_ZERO > P.d(i))
	list.push_back(Interval<type>(P.d(i),t2));
      if (t2 +NEGATIVE_ZERO > Dp(P,i,t2))
	list.push_back(Interval<type>(Dp(P,i,t2),t2));
    }
    else if (lst(P,i) + NEGATIVE_ZERO > eet(P,i)){
      if (t2 + NEGATIVE_ZERO <= Ht2(P,i)){
	if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	  list.push_back(Interval<type>(P.r(i)+P.d(i)-t2,t2));
      }
    }
    else if (t2 + NEGATIVE_ZERO <= eet(P,i)) {
      if (t2 +NEGATIVE_ZERO > Up(P,i,t2))
	list.push_back(Interval<type>(Up(P,i,t2),t2));
    }
    else{
      if (t2 +NEGATIVE_ZERO > Up(P,i,t2))
	list.push_back(Interval<type>(Up(P,i,t2),t2)); 
      if (t2 +NEGATIVE_ZERO > Dp(P,i,t2))
	list.push_back(Interval<type>(Dp(P,i,t2),t2));
    }
  }
  return 0;
} 


template<typename type,typename type2>
int computeCheckInterval(IntervalList<type>& list,const Problem<type,type2>& P){
  std::vector<type> t1;
  t1.reserve(2*P.nbTask);
  for (int i=0;i<P.nbTask;++i) {
    t1.push_back(P.r(i));
    if (isEqual(P.bmin(i),P.bmax(i)))
      t1.push_back(eet(P,i));
  }
  for (uint t=0; t< t1.size();++t){
    for (int i=0;i<P.nbTask;++i) {
      if (isEqual(P.bmin(i),0.0))
	interNul_t2(list,P,t1[t],i); 
      else if (P.W(i) + NEGATIVE_ZERO <= P.A[i].Fi(P.bmin(i))*(P.d(i)-P.r(i)))
	interInf_t2(list,P,t1[t],i);
      else if (P.W(i) - POSITIVE_ZERO > P.A[i].Fi(P.bmin(i))*(P.d(i)-P.r(i)))
	interSup_t2(list,P,t1[t],i);
    }
  }
  t1.clear();
  for (int i=0;i<P.nbTask;++i) {
    t1.push_back(P.d(i));
    if (isEqual(P.bmin(i),P.bmax(i)))
      t1.push_back(lst(P,i));
  }
  for (uint t=0; t< t1.size();++t){
    for (int i=0;i<P.nbTask;++i) {
      if (isEqual(P.bmin(i),0.0))
	interNul_t1(list,P,t1[t],i); 
      else if (P.W(i) + NEGATIVE_ZERO <= P.A[i].Fi(P.bmin(i))*(P.d(i)-P.r(i)))
	interInf_t1(list,P,t1[t],i);
      else if (P.W(i) - POSITIVE_ZERO > P.A[i].Fi(P.bmin(i))*(P.d(i)-P.r(i)))
	interSup_t1(list,P,t1[t],i);
    }
  }
  return 0;
}


template<typename type,typename type2>
int interLSInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i){
  if ( t1 + NEGATIVE_ZERO <= P.r(i) )
    list.push_back(Interval<type>(t1,P.d(i)));
  else if (eet(P,i) > t1 + POSITIVE_ZERO )
    list.push_back(Interval<type>(t1,U(P,i,t1)));
  return 0;
}

template<typename type, typename type2>
type Emax(const Problem<type,type2> &P, int i){
  return P.r(i) + P.W(i) / P.A[i].Fi(P.bmin(i));
}
int Emax(const Problem<int,int> &P, int i);
int Emax(const Problem<int,double> &P, int i);

template<typename type,typename type2>
int interLSInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i){
  if ( t2 > P.r(i) + NEGATIVE_ZERO)
    list.push_back(Interval<type>(P.r(i),t2));
  if (t2 +NEGATIVE_ZERO < Emax(P,i) && t2 + NEGATIVE_ZERO > eet(P,i))
    list.push_back(Interval<type>(Up(P,i,t2),t2));
  return 0;
}

template<typename type,typename type2>
int interLSSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i){
  if ( t1 + NEGATIVE_ZERO <= It1(P,i) )
    list.push_back(Interval<type>(t1, P.d(i))); 
  else if ( t1 + POSITIVE_ZERO < eet(P,i))
    list.push_back(Interval<type>(t1,U(P,i,t1)));
  return 0;
}


template<typename type,typename type2>
int interLSSup_t1(IntervalList<type>& list, const Problem<type,type2> &P, type t2, int i){
  if ( t2 > P.r(i)  + POSITIVE_ZERO  )
    list.push_back(Interval<type>(P.r(i),t2));
  if ( t2 +NEGATIVE_ZERO > eet(P,i) && t2 + POSITIVE_ZERO < P.d(i))
    list.push_back(Interval<type>(Up(P,i,t2),t2));
  return 0;
}
template<typename type, typename type2>
type Smin(const Problem<type,type2> &P, int i){
  return P.d(i) - P.W(i) / P.A[i].Fi(P.bmin(i));
}
int Smin(const Problem<int,int> &P, int i);
int Smin(const Problem<int,double> &P, int i);


template<typename type,typename type2>
int interRSInf_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i){
  if ( t1 + NEGATIVE_ZERO < P.d(i))
    list.push_back(Interval<type>(t1,P.d(i)));
  if (t1 + POSITIVE_ZERO < lst(P,i) && t1 + POSITIVE_ZERO > Smin(P,i))
  list.push_back(Interval<type>(t1,D(P,i,t1)));
return 0;
}

template<typename type,typename type2>
int interRSInf_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i){
  if (t2 >= P.d(i) + NEGATIVE_ZERO)
    list.push_back(Interval<type>(P.r(i),t2));
  else if (t2 + NEGATIVE_ZERO > lst(P,i))
    list.push_back(Interval<type>(Dp(P,i,t2),t2));
  return 0;
}

template<typename type,typename type2>
int interRSSup_t2(IntervalList<type>& list, const Problem<type,type2> &P,type t1, int i){
  if ( t1 + POSITIVE_ZERO < P.d(i) )
    list.push_back(Interval<type>(t1,P.d(i)));
  if (t1 + NEGATIVE_ZERO < lst(P,i) && t1 + POSITIVE_ZERO > P.r(i))
    list.push_back(Interval<type>(t1,D(P,i,t1)));
  return 0;
}

template<typename type,typename type2>
int interRSSup_t1(IntervalList<type>& list, const Problem<type,type2> &P,type t2, int i){
  if (t2 >= It2(P,i) + NEGATIVE_ZERO)
    list.push_back(Interval<type>(P.r(i),t2));
  else if (t2 + NEGATIVE_ZERO > lst(P,i) )
    list.push_back(Interval<type>(Dp(P,i,t2),t2));
  return 0;
}

template<typename type,typename type2>
int computeLSInterval(IntervalList<type>& list,const Problem<type,type2>& P,int i){
  //LS x LS
  for (int j=0;j <P.nbTask;++j){
    if (P.bmin(j) != 0){
      if (P.W(j) + NEGATIVE_ZERO <=P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interLSInf_t2(list,P,P.r(i),j);
	interLSInf_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interLSInf_t2(list,P,lst(P,i),j);
	  interLSInf_t1(list,P,eet(P,i),j);
	}
      }
      else if (P.W(j) + POSITIVE_ZERO  > P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interLSSup_t2(list,P,P.r(i),j);
	interLSSup_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interLSSup_t2(list,P,lst(P,i),j);
	  interLSSup_t2(list,P,eet(P,i),j);
	}
      }
    }
  }
  return 0;
}

template<typename type,typename type2>
int computeRSInterval(IntervalList<type>& list,const Problem<type,type2>& P,int i){
  //RS x RS
  for (int j=0;j <P.nbTask;++j){
    if (P.bmin(j) != 0){
      if (P.W(j) + NEGATIVE_ZERO <=P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interRSInf_t2(list,P,P.r(i),j);
	interRSInf_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interRSInf_t2(list,P,lst(P,i),j);
	  interRSInf_t1(list,P,eet(P,i),j);
	}
      }
      else if (P.W(j) + POSITIVE_ZERO  > P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interRSSup_t2(list,P,P.r(i),j);
	interRSSup_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interRSSup_t2(list,P,lst(P,i),j);
	  interRSSup_t2(list,P,eet(P,i),j);
	}
      }
    }
  }
  return 0;
}


template<typename type>
int printListInterval(IntervalList<type>& list){
  for (uint i=0;i<list.size();++i){
    std::cout << list[i].t1 << " " << list[i].t2 << "\n";
  }
  return 0;
}

#endif
