#include "ListInterval.h"
#include <vector>
#include <cassert>

template<typename type>
int push_backBound(IntervalList<type>& list,type t1,type t2){
  list.push_back(Interval<type>(std::max(0.0,t1),t2));
  return 0;
}

//gamma(t_1)
template<typename type>
type U(const Problem<type>& P,int i,type val){
  assert(P.bmin(i)!=0.0);
  return (P.W(i)+val*(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))+P.r(i)*P.A[i].Fi(P.bmax(i)))
    /P.A[i].Fi(P.bmin(i));
}


template<typename type>
type Dp(const Problem<type>& P,int i,type val){
  assert(P.bmin(i)!=0.0);
  return (val*(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))+ P.d(i)*P.A[i].Fi(P.bmax(i))-P.W(i))
    /P.A[i].Fi(P.bmin(i)); //bimin !=0 (car ce cas est traité a part)
}


//delta(t_1)
template<typename type>
type D(const Problem<type> &P,int i,type val){
  if (P.bmin(i)==P.bmax(i))
    return P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i));
  else return (-P.W(i)- val*P.A[i].Fi(P.bmin(i))+P.d(i)*P.A[i].Fi(P.bmax(i)))
	 /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)));
}

template<typename type>
type Up(const Problem<type> &P,int i,type val){
  if (P.bmin(i)==P.bmax(i))
    return P.d(i) - P.W(i) /P.A[i].Fi(P.bmax(i));
  else  return (P.W(i)-val*P.A[i].Fi(P.bmin(i))+P.r(i)*P.A[i].Fi(P.bmax(i)))
	  /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)));
}

template<typename type>
int interNul_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO < P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)))
    push_backBound(list,t1,P.d(i));
  return 0;
} 

template<typename type>
int interNul_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i){
  if (t2  > P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) + NEGATIVE_ZERO)
    push_backBound(list,P.r(i),t2);
  return 0;
} 


template<typename type>
int interInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO <= P.r(i))
    push_backBound(list,t1,P.d(i));
  else if (t1 +NEGATIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))){
    if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) +NEGATIVE_ZERO <= P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))){  
      if (P.r(i)+P.d(i) - t1  > t1 +NEGATIVE_ZERO)
	push_backBound(list, t1,P.r(i)+P.d(i)-t1);
    }
    else {
      if (t1 + NEGATIVE_ZERO <= Ht1(P,i)){
	if (P.r(i)+P.d(i) - t1  > t1 +NEGATIVE_ZERO)
	  push_backBound(list, t1,P.r(i)+P.d(i)-t1);
      }
      else if (t1 + NEGATIVE_ZERO >=  P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))) {
	if (t1 +NEGATIVE_ZERO < U(P,i,t1))
	  push_backBound(list,t1,U(P,i,t1)); 
      }
      else if (t1 +NEGATIVE_ZERO > Ht1(P,i) && P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) +POSITIVE_ZERO < P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) ){	
	if (t1 +NEGATIVE_ZERO<U(P,i,t1))
	  push_backBound(list,t1,U(P,i,t1));
	if (t1 +NEGATIVE_ZERO<D(P,i,t1)) 
	  push_backBound(list,t1,D(P,i,t1));
      }
    }
  }
  return 0;
} 
  
template<typename type>
int interInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i){
  if (t2 + POSITIVE_ZERO >= P.d(i))
    push_backBound(list,P.r(i),t2);
  else if (t2 + POSITIVE_ZERO >= P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))){
    if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) + NEGATIVE_ZERO <= P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))) {
      if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	push_backBound(list, P.r(i)+P.d(i)-t2,t2);
    }
    else{
      if (t2 +POSITIVE_ZERO >= Ht2(P,i)){
	if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	  push_backBound(list, P.r(i)+P.d(i)-t2,t2);
      }
      else if (t2 + POSITIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))) {
	if (Up(P,i,t2) +POSITIVE_ZERO<t2)
	  push_backBound(list,Up(P,i,t2),t2); 
      }
      else if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) + NEGATIVE_ZERO < P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) && t2 +POSITIVE_ZERO < Ht2(P,i)) {
	if (Up(P,i,t2) +POSITIVE_ZERO<t2)
	  push_backBound(list,Up(P,i,t2),t2);
	if (Dp(P,i,t2) +POSITIVE_ZERO<t2)
	  push_backBound(list,Dp(P,i,t2),t2);
      }
    }
  }
  return 0;
}
  

