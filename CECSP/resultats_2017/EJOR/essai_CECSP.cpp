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
  double best_obj;
  int nb_node;
  int nb_cut;
  res();
};

res::res() : size(0), root_obj(0.0) , root_gap(0.0), obj_first_sol (0.0), 
	     time_first_sol (0.0), gap_first_sol (0.0), status(0),time_end (0.0), 
	     final_obj (0.0), final_gap (0.0), nb_node(0), nb_cut(0){}

struct cell{
  string name;
  double best;
};

int main( int argc, char* argv[]){
  std::ifstream res_file(argv[1],std::ios::in);
  //  std::ifstream res_file2(argv[2],std::ios::in);
  if (!res_file)  {    
    std::cout << "Ouverture res impossible" << std::endl;
    return 1;
  }

  /* std::vector<cell> cell_vec;
  cell_vec.resize(1000);
  int cpt=0;
   while (!res_file2.eof()){
    res_file2 >> cell_vec[cpt].name; 
    res_file2 >> cell_vec[cpt].best; 
    cpt++;
  }
  res_file2.close();
  */
  std::string trash;
  char prout;
  std::vector<res> res_vec;
  while (!res_file.eof()){  
    string inst_name;
    res res_line;
    /*getline(res_file, inst_name);
    for (int i=0;i<cell_vec.size();++i)
      if (inst_name == cell_vec[i].name)
	res_line.best_obj=cell_vec[i].best;

	res_file.seekg(-inst_name.size()-1, res_file.cur);*/
    res_file.ignore(4);
    res_file >> res_line.size;
    res_file.ignore(1000,'\n');
    prout=res_file.get();
    if (prout != '*'){
      res_file.ignore(1000,':');
      res_file >> res_line.root_obj;
      res_file >> trash;
      if (trash!= "..."){
	res_file.ignore(1000, ':');
	res_file >> res_line.obj_first_sol;
	res_file.ignore(1000,':');
	res_file >> res_line.time_first_sol;
	res_file.ignore(1000,':');
	res_file >> trash ;
	prout=res_file.get();  
	prout=res_file.get();  
	if (prout != '*'){
	  res_file.ignore(1000,':');
	  res_file >> trash ;
	  if (trash == "Optimal")
	    res_line.status=3;
	  if (trash == "Feasible")
	    res_line.status=2;
	  res_file >> trash;
	  res_file >> res_line.time_end;
	  res_file.ignore(1000,':');
	  res_file >> res_line.final_obj;
	  res_file.ignore(1000,':');
	  res_file >> res_line.final_gap;
	  res_file.ignore(1000,':');
	  res_file >> res_line.nb_node; 
	  //	res_file.ignore(1000,'\n'); 
	  res_file >> trash;
	  if (trash == "number"){
	    res_file.ignore(1000,':');
	    res_file >> res_line.nb_cut; 
	  }
	}
      }
    }
    res_file.ignore(10000,'*');
    res_file.ignore(1000,'\n'); 
    res_vec.push_back(res_line);  /*
   std::cout << fixed << setprecision(2);
   std:: cout << "$"<<inst_name<< " " << 
	    res_line.size << " $& $" <<    
	    res_line.root_gap << "$ & $" <<
	    res_line.time_first_sol << "$ & $" <<
	    res_line.gap_first_sol << "$ & $" <<
	    res_line.time_end << "$ & $" <<
	    res_line.final_gap << "$ & $" <<
	    res_line.nb_node << "$ & $" <<
	    res_line.nb_cut<< " $ \\\\ \n";*/

      prout=res_file.get(); 
    if (res_file.eof()) break; 
      else res_file.seekg(-1,res_file.cur);   
  }
  
  for (int j=0;j<4;++j){
    res res_line;
    res_line.time_end=0.0;
    int nb=0;
    int nb_sol=0;
    int nb_opt=0;
    int nb_feas=0;
    for (uint i=0; i<res_vec.size()-1;++i){
      if (j==0 && res_vec[i].size==10 ){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>=1){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	  res_line.nb_cut+= res_vec[i].nb_cut;
	}
	if (res_vec[i].status>=2){
	  nb_feas++;
	  res_line.root_gap+=100* (1 - res_vec[i].root_obj/res_vec[i].best_obj); 
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.gap_first_sol+=100*( 1 - 
					res_vec[i].best_obj/res_vec[i].obj_first_sol); 
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==2) res_line.final_gap+=res_vec[i].final_gap;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }

      if (j==1 && res_vec[i].size==20){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	  res_line.nb_cut+= res_vec[i].nb_cut;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	  res_line.root_gap+=100* (1 - res_vec[i].root_obj/res_vec[i].best_obj ); 
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.gap_first_sol+=100*( 1 - 
					res_vec[i].best_obj/res_vec[i].obj_first_sol); 
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==2) res_line.final_gap+=res_vec[i].final_gap;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }

      if (j==2 && res_vec[i].size==25){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	  res_line.nb_cut+= res_vec[i].nb_cut;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	  res_line.root_gap+=100* (1 - res_vec[i].root_obj/res_vec[i].best_obj ); 
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.gap_first_sol+=100*( 1 - 
					res_vec[i].best_obj/res_vec[i].obj_first_sol); 
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==2) res_line.final_gap+=res_vec[i].final_gap;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }

      if (j==3 && res_vec[i].size==30){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	  res_line.nb_cut+= res_vec[i].nb_cut;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	  res_line.root_gap+=100* (1 - res_vec[i].root_obj/res_vec[i].best_obj ); 
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.gap_first_sol+=100*( 1 - 
					res_vec[i].best_obj/res_vec[i].obj_first_sol); 
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==2) res_line.final_gap+=res_vec[i].final_gap;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
    }
    if (nb!=0) {
      res_line.size=res_line.size/nb;
      res_line.time_end= res_line.time_end/nb;
    }    
    if (nb_feas!=0){
      res_line.root_gap= res_line.root_gap/nb_feas;
      res_line.time_first_sol= res_line.time_first_sol/nb_feas;
      res_line.gap_first_sol= res_line.gap_first_sol/nb_feas;
    }
    if (nb_opt!=nb_feas)
      res_line.final_gap= 100*res_line.final_gap/(nb_feas-nb_opt);
    else 
      res_line.final_gap= 0;
    if (nb_sol!=0){
    res_line.nb_node= res_line.nb_node/nb_sol;
    res_line.nb_cut=res_line.nb_cut/nb_sol;
    }
    
    if (nb_sol==0 && j==0) res_line.size=10;
    if (nb_sol==0 && j==1) res_line.size=20;
    if (nb_sol==0 && j==2) res_line.size=25;
    if (nb_sol==0 && j==3) res_line.size=30;
    if (nb_sol==0 && j==4) res_line.size=60;
    //  if (j!=0 && j!=4){
      std::cout << fixed << setprecision(2);
      std:: cout << "$"<<
        res_line.size << " $& $" <<    
	//res_line.root_gap << "$ & $" <<
	//res_line.time_first_sol << "$ & $" <<
	//res_line.gap_first_sol << "$ & $" <<
	res_line.time_end << "$ & $" <<
	//res_line.final_gap << "$ & $" <<
	100*nb_opt/nb<< "$ & $" <<
	100*nb_sol/nb << "$ & $" <<
	//res_line.nb_node << "$ & $" <<
	//res_line.nb_cut
	100* nb_feas/nb << " $ \\\\ \n";
      //  }
  }
  
  res_file.close();
  //res_file2.close();
  
  return 0;
}
