#include "model_RCPSP_OOE.h"

int main(int /*argc*/, char* argv[]){
  int nbTask,nbRes;
  type  obj1,obj2,obj3,obj;
  std::ifstream instance(argv[1],std::ios::in);
  if (!instance)  {    
    std::cout << "Ouverture fichier instance impossible" << std::endl;
    return 1;
  }
  instance >> nbTask >> nbRes;
  problem P(nbTask,nbRes);
  P.readFromFile(instance);
  instance.close();
  std::ifstream TW(argv[2],std::ios::in);
  if (TW) P.timeWindows(TW);
  TW.close();
  int preemcut=atoi(argv[3]);
  solution s(nbTask);
  // P.displayProblem();
   std::cout << "________Starting LP relaxation resolution_______\n";
  solve_OOE_LP(P,obj1);  
  std::cout << "________Starting MIP resolution with node Lim=1 et without cut_______\n";
  solve_OOE_MIP(P,s,obj2,1,-1,preemcut);
  s.clear();
  s.reAlloc(nbTask);
  std::cout << "________Starting MIP resolution with nbSol=1 and without cuts_______\n" ; 
  solve_OOE_MIP(P,s,obj3,-1,1,preemcut);
  std::cout << "valide? " << s.isValid(P) << std::endl;
  s.clear();
  s.reAlloc(nbTask);
  std::cout << "________Starting MIP resolution with default settings_______\n" ; 
  solve_OOE_MIP(P,s,obj,-1,-1,preemcut);
  //s.printSol();
  std::cout << "valide? " << s.isValid(P) << std::endl;
  std::cout << "_________Others results_____________\n";
  std::cout << "Root gap without cut: " << (obj2-obj)/obj2 *100<<"\n" ;

  std::cout << "*****************************************************************************"
	    <<std::endl;
  return 0;

}
