#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector> 
using namespace std;
struct res{
  int size;
  double total_time;
  double tree_time;
  double MIP_time;
  int nb_node;
  int nb_MIP;
  int nb_checkER;
  int nb_adjust;
  int solved;
  int feas;
  //constructeur par defaut, cas unsolved?
  res();
};
res::res() : size(0),total_time(0.0), tree_time(0.0),MIP_time(0.0),nb_node(0),nb_MIP(0), nb_checkER(0),nb_adjust(0),solved(0),feas(0){}

int main( int argc, char* argv[]){
  std::ifstream* res_file=new ifstream[argc-1];
  for (int i=0;i<argc-1;++i){
    res_file[i].open(argv[i+1], std::ifstream::in);
  }
  int j=0;
  int size;
  double val;
  double root_obj;
  std::string trash;
  char prout;
  std::vector<res> res_vec;
  int i=argc-1;
  while (i!=0){
    i--;
    while (!res_file[i].eof()){
      res res_line;
      if (prout=='i')
	res_file[i].ignore(3);
      else
	res_file[i].ignore(4);
    res_file[i] >> size;
    res_line.size=size;
    res_file[i].ignore(1000,'\n');
    res_file[i].ignore(1000,'\n');
    prout=res_file[i].get();
    if (prout == '*'){
      res_line.solved=0;
      res_line.feas=0;
      res_line.total_time=7200;
      res_file[i].ignore(1000,'\n');
      continue;
    }   
    else if (prout == 'K'){
      res_line.solved=0;
      res_line.feas=0;
      res_line.total_time=7200;
    }
    else if (prout == 'I' || prout=='1'){
      res_file[i] >> trash;
      res_file[i] >> trash;
      if (trash == "résolue"){
	res_line.feas=1;
	res_line.solved=1;
	res_file[i].ignore(1000, ':');
	res_file[i] >> res_line.total_time;
	res_file[i].ignore(1000, ':');
	res_file[i] >> val;
	res_line.tree_time = 100 * val / res_line.total_time; 
	res_file[i].ignore(1000, ':');
	res_file[i] >> val;
	res_line.MIP_time = 100 * val / res_line.total_time;  
	res_file[i].ignore(1000, ':');
	res_file[i] >> res_line.nb_node;
	res_file[i].ignore(1000, ':');
	res_file[i] >> res_line.nb_MIP;
	res_file[i].ignore(1000, ':');
	res_file[i] >> res_line.nb_checkER;
	res_file[i].ignore(1000, ':');
	res_file[i] >> res_line.nb_adjust;
      }
      else {
	res_line.feas=0;
	res_line.solved=0;
	res_file[i].ignore(1000, ':');
        res_line.nb_node=0;
	res_file[i].ignore(1000, ':');
	res_line.nb_MIP=0;
	res_file[i].ignore(1000, ':');
        res_line.nb_checkER=0;
	res_file[i].ignore(1000, ':');
	res_line.nb_adjust=0;

	res_file[i].ignore(10000, ':');
	res_file[i] >> res_line.total_time;
      }
    }
    res_file[i].ignore(10000,'*');
    res_file[i].ignore(1000,'\n');
    prout=res_file[i].get();
    res_vec.push_back(res_line);   
    std:: cout << "prout $"<<
      res_line.size << "$ & $" <<
      res_line.total_time<< "$ & $" <<
      res_line.MIP_time<< "$ & $" <<
      res_line.tree_time<< "$ & $" <<
      res_line.solved<< "$ & $" <<
      res_line.nb_node<< "$ & $" <<
      res_line.nb_MIP<< "$ & $" <<
      res_line.nb_checkER<< "$ & $" <<
      res_line.nb_adjust<< "$ \\\\ \n";
    if (res_file[i].eof()) break;
    }
  }
  std:: cout << "\\begin{center}\n";
  std:: cout << "	\\begin{tabular}{|c|cccc"/*cccc*/<<"|}\n";
  std:: cout << "	\\hline\n";
  std:: cout << "	\\#act. &  \\multicolumn{4}{c|}{Méthode de branchement hybride}\\\\ \n ";
  std:: cout << "	&  \\multicolumn{4}{c|}{$\\epsilon=" << argv[2] << "$} \\\\ \n ";
  std:: cout << "	\\hline \n";
  std:: cout << "	\\multicolumn{5}{|l|}{Famille 2}\\\\ \n ";
  std:: cout << "	\\hline \n";
  std:: cout << "  & Tps total(s) & "/*\\%Tps CPLEX & \\%Tps arbre &*/<<" \\%solv.  & \\#n\\oe ud & \\#PLNE "/*& \\#fail. & \\#adj.*/<<" \\\\ \n";
  std:: cout << " \\hline \n";


  for (int j=0;j<5;++j){
    res res_line;
    int nb=0;
    for (uint i=0; i<res_vec.size()-1;++i){
      if (j==0 && res_vec[i].size==10){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.total_time+=res_vec[i].total_time;
	res_line.tree_time+=res_vec[i].tree_time;
	res_line.MIP_time+=res_vec[i].MIP_time;
	res_line.nb_node+=res_vec[i].nb_node;
	res_line.nb_MIP+=res_vec[i].nb_MIP;
	res_line.nb_adjust+=res_vec[i].nb_adjust;
	res_line.nb_checkER+=res_vec[i].nb_checkER;
	res_line.solved+=res_vec[i].solved;
	res_line.feas+=res_vec[i].feas;
      }
      if (j==1 && res_vec[i].size==20){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.total_time+=res_vec[i].total_time;
	res_line.tree_time+=res_vec[i].tree_time;
	res_line.MIP_time+=res_vec[i].MIP_time;
	res_line.nb_node+=res_vec[i].nb_node;
	res_line.nb_MIP+=res_vec[i].nb_MIP;
	res_line.nb_adjust+=res_vec[i].nb_adjust;
	res_line.nb_checkER+=res_vec[i].nb_checkER;
	res_line.solved+=res_vec[i].solved;
	res_line.feas+=res_vec[i].feas;
      }
      if (j==2 && res_vec[i].size==25){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.total_time+=res_vec[i].total_time;
	res_line.tree_time+=res_vec[i].tree_time;
	res_line.MIP_time+=res_vec[i].MIP_time;
	res_line.nb_node+=res_vec[i].nb_node;
	res_line.nb_MIP+=res_vec[i].nb_MIP;
	res_line.nb_adjust+=res_vec[i].nb_adjust;
	res_line.nb_checkER+=res_vec[i].nb_checkER;
	res_line.solved+=res_vec[i].solved;
	res_line.feas+=res_vec[i].feas;
      }
      if (j==3 && res_vec[i].size==30){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.total_time+=res_vec[i].total_time;
	res_line.tree_time+=res_vec[i].tree_time;
	res_line.MIP_time+=res_vec[i].MIP_time;
	res_line.nb_node+=res_vec[i].nb_node;
	res_line.nb_MIP+=res_vec[i].nb_MIP;
	res_line.nb_adjust+=res_vec[i].nb_adjust;
	res_line.nb_checkER+=res_vec[i].nb_checkER;
	res_line.solved+=res_vec[i].solved;
	res_line.feas+=res_vec[i].feas;
      }
      if (j==4 && res_vec[i].size==60){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.total_time+=res_vec[i].total_time;
	res_line.tree_time+=res_vec[i].tree_time;
	res_line.MIP_time+=res_vec[i].MIP_time;
	res_line.nb_node+=res_vec[i].nb_node;
	res_line.nb_MIP+=res_vec[i].nb_MIP;
	res_line.nb_adjust+=res_vec[i].nb_adjust;
	res_line.nb_checkER+=res_vec[i].nb_checkER;
	res_line.solved+=res_vec[i].solved;
	res_line.feas+=res_vec[i].feas;
      }
    }
      if (res_line.feas) {
      res_line.tree_time=res_line.tree_time/res_line.solved;
      res_line.MIP_time=res_line.MIP_time/res_line.solved;
      res_line.nb_node=res_line.nb_node/res_line.solved; 
      res_line.nb_MIP=res_line.nb_MIP/res_line.solved;
      res_line.nb_adjust=res_line.nb_adjust/res_line.solved;
      res_line.nb_checkER=res_line.nb_checkER/res_line.solved;
    }
    
    if (nb){
      res_line.size=res_line.size/nb;
      res_line.total_time=res_line.total_time/nb;
      res_line.solved=100*res_line.solved/nb;    
    }	
    if (nb){
      std:: cout << "$"<<
	//res_line.size << "$ & $" <<
	res_line.total_time<< "$ & $" <<
	//res_line.MIP_time<< "$ & $" <<
	res_line.tree_time<< "$ & $" <<
	res_line.solved<< "$ & $" <<
	res_line.nb_MIP<< "$ & $" <<
	res_line.nb_node/*<< "$ & $" <<
	res_line.nb_checkER<< "$ & $" <<
	res_line.nb_adjust*/<< "$ \\\\ \n";
    }
  }
  std::cout << "\\hline \n";
  std::cout << "\\end{tabular}\n";
  if (atoi(argv[4])==1)
    std::cout << "\\captionof{table}{Results of the hybrid branch-and-bound with heuristic "<< argv[3] << " and ER satisfiability}\n";
  if (atoi(argv[4])==3)
    std::cout << "\\captionof{table}{Results of the hybrid branch-and-bound with heuristic "<< argv[3] << " and flow satisfiability}\n";
  if (atoi(argv[4])==4)
    std::cout << "\\captionof{table}{Results of the hybrid branch-and-bound with heuristic "<< argv[3] << " and ER+flot satisfiability}\n";
  std::cout << "\\end{center}\n";

  res_file[i].close();
  

  return 0;
}
