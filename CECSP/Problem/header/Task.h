#ifndef DEF_TASK
#define DEF_TASK

#include "Function.h"
#include <algorithm>
#include <iostream>

template<typename type> struct Task;
//!!!!!!!!!!!bmin!=0!!!!!!!!!!!!!!!!
template<typename type>
struct Task {
  //!!! Penser a updater smax et emin si changement Fi!!!
  Task(int);
  Task(type,type,type,type,type,Function<type>);
  Task(type,type,type,type,type,type,type);
  Task(type,type,type,type,type,int tmp=1);

void addPiecewiseFunction();
  void displayTask() const;

  //attention arrondi si integer!!

  inline void updateEMin() {emin = std::max(emin,ri +Wi/Fi(bmax));}
  inline void updateSMax() {smax = std::min(smax,di - Wi/Fi(bmax));}
  inline void updateRi() {if (bmin!=0.0) ri = std::max(ri,emin - (Wi/Fi(bmin)));}
  inline void updateDi() {if (bmin!=0.0) di = std::min(di,smax + (Wi/Fi(bmin)));}

  inline int dataConsistency() const {return (Fi(bmax)*(di-ri) - Wi >=NEGATIVE_ZERO);}
 
  type energyConsumption(const Interval<type>&) const;
  type resourceConsumption(const Interval<type>&) const;
  type resourceConversion(const type& energy, const Interval<type>&) const;
  type leftShift_adjust(const Interval<type>&) const;
  type rightShift_adjust(const Interval<type>&) const;  

  type leftShift(const Interval<type>&) const;
  type rightShift(const Interval<type>&) const;
  type bothShift(const Interval<type>&) const;
  
  type ri;
  type di;
  type Wi;
  type bmin;
  type bmax;
  Function<type> Fi;
  type emin;
  type smax;
};

int rdtsc();
#include "Task.tpp"
#endif