template<typename type>
int interSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i){
  if (t1 + NEGATIVE_ZERO <= P.r(i))
    push_backBound(list,t1,P.d(i));
  else if (t1 +NEGATIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))){
    if (t1 +NEGATIVE_ZERO <= It1(P,i)){
      if (P.d(i) > t1 + POSITIVE_ZERO)
	push_backBound(list,t1,P.d(i));
      if (t1 +POSITIVE_ZERO<D(P,i,t1))
	push_backBound(list,t1,D(P,i,t1));
    }
    else if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))+POSITIVE_ZERO < P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))){
      if (t1 + NEGATIVE_ZERO >= Ht1(P,i)){
	if (P.r(i) +P.d(i) -t1 +NEGATIVE_ZERO>t1)
	  push_backBound(list, t1,P.r(i)+P.d(i)-t1);
      }
    }
    else if (t1 + NEGATIVE_ZERO >= P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))) {
      if (t1 +POSITIVE_ZERO<U(P,i,t1))
	push_backBound(list,t1,U(P,i,t1)); 
    }
    else {
      if (t1 +POSITIVE_ZERO<U(P,i,t1))
	push_backBound(list,t1,U(P,i,t1)); 
      if (t1 +POSITIVE_ZERO<D(P,i,t1))
	push_backBound(list,t1,D(P,i,t1));
    }
  }
  return 0;
} 


template<typename type>
int interSup_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i){
  if (t2 + POSITIVE_ZERO >= P.d(i))
    push_backBound(list,P.d(i),t2);
  else if (t2 + POSITIVE_ZERO >= P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i))){
    if (t2 + POSITIVE_ZERO >= It2(P,i)){
      if (t2 +NEGATIVE_ZERO > P.d(i))
	push_backBound(list,P.d(i),t2);
      if (t2 +NEGATIVE_ZERO > Dp(P,i,t2))
	push_backBound(list,Dp(P,i,t2),t2);
    }
    else if (P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) + NEGATIVE_ZERO > P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))){
      if (t2 + NEGATIVE_ZERO <= Ht2(P,i)){
	if (P.r(i)+P.d(i)-t2 +POSITIVE_ZERO < t2)
	  push_backBound(list, P.r(i)+P.d(i)-t2,t2);
      }
    }
    else if (t2 + NEGATIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))) {
      if (t2 +NEGATIVE_ZERO > Up(P,i,t2))
	push_backBound(list,Up(P,i,t2),t2);
    }
    else{
      if (t2 +NEGATIVE_ZERO > Up(P,i,t2))
	push_backBound(list,Up(P,i,t2),t2); 
      if (t2 +NEGATIVE_ZERO > Dp(P,i,t2))
	push_backBound(list,Dp(P,i,t2),t2);
    }
  }
  return 0;
} 


template<typename type>
int computeCheckInterval(IntervalList<type>& list,const Problem<type>& P){
  std::vector<type> t1;
  t1.reserve(2*P.nbTask);
  for (int i=0;i<P.nbTask;++i) {
    t1.push_back(P.r(i));
    if (isEqual(P.bmin(i),P.bmax(i)))
      t1.push_back(P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)));
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
      t1.push_back(P.d(i) - P.W(i)/P.A[i].Fi(P.bmax(i)));
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


template<typename type>
int interLSInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i){
  if ( t1 + NEGATIVE_ZERO <= P.r(i) && P.d(i) > POSITIVE_ZERO + t1)
    push_backBound(list,t1,P.d(i));
  else if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) > t1 +NEGATIVE_ZERO 
	   && t1 +POSITIVE_ZERO < U(P,i,t1))
    push_backBound(list,t1,U(P,i,t1));
  return 0;
}

template<typename type>
int interLSInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i){
  if ( t2 > P.r(i) + NEGATIVE_ZERO && 
       (t2 + NEGATIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))  ||  
	t2 >= P.r(i) + P.W(i)/P.A[i].Fi(P.bmin(i)) + NEGATIVE_ZERO))
    push_backBound(list,P.r(i),t2);
  if ((t2 +NEGATIVE_ZERO < P.r(i) + P.W(i)/P.A[i].Fi(P.bmin(i)) 
       || t2 + NEGATIVE_ZERO > P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i))) 
      && t2 > Up(P,i,t2) + POSITIVE_ZERO)
    push_backBound(list,Up(P,i,t2),t2);
  return 0;
}

template<typename type>
int interLSSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i){
  if (P.r(i) + P.W(i)/P.A[i].Fi(P.bmax(i)) > t1+ NEGATIVE_ZERO 
      && t1 > It1(P,i) + NEGATIVE_ZERO && t1 + POSITIVE_ZERO<U(P,i,t1))
    push_backBound(list,t1,U(P,i,t1));
  if (t1 + NEGATIVE_ZERO <= It1(P,i) && t1 + POSITIVE_ZERO < P.d(i))
    push_backBound(list, t1, P.d(i)); 
  return 0;
}


