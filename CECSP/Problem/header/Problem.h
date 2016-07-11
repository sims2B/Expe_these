#ifndef DEF_PROBLEM
#define DEF_PROBLEM

#include <vector>
#include <iostream>
#include <fstream>
#include "Task.h"


template<typename type>
using TaskList=std::vector<Task<type>>;

template<typename type>
struct Problem {
  Problem(int);
  Problem(int,type);
  Problem(int,type,TaskList<type>);
  Problem(const Problem&);

  void updateHorizon();
 
  void readFromFile(std::ifstream&); 
  void addPiecewiseFunction();
  void ecrire(std::ofstream& new_inst) const;

  inline type r(int i) const {return A[i].ri;}
  inline type d(int i) const {return A[i].di;}
  inline type bmin(int i) const {return A[i].bmin;}
  inline type bmax(int i) const {return A[i].bmax;}
  inline type emin(int i) const {return A[i].emin;}
  inline type smax(int i) const {return A[i].smax;}
  inline type W(int i) const {return A[i].Wi;}
  inline Function<type> f(int i) const {return A[i].Fi;}

  Problem<type>& operator=(const Problem<type>& P);

  void displayProblem() const;

  int dataConsistency() const;

  type totalEnergyConsumption(const Interval<type>&) const;
  type totalResourceConsumption(const Interval<type>&) const;
  inline int energeticReasonning(const Interval<type>& I,type total) const
  {return (B*(I.t2-I.t1) - total>= NEGATIVE_ZERO);}

  //vérifier ajustement !!!
  int adjustmentLS(const Interval<type>&,int,type);
  int adjustmentRS(const Interval<type>&,int,type);
  int totalTest(const Interval<type>&);

  int nbTask;
  type B;
  type R;
  type D;
  TaskList<type> A;
};

#include "Problem.tpp"
#endif
