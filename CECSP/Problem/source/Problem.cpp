#include "Problem.hpp"

template<>
int Problem<int>::adjustmentEmin(const Interval<int> &I,int i,int total){  
if (I.t2 < d(i)&& I.t2 > emin(i)){
    int performed=0;
    const int LS = A[i].resourceConversion(A[i].leftShift(I),I);
    const int SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  < LS) {
      int temp=emin(i);
      A[i].emin=std::max(emin(i) , 
			  I.t2 + (int)ceil((double)(LS - SL) /(double)bmax(i)));
      if (A[i].emin!=temp)
	performed=1;
    }
    return performed; 
  }
  else return 0;
}

template<>
int Problem<int,double>::adjustmentEmin(const Interval<int> &I,int i,double total){  
  if (I.t2 < d(i) && I.t2 > emin(i)){

    // std:: cout << "**début ajustment emin**\n";
    int performed=0;
    const double LS = A[i].resourceConversion(A[i].leftShift(I),I);
    const double SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  - POSITIVE_ZERO < LS) {
      int temp=emin(i);
      A[i].emin=std::max(emin(i) , 
		         I.t2 + (int)ceil((double)(LS - SL) /(double)bmax(i)));
      if (A[i].emin!=temp){
	performed=1;
	//	std::cout << "emin old=" << temp << " new=" << A[i].emin << std::endl;
      }
    }
    return performed; 
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentRi(const Interval<int> &I,int i,int total){
  if (I.t1 > r(i)  && I.t1 < d(i) ){
    int performed=0;
    const int LSboth =A[i].resourceConversion(std::min(A[i].leftShift(I),
						       A[i].bothShift(I)),I);
    const int SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  < LSboth && bmin(i)!=0) {
      int temp=r(i);
      A[i].ri=std::max(r(i), I.t2 - (int)ceil((double)SL/(double)bmin(i)));  
      if (A[i].ri!=temp)
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int,double>::adjustmentRi(const Interval<int> &I,int i,double total){
  if (I.t1 > r(i)  && I.t1 < d(i) ){

    // std:: cout << "**début ajustment ri**\n";
    int performed=0;   
    const int LSboth = A[i].resourceConversion(std::min(A[i].leftShift(I),
							A[i].bothShift(I)),I);
    const double SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  < LSboth- POSITIVE_ZERO && bmin(i)!=0.0) {
      /*   std:: cout << "LS = " <<A[i].resourceConversion(A[i].leftShift(I),I) << " " ;
    std:: cout << "BS = " <<A[i].resourceConversion(A[i].bothShift(I),I) << " " ;
    std:: cout << "SL = " <<SL << " " ;
    std:: cout << "cap = "<<B*(I.t2-I.t1)<< " " ;
    std::cout << "total = "<< total << " ";
    std::cout << "ex ri = "<< r(i) << " ";
      */
      int temp=r(i);
      A[i].ri=std::max(r(i), I.t2 - (int)ceil((double)SL/(double)bmin(i)));  
      if (A[i].ri!=temp){
	performed=1;
	//std::cout << "ri old=" << temp << " new=" << A[i].ri << std::endl;
      }
      //   std::cout << "new ri = "<< r(i) << "\n";
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentSmax(const Interval<int> &I,int i,int total){
  if (I.t1 > r(i)  && I.t1 < smax(i)){
    int performed=0;
    const int SL=B*(I.t2-I.t1) -total+A[i].resourceConsumption(I);
    const int RS=A[i].resourceConversion(A[i].rightShift(I),I);
    if (SL < RS) {
      int temp=smax(i);
      A[i].smax=std::min(smax(i), I.t1 - (int)ceil((double)(RS-SL)/(double)bmax(i)));
      if (A[i].smax!=temp)
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int,double>::adjustmentSmax(const Interval<int> &I,int i,double total){
  // std:: cout << "**début ajustment smax**\n";
  if (I.t1 > r(i) && I.t1 < smax(i)){
    int performed=0;
    const double SL=B*(I.t2-I.t1) -total+A[i].resourceConsumption(I);
    const int RS=A[i].resourceConversion(A[i].rightShift(I),I);
    if (SL + POSITIVE_ZERO < RS) {
      int temp=smax(i);
      A[i].smax=std::min(smax(i), I.t1 - (int)ceil((double)(RS-SL)/(double)bmax(i)));
      if (A[i].smax!=temp){
	performed=1;
	//	std::cout << "smax old=" << temp << " new=" << A[i].smax << std::endl;
      }
}
    return performed;
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentDi(const Interval<int> &I,int i,int total){
  if (I.t2 < d(i) && I.t2 > r(i) ){
    int performed=0;
    const int RSboth =A[i].resourceConversion(std::min(A[i].rightShift(I),
							  A[i].bothShift(I)),I);
    const int SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  < RSboth && bmin(i)!=0) {
      int temp=d(i);
          A[i].di=std::max(d(i), I.t1 - (int)ceil((double)SL/(double)bmin(i)));  
      if (A[i].di!=temp)
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int,double>::adjustmentDi(const Interval<int> &I,int i,double total){
  // std:: cout << "**début ajustment di**\n";
  if (I.t2 < d(i) && I.t2 > r(i) ){
    int performed=0;
    const double RSboth =A[i].resourceConversion(std::min(A[i].rightShift(I),
							  A[i].bothShift(I)),I);
    const double SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL  + POSITIVE_ZERO  < RSboth && bmin(i)!=0.0) {
      int temp=d(i);
      A[i].di=std::max(d(i), I.t1 - (int)ceil((double)SL/(double)bmin(i)));  
      if (A[i].di!=temp){
	performed=1;
	//	std::cout << "di old=" << temp << " new=" << A[i].di << std::endl;
      }
    }
    return performed;
  }
  else return 0;
}




Problem<int,int> generate(int nbTask, int B){
  Problem<int,int> Q(nbTask,B);
  for (int i = 0 ; i < nbTask ; ++i){
    //init random
    int  Wi, bmin, bmax, ri, di;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> disA(1,10);
    int _a = disA(gen);
    std::uniform_int_distribution<> disC(0,10);
    //random generation of c0 (if bmin=0 c1=0) in [0,10]
    int _c= disC(gen);
    
    
    //Wi!!!
    std::uniform_int_distribution<> disWi(_a + _c , (_a * B *1.25+ _c) );
    Wi = disWi(gen);
    //bmin!!!
    std::uniform_int_distribution<> disBmin(0,std::max((0.25 * Wi - _c)/_a,(double)1)); 
    bmin = disBmin(gen);
    //bmax!!!
    std::uniform_int_distribution<> disBmax(std::max(4,bmin) , std::max(2*bmin,8));
    bmax = disBmax(gen);
    //ri!!!
    std::uniform_int_distribution<> disRi(0 , 0.5 * nbTask);
    ri = disRi(gen);
    //di!!!
    int emin = ceil((double)ri + (double)Wi / ((double)_a * (double)bmax + (double)_c));
    std::uniform_int_distribution<> disDi(emin , emin + nbTask);
    di = disDi(gen);
    Piece<int> _P(Interval<int>(bmin,bmax),LinearFunction<int>(_a,_c));
    PieceList<int> P(1,_P);
    std::cout << ri << " " << di << " " << Wi << " " << bmin << " " << bmax << " " << _a << " " << _c<<std::endl;
    Task<int,int> T(ri,di,Wi,bmin,bmax,P);
    Q.A.push_back(T);
  }
  Q.updateHorizon();
  return Q;
}
