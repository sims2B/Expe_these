#include "model_mip_bin.h"
#include "model_mip_simpl.h"
#include <sys/time.h>

int main(int , char* argv[])
{
  struct timeval tim;  
  std::ifstream instance(argv[1],std::ios::in);

  if (!instance) 
    {    
      std::cout << "Ouverture fichier instance impossible" << std::endl;
      return 1;
    } 

  int n,m,Q,t;
  instance >> n >> m >> Q >> t;
  LotProblem LP(n,m,Q,t);

  LP.readFromFile(instance);
  //LP.nbSublot=5;
  LP.nbSublot=atoi(argv[2]);
  Solution s(LP.nbLot,LP.nbSublot,LP.nbLine);
  Solution s2(LP.nbLot,LP.nbSublot,LP.nbLine);
  
  //decommenter pour mon_instance
  //LP.beginHP[0]=2;
  //LP.endHP[0]=4;

  std::cout << "Starting resolution with "<< LP.nbSublot 
	    << " sublots and alpha=" << ALPHA << " beta="<<BETA << "and +20% timeline" 
	    << std::endl; 
  gettimeofday(&tim,NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  if (!solveSimple(LP,s)) {
    //std::cout << "solution 1 valide? " << s.isValid(LP) << std::endl;
    std::cout<< "conso s1? " << s.conso(LP) << "\n";
    std::cout << "peak s1? "<<s.peak(LP) << "\n";
    std::cout << "tard s1? "<<s.tardiness(LP) << "\n";
    std::cout << "obj s1?" << s.totalObj(LP,ALPHA,BETA,GAMMA) <<"\n";
    double totobj=solveBinSimple(LP,s,s2);
    gettimeofday(&tim,NULL);
    double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
    std::cout << "temps d'exectution : " << t2-t1 << std::endl;
    std::cout << "solution 2 valide? " << s2.isValid(LP) << std::endl;
    std::cout << "conso s2? " << s2.conso(LP) << std::endl;  
    std::cout << "peak? " << totobj << std::endl;
    std::cout << "tard s2? "<<s2.tardiness(LP) << "\n";
    }

  return 0;
}

