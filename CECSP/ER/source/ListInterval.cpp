#include "ListInterval.hpp"


int Ht2(const Problem<int,int> &P,int i) {
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),2*P.A[i].Fi(P.bmin(i))));
  return (int)ceil((double)(P.d(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
		   /(double)(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i))));
}

int Ht2(const Problem<int,double> &P,int i) {
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),2*P.A[i].Fi(P.bmin(i))));
  return (int)ceil((double)(P.d(i)*(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i)))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
		   /(double)(P.A[i].Fi(P.bmax(i))-2*P.A[i].Fi(P.bmin(i))));
}


int It2(const Problem<int,int> &P,int i) { 
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),P.A[i].Fi(P.bmin(i))));
  return (int)ceil((double)(P.d(i)*P.A[i].Fi(P.bmax(i))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
		   /(double)(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));
}
	  
int It2(const Problem<int,double> &P,int i) { 
  assert(!isEqual(P.A[i].Fi(P.bmax(i)),P.A[i].Fi(P.bmin(i))));
  return (int)ceil((double)(P.d(i)*P.A[i].Fi(P.bmax(i))-P.r(i)*P.A[i].Fi(P.bmin(i))-P.W(i))
		   /(double)(P.A[i].Fi(P.bmax(i))-P.A[i].Fi(P.bmin(i))));
}

int eet(const Problem<int,int> &P, int i){
  return P.r(i) + (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmax(i)));
}

int eet(const Problem<int,double> &P, int i){
  return P.r(i) + (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmax(i)));
}

int lst(const Problem<int,int> &P, int i){
  return P.d(i) - (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmax(i)));
}

int lst(const Problem<int,double> &P, int i){
  return P.d(i) - (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmax(i)));
}

int Smin(const Problem<int,int> &P, int i){
  return P.d(i) - (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmin(i)));
}

int Smin(const Problem<int,double> &P, int i){
  return P.d(i) - (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmin(i)));
}

int Emax(const Problem<int,int> &P, int i){
  return P.r(i) + (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmin(i)));
}

int Emax(const Problem<int,double> &P, int i){
  return P.r(i) + (int)ceil((double)P.W(i)/(double)P.A[i].Fi(P.bmin(i)));
}
