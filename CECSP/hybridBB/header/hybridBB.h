#ifndef DEF_HYBRIDBB
#define DEF_HYBRIDBB

#include "satisfiability.h"
#include "Solution.h"
//#include "ttdr.h"
#include <stack>

template<typename type=double>
using ptrTest=int(*)(Problem<type>&);
template<typename type=double>
using ptrVar=int(*)(const Problem<type>&,type);

////////////////////////////////////////////
///////////choix variable///////////////////
///////////////////////////////////////////


//variable dans l'ordre 
template<typename type=double>
int chooseVar1(const Problem<type>&,type);
//variable random
template<typename type=double>
int chooseVar2(const Problem<type>&,type);
//+ petit intervalle (min=100000.0)
template<typename type=double>
int chooseVar3(const Problem<type>&,type);
//+ grand intervalle (max=0.0)
template<typename type=double>
int chooseVar4(const Problem<type>&,type);
//tâche la plus contrainte (fi(bmax)(di-ri)-wi min) en commençant par ri
template<typename type=double>
int chooseVar5(const Problem<type>&,type);
//tâche la moins contrainte
template<typename type=double>
int chooseVar6(const Problem<type>&,type);

///////////////////////////////////////

//PL inspiré du flot pour affecter les b_it aux feuilles
template<typename type=double>
int testFlow(const Problem<type>&, Solution<type,type>&);

//int treat(Problem<type> &P,Solution &s);

//le dernier parametre permet de regler l'endroit ou l'on coupe l'intervalle (0.5 on coupe a la moitié, 0.33 au 2eme tiers...)
template<typename type=double> 
void createBranch(Problem<type>&,int,std::stack<Problem<type>>&,double);

template<typename type=double,typename type2= type>
int BranchBound(Problem<type>&, Solution<type,type2>&, ptrVar<type>, ptrTest<type>,type,double parameter=0.5);

#include "hybridBB.tpp"
#endif
