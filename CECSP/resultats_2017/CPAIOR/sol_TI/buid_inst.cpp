#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <vector>
#include "Solution.h"
using namespace std;

struct cell{
  string name;
  Problem<double> P;
  cell(const Problem<double>& p);
};

cell::cell(const Problem<double> &p): P(p){}

int main( int argc, char* argv[]){
  std::ifstream* res_file=new ifstream[argc-2];
  for (int i=0;i<argc-2;++i){
    res_file[i].open(argv[i+2], std::ifstream::in);
  }
  std::ifstream sol_file(argv[1],std::ios::in);
  if (!res_file)  {    
    std::cout << "Ouverture sol impossible" << std::endl;
    return 1;
  }
  
  std::vector<cell> cell_vec;
  int cpt=argc-3;
  while (cpt){
    std::string str= argv[cpt+2];
    std::string str2;
    std::size_t found = str.rfind("/");
    if (found!=std::string::npos)
      str=str.substr(found);
    str=str.substr(1,str.size());
    str2=str.substr(4,2);
    Problem<double> Q(stoi(str2,nullptr));
    res_file[cpt] >> Q.nbTask;
    Q.readFromFile(res_file[cpt]);
    cell elmt(Q);
    elmt.name=str;
    cell_vec.push_back(elmt);
    res_file[cpt].close();
    cpt--;
  }
  /*for (uint i =0;i <cell_vec.size();++i){ std::cout << "name " <<
    cell_vec[i].name<<'\n'; std::cout << "size " <<
    cell_vec[i].P.nbTask<<'\n';
    }*/
      char prout = 'g';
  while (!sol_file.eof()){
    string inst_name;
    string trash;
    int val=0;
    if (prout == 'i')
      sol_file.seekg(-1,sol_file.cur);
    std::cout << "rrr    " << prout << '\n';
    getline(sol_file, inst_name);
    std::cout << "!!!!!!!!!!!!!!!!!!! d " << inst_name << " d !!!!!!!!!!!!!\n";
    int size = stoi(inst_name.substr(4,2));
    ofstream fichier(inst_name, ios::out | ios::trunc);

    for (uint i=0;i<cell_vec.size();++i)
      if (inst_name == cell_vec[i].name)
        val=i;
    std::cout << size<< " dnelzDDDD " << cell_vec[val].P.D+1<<'\n';
    Solution<double> s(size,cell_vec[val].P.D+1);
    sol_file.ignore(1000,':');
    sol_file.ignore(1000,':');
    sol_file.ignore(1000,':');
    sol_file.ignore(1000,':');
    sol_file.ignore(1000,':');
    sol_file >> trash;
    if (trash=="Optimal"){
      for (int j=0 ; j<cell_vec[val].P.nbTask;++j){
	sol_file.ignore(1000,'=');
	sol_file >> size;
	s.st[j] = size ;
	sol_file.ignore(1000,'=');
	sol_file >> size;
	s.ft[j] = size ;
	sol_file.ignore(1000,'(');
	for (uint t=0;t<cell_vec[val].P.D+1;++t){
	  sol_file >> size;
	  s.b[j][t] = size;
	  sol_file.ignore(1000,',');
	}
      }
      
      sol_file.ignore(1000,'(');
      for (int t=0;t<cell_vec[val].P.D+1 ;++t){
	sol_file >> size;
	s.event.push_back(size);
	std::cout << s.event[t] << " gggg \n";
	sol_file.ignore(1000,',');
      }

      //generer fonction concave
      //      cell_vec[val].P.addPiecewiseFunction();

      //modifier WI
      for (int j=0;j<cell_vec[val].P.nbTask;++j){
	int nrj=0;
	for (int t=0;t<cell_vec[val].P.D+1;++t)
	  nrj+=cell_vec[val].P.A[j].Fi(s.b[j][t]);
	cell_vec[val].P.A[j].Wi=nrj;
      }
      //ecrire l'instance dans le fichier
      cell_vec[val].P.ecrire(fichier);

      //aller à l'instance suivante
    }
    sol_file.ignore(10000,'*');
    sol_file.ignore(1000,'\n');
    prout = sol_file.get();
  }
  sol_file.close();
  return 0;
}
