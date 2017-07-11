#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <vector> 
using namespace std;
struct res{
  int size;
  double root_obj;
  double root_gap;//a calculer
  double obj_first_sol;
  double time_first_sol;
  double gap_first_sol;//a calculer
  int status;
  double time_end;
  double final_obj;
  double final_gap;
  int nb_node;
  int nb_cut;
  res();
};
res::res() : size(0), root_obj(0.0) , root_gap(0.0), obj_first_sol (0.0), 
	     time_first_sol (0.0), gap_first_sol (0.0), status(0),time_end (0.0), 
	     final_obj (0.0), final_gap (0.0), nb_node(0), nb_cut(0){}

struct cell{
  std::string name;
  double best;
};

int main( int argc, char* argv[]){
  std::ifstream* res_file=new ifstream[argc-1];
  for (int i=0;i<argc-1;++i){
    res_file[i].open(argv[i+1], std::ifstream::in);
  }
  std::string trash;
  char prout;
  std::string inst_name;
  std::vector<cell> cell_vec;
  cell_vec.resize(1000);
  int cpt=0;
  int cpt2=42;
  while (!res_file[0].eof()){
    res_file[0] >> cell_vec[cpt].name;
    cell_vec[cpt].best=0.0;
     res_file[0].ignore(1000000,'*');
    res_file[0].ignore(1000000,'\n');
    cpt++;
  }
  res_file[0].clear();
  res_file[0].seekg(0, res_file[0].beg);
  cpt=0;
  while (cpt < argc-1){ 
    cpt2=42;
    while (!res_file[cpt].eof()){
      
      res res_line;
      getline(res_file[cpt], inst_name);
      res_file[cpt].seekg(-inst_name.size()-1, res_file[cpt].cur);
      res_file[cpt].ignore(4);
      res_file[cpt] >> res_line.size;
      res_file[cpt].ignore(1000,'\n');
      prout=res_file[cpt].get();
      prout=res_file[cpt].get();
      if (prout != '*'){
	res_file[cpt].ignore(1000,':');
	res_file[cpt] >> res_line.root_obj;
	res_file[cpt] >> trash;
	if (trash!= "..."){
	  res_file[cpt].ignore(1000, ':');
	  res_file[cpt] >> res_line.obj_first_sol;
	  res_file[cpt].ignore(1000,':');
	  res_file[cpt] >> res_line.time_first_sol;
	  res_file[cpt].ignore(1000,':');
	  res_file[cpt] >> trash ;
	  prout=res_file[cpt].get();  
	  prout=res_file[cpt].get();  
	  if (prout != '*'){
	    res_file[cpt].ignore(1000,':');
	    res_file[cpt] >> trash ;
	    if (trash == "Optimal")
	      res_line.status=3;
	    if (trash == "Feasible")
	      res_line.status=2;
	    res_file[cpt] >> trash;
	    res_file[cpt] >> res_line.time_end;
	    res_file[cpt].ignore(1000,':');
	    res_file[cpt] >> res_line.final_obj;
	    res_file[cpt].ignore(1000,':');
	    res_file[cpt] >> res_line.final_gap;
	    res_file[cpt].ignore(1000,':');
	    res_file[cpt] >> res_line.nb_node; 
	    //	res_file[cpt].ignore(1000,'\n'); 
	    res_file[cpt] >> trash;
	    if (trash == "number"){
	      res_file[cpt].ignore(1000,':');
	      res_file[cpt] >> res_line.nb_cut; 
	    }
	  }
	}
      }
      res_file[cpt].ignore(10000,'*');
      res_file[cpt].ignore(1000,'\n'); 
      prout=res_file[cpt].get(); 
      /*    std::cout << prout ;
	    std::cout << fixed << setprecision(2);
	    std:: cout << "$"<<
	    res_line.size << " $& $" <<    
	    res_line.root_gap << "$ & $" <<
	    res_line.time_first_sol << "$ & $" <<
	    res_line.gap_first_sol << "$ & $" <<
	    res_line.time_end << "$ & $" <<
	    res_line.final_gap << "$ & $" <<
	    res_line.nb_node << "$ & $" <<
	    res_line.nb_cut<< " $ \\\\ \n";
      */
      //cherche la best value et remplacer au besoin
      
      for (int i=0 ; i< cell_vec.size(); ++i){
	//	std::cout << inst_name << " " << cell_vec[i].best << " " << res_line.final_obj << "\n";
	if (inst_name == cell_vec[i].name){
	  if (cell_vec[i].best==0)    cell_vec[i].best = res_line.final_obj ;
	  else if (res_line.final_obj < cell_vec[i].best && res_line.final_obj!=0)
	    cell_vec[i].best = res_line.final_obj ;
	}
      }
      if (res_file[cpt].eof()) break;    
      else res_file[cpt].seekg(-1,res_file[cpt].cur);
    }
  res_file[cpt].close();
  cpt++;
  }
  delete [] res_file;
  for (int i=0 ; i < cell_vec.size(); ++i)
    if (cell_vec[i].name!="")
    std:: cout << cell_vec[i].name << " " << cell_vec[i].best << "\n";
  
  return 0;
}
