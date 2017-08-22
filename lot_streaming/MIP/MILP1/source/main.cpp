#include "model_mip_se.h"
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
  LP.nbSublot=3; // attentions instaces irrealisables a 2 soous-lots
  Solution s(LP.nbLot,LP.nbSublot,LP.nbLine);
  Solution s2(LP.nbLot,LP.nbSublot,LP.nbLine);
  
  //decommenter pour mon_instance
  //LP.beginHP[0]=2;
  //LP.endHP[0]=4;

  //LP.displayProblem();

  solveSimple(LP,s);
  std::cout << "solution valide? " << s.isValid(LP) << std::endl;
  std::cout << "peak? " << s.peak(LP) << std::endl;
  std::cout << "total obj? " << s.totalObj(LP,ALPHA,BETA) << std::endl;
  if (s.isValid(LP)) { 
    solveRealSimple(LP,s,s2);
    std::cout << "solution valide? " << s2.isValid(LP) << std::endl;
    std::cout << "conso? " << s2.conso(LP) << std::endl;
    std::cout << "peak? " << s2.peak(LP) << std::endl;
  } else 
    std::cout << "common deadline not met, stop" << std::endl;
  return 0;
}

