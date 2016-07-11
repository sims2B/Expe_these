#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "hybridBB.h"
#include "flot_model.h"
#include <sys/time.h>


//OUBLIE PAS LE PARAMETRE TEU-BÊ!!!!!
int main(int /*argc*/, char* argv[]){

  /*int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  std::ofstream instance_PL(argv[2],std::ios::out);
  std::ofstream instance_L(argv[3],std::ios::out);
  instance >> nbTask ;
  Problem<double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
 
  for (int i=0;i<nbTask;++i){
    if (P.bmin(i)==0.0){      
      P.A[i].bmin =  P.bmax(i)/2;
    }
    else if ((double)P.bmax(i)/(double)P.bmin(i) >2){
      double delta =(double)P.bmax(i)/(double)P.bmin(i)  - 2;
      P.A[i].bmin = P.bmin(i) + (int)(delta/2);
      P.A[i].bmax = P.bmax(i) - (int)(ceil(delta/2));
    }
  } 
  P.addPiecewiseFunction(); 

  Solution<double,double> s(nbTask,P.D+1);  
  while ( !BranchBound(P,s,chooseVar3,intervalTotalTest,5.0,0.5) ){
    std::cout << "valide? " << s.isValid(P) << std::endl;
    for (int i=0;i<P.nbTask;++i)
      P.A[i].Wi=P.W(i)-P.W(i)*0.1;
  }
  std::cout << "valide? " << s.isValid(P) << std::endl;
  P.ecrire(instance_PL);

  for (int i=0;i<P.nbTask;++i){
    int a,c;
    if (P.bmin(i)==P.bmax(i))
      a=0;
    else
      a=(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))/(P.bmin(i)-P.bmax(i));
    c=P.A[i].Fi(P.bmin(i)) - a *P.bmin(i);
    P.A[i].Fi.F[0].f.c=c;
    P.A[i].Fi.F[0].f.a=a;
    P.A[i].Fi.nbPiece=1;
  }
  P.ecrire(instance_L);

  
  return 0;
  */

  struct timeval tim;
  //  std::cout << argc;
  
  
  std::ifstream instance(argv[1],std::ios::in);
  if (!instance)   {    
    std::cout << "Ouverture fichier instance impossible" << std::endl;
    return 0;
  }
    
  int _nbTask;
  double epsilon=atof(argv[4]);
  std::cout << "epsilon set to " << epsilon << std::endl;
  instance >> _nbTask;
  Problem<double> P(_nbTask);
  P.readFromFile(instance);
  instance.close();
  Problem<double> Q(P);


  Solution<double,double> s(_nbTask,2*_nbTask);
  ptrVar<double> choiceVar;
  ptrTest<double> TotalTest;
 

  if (atoi(argv[2])==1) choiceVar=chooseVar1;
  else if (atoi(argv[2])==2) choiceVar=chooseVar2;
  else if (atoi(argv[2])==3) choiceVar=chooseVar3;
  else if (atoi(argv[2])==4) choiceVar=chooseVar4;
  else if (atoi(argv[2])==5) choiceVar=chooseVar5;
  else if (atoi(argv[2])==6) choiceVar=chooseVar6;
  else {
    std::cout << "Wrong argument" << std::endl;    
    return 0;
  }

  if (atoi(argv[3])==1) TotalTest=intervalTotalTest;
  // else if (atoi(argv[3])==2) TotalTest=TTDR;
  else if (atoi(argv[3])==3) TotalTest= flowTotalTest;
  else if (atoi(argv[3])==4) TotalTest= flowERTotalTest;
  else {
    std::cout << "Wrong argument" << std::endl;    
    return 0;
    }

  gettimeofday(&tim,NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  std::cout << "le bb a dit " << BranchBound(Q,s,choiceVar,TotalTest,epsilon,0.5) <<std::endl;
  gettimeofday(&tim,NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  std::cout << "temps d'exectution : " << t2-t1 << std::endl;
  std::cout << " valide ?" << s.isValid(P) << std::endl;

  return 0;
}