template<typename type>
int interLSSup_t1(IntervalList<type>& list, const Problem<type> &P, type t2, int i){
  if ( t2 > P.r(i)  + POSITIVE_ZERO  && (t2 >= P.d(i)  + NEGATIVE_ZERO  || t2 + NEGATIVE_ZERO <= P.r(i) + P.W(i)/P.A[i].Fi(P.bmin(i))))
    push_backBound(list,P.r(i),t2);
  else if ( t2 +NEGATIVE_ZERO > Up(P,i,t2))
    push_backBound(list,Up(P,i,t2),t2);
  return 0;
}


template<typename type>
int interRSInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i){
  if ( t1 + NEGATIVE_ZERO < P.d(i) && (t1 + NEGATIVE_ZERO <= P.d(i) - P.W(i) + P.A[i].Fi(P.bmax(i)) || t1 + NEGATIVE_ZERO <= P.d(i) - P.W(i) + P.A[i].Fi(P.bmin(i))))
    push_backBound(list,t1,P.d(i));
  else if (t1 + POSITIVE_ZERO< P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) && t1 + NEGATIVE_ZERO > P.d(i) - P.W(i)/P.A[i].Fi(P.bmax(i))&& t1 +POSITIVE_ZERO < D(P,i,t1))
    push_backBound(list,t1,D(P,i,t1));
  return 0;
}

template<typename type>
int interRSInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i){
  if (t2 >= P.d(i) + NEGATIVE_ZERO && t2 > P.r(i) +POSITIVE_ZERO)
    push_backBound(list,P.r(i),t2);
  if (t2 +POSITIVE_ZERO > P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) && t2 + NEGATIVE_ZERO < P.d(i) && t2 + NEGATIVE_ZERO > Dp(P,i,t2))
    push_backBound(list,Dp(P,i,t2),t2);
  return 0;
}

template<typename type>
int interRSSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i){
  if ( t1 + POSITIVE_ZERO < P.d(i) && (t1 <= P.r(i) + POSITIVE_ZERO || t1 >= P.d(i) + P.W(i)/P.A[i].Fi(P.bmin(i))+ NEGATIVE_ZERO))
    push_backBound(list,t1,P.d(i));
  if ((t1 + NEGATIVE_ZERO < P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) 
       || t1 + POSITIVE_ZERO > P.r(i)) 
      && t1 + POSITIVE_ZERO < D(P,i,t1))
    push_backBound(list,t1,D(P,i,t1));
  return 0;
}

template<typename type>
int interRSSup_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i){
  if (t2 >= It2(P,i) + NEGATIVE_ZERO && t2 > P.r(i) + POSITIVE_ZERO)
    push_backBound(list, P.r(i), t2);
  if (t2 +POSITIVE_ZERO > P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)) && t2 + NEGATIVE_ZERO < It2(P,i) && t2 + NEGATIVE_ZERO > Dp(P,i,t2))
    push_backBound(list,Dp(P,i,t2),t2);
  return 0;
}

template<typename type>
int computeLSInterval(IntervalList<type>& list,const Problem<type>& P,int i){
  //LS x LS
  for (int j=0;j <P.nbTask;++j){
    if (P.bmin(j) != 0){
      if (P.W(j) + NEGATIVE_ZERO <=P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interLSInf_t2(list,P,P.r(i),j);
	interLSInf_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interLSInf_t2(list,P,P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	  interLSInf_t1(list,P,P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	}
      }
      else if (P.W(j) + POSITIVE_ZERO  > P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interLSSup_t2(list,P,P.r(i),j);
	interLSSup_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interLSSup_t2(list,P,P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	  interLSSup_t2(list,P,P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	}
      }
    }
  }
  return 0;
}

template<typename type>
int computeRSInterval(IntervalList<type>& list,const Problem<type>& P,int i){
  //RS x RS
  for (int j=0;j <P.nbTask;++j){
    if (P.bmin(j) != 0){
      if (P.W(j) + NEGATIVE_ZERO <=P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interRSInf_t2(list,P,P.r(i),j);
	interRSInf_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interRSInf_t2(list,P,P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	  interRSInf_t1(list,P,P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	}
      }
      else if (P.W(j) + POSITIVE_ZERO  > P.A[j].Fi(P.bmin(j))*(P.d(j)-P.r(j))){
	interRSSup_t2(list,P,P.r(i),j);
	interRSSup_t1(list,P,P.d(i),j);
	if (isEqual(P.bmax(i),P.bmin(i))){
	  interRSSup_t2(list,P,P.d(i)-P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	  interRSSup_t2(list,P,P.r(i)+P.W(i)/P.A[i].Fi(P.bmax(i)),j);
	}
      }
    }
  }
  return 0;
}







