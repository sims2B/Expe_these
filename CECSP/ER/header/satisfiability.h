#ifndef DEF_SATISFIABILITY
#define DEF_SATISFIABILITY
#include <vector>

#include "ListInterval.h"

//checker
template<typename type=double>
int intervalTest(const Problem<type>&);
//filter
template<typename type=double>
int intervalTotalAdjust(Problem<type> &);
//checker+filter
//nb adjust=adjust-1!
template<typename type=double>
int intervalTotalTest(Problem<type> &);

#include "satisfiability.tpp"
#endif
