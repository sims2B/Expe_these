#ifndef DEF_LISTINTERVAL
#define DEF_LISTINTERVAL
#include <vector>
#include <cassert>
#include "Problem.h"

template<typename type>
using IntervalList=std::vector<Interval<type>>;

template<typename type> 
int push_backBound(IntervalList<type>& ,type,type );

template<typename type>
inline int isBetween(type x, type y, type z){
  return (z>=y && y>=x);}
  
template<typename type>
inline type Ht1(const Problem<type> &P,int i) {  
  assert(P.A[i].Fi(P.bmax(i))!=2*P.A[i].Fi(P.bmin(i)));
  return (P.r(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.d(i)*P.A[i].Fi(P.bmin(i))+P.W(i))
	  /(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i)));
}

template<typename type>
  inline type Ht2(const Problem<type> &P,int i) {
  assert(P.A[i].Fi(P.bmax(i))!=2*P.A[i].Fi(P.bmin(i)));
  return (P.d(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
	  /(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i)));

}

  template<typename type>	 
  inline type It1(const Problem<type> &P,int i) { 
    assert(P.A[i].Fi(P.bmax(i))!=P.A[i].Fi(P.bmin(i)));
    return ((P.r(i)*P.A[i].Fi(P.bmax(i))-P.d(i)*P.A[i].Fi(P.bmin(i))+P.W(i))
	    /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));}
	
    template<typename type> 
  inline type It2(const Problem<type> &P,int i) { 
      assert(P.A[i].Fi(P.bmax(i))!=P.A[i].Fi(P.bmin(i)));
      return ((P.d(i)*P.A[i].Fi(P.bmax(i))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
	      /(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));}
	  
      template<typename type>
  type Dp(const Problem<type>&,int i,type val);
template<typename type>
  type U(const Problem<type>&,int i,type val);
template<typename type>
  type D(const Problem<type>&,int i,type val);
template<typename type> 
  type Up(const Problem<type>&,int i,type val);


template<typename type>
  int interNul_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i);
template<typename type>
  int interNul_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i);
template<typename type>
  int interInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i);
template<typename type>
  int interInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i);
template<typename type>
  int interSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1,int i);
template<typename type>
  int interSup_t1(IntervalList<type>& list, const Problem<type> &P,type t2,int i);


template<typename type>
  int computeCheckInterval(IntervalList<type>& list,const Problem<type>& P);

template<typename type>
  int interLSInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i);
template<typename type>
  int interLSInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i);
template<typename type>
  int interLSSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i);
template<typename type>
  int interLSSup_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i);

template<typename type>
  int interRSInf_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i);
template<typename type>
  int interRSInf_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i);
template<typename type>
  int interRSSup_t2(IntervalList<type>& list, const Problem<type> &P,type t1, int i);
template<typename type>
  int interRSSup_t1(IntervalList<type>& list, const Problem<type> &P,type t2, int i);


//Pour faire tous les ajustement à gauche il faut aussi faire avec checkInterval!! et pareil à droite!
template<typename type>
  int computeLSInterval(IntervalList<type>& list,const Problem<type>& P,int i);
template<typename type>
  int computeRSInterval(IntervalList<type>& list,const Problem<type>& P,int i);

#include "ListInterval.tpp"
#endif
