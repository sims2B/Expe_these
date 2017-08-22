#include "model_mip_simpl.h"

int main(int , char* argv[])
{
  
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
  LP.nbSublot=atoi(argv[2]);
  Solution s(LP.nbLot,LP.nbSublot,LP.nbLine);
  
  //decommenter pour mon_instance
  //LP.beginHP[0]=2;
  //LP.endHP[0]=4;

  if (!solveSimple(LP,s)) {
    std::cout << "solution valide? " << s.isValid(LP) << std::endl;
    std::cout << "pic HP? " << s.peak(LP) << std::endl;
    std::cout << "conso totale? " << s.conso(LP) << std::endl;
    std::cout << "obj pondéré " << s.totalObj(LP,0.5,0.5) << std::endl;
    }
  return 0;
}

