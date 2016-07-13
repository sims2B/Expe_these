#ifndef DEF_HYBRIDBB
#define DEF_HYBRIDBB

#include "satisfiability.h"
#include "Solution.h"
//#include "ttdr.h"
#include <stack>

template<typename type>
using ptrTest=int(*)(Problem<type>&);
template<typename type>
using ptrVar=int(*)(const Problem<type>&,type);

////////////////////////////////////////////
///////////choix variable///////////////////
///////////////////////////////////////////


//variable dans l'ordre 
template<typename type>
int chooseVar1(const Problem<type>&,type);
//variable random
template<typename type>
int chooseVar2(const Problem<type>&,type);
//+ petit intervalle (min=numericlimits::max)
template<typename type>
int chooseVar3(const Problem<type>&,type);
//+ grand intervalle (max=numericlimits::min)
template<typename type>
int chooseVar4(const Problem<type>&,type);
//tâche la plus contrainte (fi(bmax)(di-ri)-wi min) en commençant par ri
template<typename type>
int chooseVar5(const Problem<type>&,type);
//tâche la moins contrainte
template<typename type>
int chooseVar6(const Problem<type>&,type);

///////////////////////////////////////

//PL inspiré du flot pour affecter les b_it aux feuilles
template<typename type>
int testFlow(const Problem<type>&, Solution<type,type>&);

//int treat(Problem<type> &P,Solution &s);

//le dernier parametre permet de regler l'endroit ou l'on coupe l'intervalle (0.5 on coupe a la moitié, 0.33 au 2eme tiers...)
template<typename type> 
void createBranch(Problem<type>&,int,std::stack<Problem<type>>&,double);

template<typename type,typename type2= type>
int BranchBound(Problem<type>&, Solution<type,type2>&, ptrVar<type>, ptrTest<type>,type,double parameter=0.5);

#include "hybridBB.tpp"
#endif
