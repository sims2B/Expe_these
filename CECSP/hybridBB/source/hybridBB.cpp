

#include "hybridBB.h"



//variable dans l'ordre 
template int chooseVar1(const Problem<int>&,int);
//variable random
template int chooseVar2(const Problem<int>&,int);
//+ petit intervalle (min=100000.0)
template int chooseVar3(const Problem<int>&,int);
//+ grand intervalle (max=0.0)
template int chooseVar4(const Problem<int>&,int);
//tâche la plus contrainte (fi(bmax)(di-ri)-wi min) en commençant par ri
template int chooseVar5(const Problem<int>&,int);
//tâche la moins contrainte
template int chooseVar6(const Problem<int>&,int);

///////////////////////////////////////

//PL inspiré du flot pour affecter les b_it aux feuilles
template int testFlow(const Problem<int>&, Solution<int,int>&);

//int treat(Problem<int> &P,Solution &s);

//le dernier parametre permet de regler l'endroit ou l'on coupe l'intervalle (0.5 on coupe a la moitié, 0.33 au 2eme tiers...)
template<> 
void createBranch(Problem<int>& P,int x,std::stack<Problem<int>>& explore,double param){ 
  Problem<int> Q(P);
  if (x < P.nbTask){
    P.A[x].smax-=(int)(ceil((P.smax(x)-P.r(x))*(1.0-param)));
    explore.push(P);
    Q.A[x].ri+=(Q.smax(x)-Q.r(x))*param;
    Q.A[x].updateEMin();
    explore.push(Q);
  }
  else  { 
    P.A[x-P.nbTask].emin+=(int)(ceil((P.d(x-P.nbTask)-P.emin(x-P.nbTask))*(1.0-param)));
    explore.push(P);
    Q.A[x-Q.nbTask].di-=(Q.d(x-Q.nbTask)-Q.emin(x-Q.nbTask))*param;
    Q.A[x-Q.nbTask].updateSMax();
    explore.push(Q);
  }


}

//template int BranchBound(Problem<int>&,Solution<int,int>&,ptrVar<int>,ptrTest<int>,int,double parameter=0.5);
//template int BranchBound(Problem<int>&,Solution<int,double>&,ptrVar<int>,ptrTest<int>,int,double parameter=0.5);







