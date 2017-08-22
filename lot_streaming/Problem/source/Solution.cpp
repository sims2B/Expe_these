#include "Solution.h"
#include <iostream>
#include <tuple>
#include <algorithm>

int Solution::isValid(const LotProblem& LP) const{
  int i,j,q,q2,k;
  uint k2;
  int Fmax=0;
  int n= LP.nbLot;
  int m=LP.nbLine;
  int Q= LP.nbSublot;


  //finit avant la deadline généraleend before the common deadline
  for (i=0;i<n;++i){
    for (q=0;q<Q;++q)
      Fmax = ((ft[i][q] > Fmax) ? ft[i][q] : Fmax) ;
  }
  if (Fmax > LP.horizon) 
    std::cout << "deadline violated\n";
  
  //demand of each job satisfied
  for (i=0; i<n;++i) {
    double dem=0;
    for (q=0;q<Q;++q){
      if (a[i][q]!=-1)
	dem+= (ft[i][q]-st[i][q])*(LP.speed[i][a[i][q]]);
    }
    if (dem - LP.demand[i] <=NEGATIVE_ZERO) return 0;
  }

  //authorized machine constraint
   for (i=0;i<n;++i){
     for (q=0;q<Q;++q){
       for (k2=0;k2<LP.subset[i].size();++k2)
	 if (a[i][q]==LP.subset[i][k2])
	   return 0;
     }
   }
  
  //setup respected
    for (i=0;i<n;++i){
     for (j=0;j<n;++j){
       for (q=0;q<Q;++q){
	 for (q2=0;q2<Q;++q2){
	   if (i!=j && a[i][q]!=-1 && a[i][q]==a[j][q2] && 
	       st[j][q2]-st[i][q]>NEGATIVE_ZERO && 
	       st[j][q2]-ft[i][q]-LP.setup[i][j]< NEGATIVE_ZERO){
	     return 0;
	   }
	 }
       }
     }
   }

  // maintenace operation
  for (k=0;k<m;++k){
    for (i=0;i<n;++i){
      for (q=0;q<Q;++q){
	if (a[i][q]==k && 
	    (ft[i][q]-dm[k]>POSITIVE_ZERO && 
	     NEGATIVE_ZERO>st[i][q]-dm[k]-LP.durMaint[k])) 
	  return 0;
      }
    }
  }

  // maintenance end before the deadline
  for (k=0;k<m;++k)
    if (dm[k]+LP.durMaint[k]>LP.horizon)
      std::cout<<"deadline maintenace violated\n";
  
  //not two batches at the same time on the same machine
  for (i=0;i<n;++i){
    for (j=0;j<n;++j){
      for (q=0;q<Q;++q){
	for (q2=0;q2<Q;++q2){
	  if ((i!=j || q!=q2) && 
	      (st[j][q2] - ft[i][q]<NEGATIVE_ZERO && 
	       st[i][q] - ft[j][q2] <NEGATIVE_ZERO) &&
	      a[i][q]==a[j][q2])
	    return 0;
	}
      }
    }
  }
  
  return 1;  
}

void Solution::printSol(const LotProblem& LP) const {
  for (int i=0; i<st.size();++i) {
    for (int q=0;q<st[i].size();++q) {
      if (a[i][q]!=-1)
	std::cout << "The batch (" << i << "," << q << ") start at "<<
	  st[i][q] << " and ends at " << ft[i][q] << " on machine  " << 
	  a[i][q] << "\n";
    }
  }
  for (int k=0;k<dm.size();++k){
    if (dm[k]!=-1)
	std::cout << "The maintenance on machine  " << k <<
	  " starts at "<< dm[k] << " and ends at " << dm[k]+LP.durMaint[k] << "\n";
  }
}

//not working :(
double Solution::peak(const LotProblem & LP) const{
  double maxP=0.0;
  double peak_evt=0.0;
  int HPin=0;
  int i,q;
  
  std::vector<double> evt;

  //remplit vector event
  for (i=0;i<LP.nbLot;++i){
    for (q=0;q<LP.nbSublot;++q){
      evt.push_back(st[i][q]);
      evt.push_back(ft[i][q]);
    }
  }
  std::sort(evt.begin(),evt.end());

  for (int e=0;e<evt.size()-1 && HPin<LP.nbHP;++e){
    peak_evt=0.0;
    if (evt[e]>LP.endHP[HPin]) HPin++;
    else if (evt[e]<=LP.endHP[HPin] && evt[e+1]>LP.beginHP[HPin]){
      for (i=0;i<LP.nbLot;++i){
	for (q=0;q<LP.nbSublot;++q){
	  if (evt[e]< ft[i][q] && evt[e+1]>st[i][q])
	  peak_evt+=1.0;
	}
      } 
    }
    maxP = ((peak_evt > maxP) ? peak_evt : maxP);
  }
  std:: cout << "the peak function does not work\n";
  return maxP*LP.delta;
}

double Solution::conso(const LotProblem &LP) const {

  double total=0.0;
  for (int i=0;i<LP.nbLot;++i) {
    for (int q=0;q<LP.nbSublot;++q) {
      if (a[i][q]!=-1)    
	total+= ft[i][q] - st[i][q];
    }
  }
  return total*LP.delta;
}


double Solution::totalObj(const LotProblem &LP,double alpha,double beta,double gamma) const {
  return alpha*conso(LP) + beta*peak(LP)+ gamma*tardiness(LP);
}

double Solution::tardiness(const LotProblem& LP) const{
  double max_evt=0;
  for (int i=0;i<LP.nbLot;++i){
    for (int q=0;q<LP.nbSublot;++q)
      max_evt= (( ft[i][q] > max_evt) ? ft[i][q] : max_evt);
  }
  for (int k=0; k<LP.nbLine;++k)
    max_evt= (( dm[k]+LP.durMaint[k] > max_evt) ? dm[k]+LP.durMaint[k] : max_evt);
  return std::max(max_evt - LP.horizon,0.0);
  
}
