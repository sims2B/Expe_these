#ifndef DEF_TTDR
#define DEF_TTDR

#include "Problem.h"
#include <vector>
#include <tuple>

template<typename type=double>
using mytuple=std::tuple<int,type,type>;
template<typename type=double>
using fonction=std::tuple<type,type>;

template<typename type=double>
bool operator <(const mytuple<type>&,const mytuple<type>&);

////////////////////////////
/////RESTRICTIVE DR////////
///////////////////////////

//normalement ok mais peut etre à vérifier avant d'utiliser
template<typename type=double>
int restrictiveLeft(Problem<type>&);
template<typename type=double>
int restrictiveRight(Problem<type>&);
template<typename type=double>
int restrictive_DR(Problem<type>&);


////////////////////////////
//////////TIME TABLE////////
///////////////////////////

template<typename type=double>
int computeTimeTable(const Problem<type>&, std::vector<fonction<type>>&);
template<typename type=double>
int timeTableCheck(const Problem<type>&,std::vector<fonction<type>>& TT);

//////////////////////////
//////////T T D R/////////
/////////////////////////
template<typename type=double>
type consumption(const Problem<type>&, int , std::vector<fonction<type>>);
template<typename type=double>
int computeSet(const Problem<type>&,Problem<type>& ,std::vector<fonction<type>>&, std::vector<type>& ,std::vector<type>&);
template<typename type=double> 
int computeFreepart(const Problem<type>& P, Problem<type>& Pf);

template<typename type=double>
int TTDRLeft(Problem<type>&,Problem<type>& Pf,std::vector<fonction<type>>& TT,std::vector<type>& ,std::vector<type>&);
template<typename type=double>
int TTDRRight(Problem<type>&,Problem<type>& Pf,std::vector<fonction<type>>& TT,std::vector<type>& ,std::vector<type>&);
template<typename type=double>
int TTDR(Problem<type>&);

#include "ttdr.tpp"
#endif
