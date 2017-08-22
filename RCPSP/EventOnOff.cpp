#include <iostream>
#include <fstream>
#include <ilcplex/ilocplex.h>
#include <string>
#include <cstring>
#include <time.h>

ILOSTLBEGIN

using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;

//---------------------------------------------------------------------------------------------------

typedef IloArray<IloIntArray> IloIntMatrix;
typedef IloArray<IloNumArray> IloNumMatrix;
//typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;

typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloIntVarArray> IloIntVarMatrix;

string  NFichier, NFichier0, FicVerif, NFichierT;
char NomFichier[75], FichierVerif[75], NomFichierT[75];

IloBoolVarArray temp;
IloIntArray B;
IloIntArray NbSucc;
IloIntArray p;
IloIntMatrix r, Succ;

IloNumVarArray t;
IloIntVarMatrix z;
IloNumVar Cmax;

IloEnv env;
IloNum start;
IloNum finish;

//---------------------------------------------------------------------------------------------------

IloInt N, n, K; //i & j taches, N Nb tache, e event, n nb event, k ressource, K nb ressource
int ES[100], LS[100], Blabla;
//--------------------------------------------------------------------------------------------------
IloInt i, j, Niv[32], infini = 100000;
bool modif = false, precede = false, Etat[32], FictifTask = false;
int NbPrec[32], Prec[32][31], MaxNiv = 0;

//---------------------------------------------------------------------------------------------------

void define_data(IloEnv env) {
   
   cout<<endl<<"   Initialisation des donnees"<<endl;
   cout<<" 	Lecture du fichier de donnees"<<endl;
   int e, g, k, a, Horiz=0, MPM=0, Duedate=0, Hm=0, q=0,f=0, h=0;
   char str1[74];
   strncpy(NomFichier, NFichier.c_str(), 100 );
   ifstream LectureCommentaire(NomFichier,ios::in);
   if(!LectureCommentaire){
      cerr<<"OUVERTURE DU FICHIER PSPLIB IMPOSSIBLE"<<endl;
      exit(1);
   };

   cout<<"	Fichier de donnees : "<<NFichier<<" Ouvert"<<endl;

/////////////////////////////////////////////////////////////////////
   LectureCommentaire>>N;
   if (FictifTask == false) N=N-2;
   LectureCommentaire>>K;
   n = (int)(N*(1.0)); // servant de parametre pour reduire le même d'evenement
   cout <<" 	K = "<<K<<"  N = "<<N<<" 	n = "<<n<<endl;
   p = IloIntArray(env,N, 0, infini);
   B = IloIntArray(env,K, 0, infini);
   r = IloIntMatrix(env,N);
   NbSucc = IloIntArray(env,N, 0, N-1);
   t = IloNumVarArray(env,n, 0, infini);
   for(i=0;i<n;i++){
   char nom[50];
         sprintf(nom,"t(%d)",i);
         t[i].setName(nom);
   };
 
   Succ = IloIntMatrix(env,N);
   z = IloIntVarMatrix(env,N); 
   Cmax = IloNumVar(env, 0, infini);
   Cmax.setName("Makespan");
 cout<<"	Initialisation phase 1 terminee"<<endl;

////////////////////////////////////////////////////

   for (i=0;i<N;i++){
       z[i] = IloIntVarArray(env,n,0,1);
       for (j=0;j<n;j++) {
         char nom[50];
         sprintf(nom,"z(%d,%d)",i,j);
         z[i][j].setName(nom);
       }; 
       r[i] = IloIntArray(env,K, 0, infini);
   };
   cout<<"	Initialisation phase 2 terminee"<<endl;

///////////////////////////////////////////////////////////
   for(k=0;k<K;k++)  LectureCommentaire>>B[k];

   IloInt G;

   if (FictifTask == false){
      int step,NbSucc;
      LectureCommentaire>>step;
      for(k=0;k<K;k++) LectureCommentaire>>step;
      LectureCommentaire>>NbSucc;
      for(j=0;j<NbSucc;j++) LectureCommentaire>>step;
   };

   for(i=0;i<N;i++){
       LectureCommentaire>>p[i];
       for(k=0;k<K;k++)  LectureCommentaire>>r[i][k];
       LectureCommentaire>>NbSucc[i];
       Succ[i] = IloIntArray(env, 30, 0, infini);
       for(j=0;j<NbSucc[i];j++){
             LectureCommentaire>>Succ[i][j];
             Succ[i][j] = Succ[i][j]-1; 
             if (FictifTask == false) Succ[i][j] = Succ[i][j]-1;
       };
   };
   cout<<"	Initialisation phase 3 terminee"<<endl;
/////////////////////////////////////////////////////////////
   cout<<"	Lecture du fichier de donnees ok"<<endl;
   LectureCommentaire.close();
   cout <<"   Initialisation des donnees terminee --  OK"<< endl;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void Affiche()
{
  cout<<endl<<"Affichage des données"<<endl;
  cout<<" N = "<<N<<endl;
  int i=0, j=0,k=0;
  cout<<"		i"<<'\t'<<"p[i]"<<'\t';
  for(k=0;k<K;k++) cout<<"ri"<<k<<'\t';
  cout<<"Nbprec"<<'\t'<<"	Succ"<<endl;
  for(i=0;i<N;i++) {
    cout<<"		"<<i<<'\t'<<p[i]<<'\t';
    for(k=0;k<K;k++) cout<<r[i][k]<<'\t';
    cout<<NbSucc[i]<<'\t';
    for(j=0;j<NbSucc[i];j++) cout<<'\t'<<Succ[i][j];
    cout<<endl;
  };
  cout<<"		B["<<i<<"] = "<<'\t';
  for(i=0;i<K;i++) cout<<B[i]<<'\t';
  cout<<endl;
}



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
///*
void TimeWindows() {
        int i, M;
        cout<<"   Fichier de fenetre temporelle : "<<NFichierT<<endl;
        strncpy( NomFichierT, NFichierT.c_str(), 75 );
        ifstream File(NomFichierT);
        if (!File){
                cerr << "Erreur d'ouverture du fichier en lecture..." << endl;
                exit(1);
        };
        i=0;
        File>>M;
        if (FictifTask == false){
           M = M-2;
	   File>>Blabla>>Blabla>>Blabla;
	};
        while (!File.eof() ){
             if(i<M){
                File>>Blabla>>ES[i]>>LS[i];
                i=i+1;
             } 
             else if(i=M)File>>Blabla>>ES[M]>>LS[M];
        };
        cout<<"		i	ESi	LSi"<<endl;
        for(i=0;i<M;i++) cout<<"		"<<i<<'\t'<<ES[i]<<'\t'<<LS[i]<<endl;
        cout<<"   Lecture fenetre temporelle terminee"<<endl;
}


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



void Event2() {
    IloInt e, f ,i, j, k, ep;
    IloModel model(env);

    try {
	define_data(env);
        Affiche();
        
	//*/
       ///////////////// PROGRAMME LINEAIRE ////////////////////////////

        // Objective Function: Minimize Cost
        model.add(IloMinimize(env,Cmax));
       
        for(i=0;i<N;i++){
           model.add(Cmax >= t[0]+(z[i][0])*p[i]);
	   for(e=1;e<n;e++) model.add(Cmax >= t[e]+(z[i][e]-z[i][e-1])*p[i]);
	};

        cout <<"   Fonction Objective     ---     OK" << endl;


        // Contrainte #2 -- Contrainte de fixation de la date de l'evenement 0 ==>"t0 = 0"
        model.add(t[0] == 0);
        cout <<"   Contrainte de fixation de la date de l'evenement 0    OK" << endl;

        // ///////////  LES COUPES /////////////
	//*
        cout<<"LS = "<<LS[N]<<endl;
        for(i=0;i<N;i++){
          model.add(t[0]<= (LS[i]*z[i][0]) + LS[N]*(1 - z[i][0]) );
          for(e=1;e<n;e++){
              model.add(t[e]<= (LS[i]*(z[i][e]-z[i][e-1])) + LS[N]*(1 - (z[i][e]-z[i][e-1])) ); //ajouter le  20 octobre 08 
              model.add(t[e]>= ES[i]*z[i][e]);
          };
	};
        
         if (FictifTask == true){
            cout<<"	"<<ES[N-1]<<" <= Cmax <= "<<LS[N-1]<<endl;
            model.add(Cmax>=ES[N-1]);//model.add(t[n-1]>=ES[N-1]);
            model.add(Cmax<=LS[N-1]);//model.add(t[n-1]<=LS[N-1]);
         };
         
         if (FictifTask == false){
            cout<<"	"<<ES[N]<<" <= Cmax <= "<<LS[N]<<endl;
            model.add(Cmax>=ES[N]);//model.add(t[n-1]>=ES[N]);
            model.add(Cmax<=LS[N]);//model.add(t[n-1]<=LS[N]);
         };
           

         cout <<"   Couple      ---     OK" << endl;





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//*
        // Contrainte #3 -- Contrainte de liaison des variables te, zie et pi
	for(i=0;i<N;i++){
           for(f=1;f<n;f++) model.add(t[f] >= t[0] +((z[i][0]) - (z[i][f] - z[i][f-1])-1)* p[i]);
           for(e=1;e<=n-1;e++)
	      for(f=e+1;f<n;f++)
	          model.add(t[f] >= t[e] +((z[i][e] -z[i][e-1]) - (z[i][f] - z[i][f-1])-1)* p[i]);
        };
	cout <<"   Contrainte de liaison des variables te, zie et pi    OK" << endl;
//*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
       // Contrainte #6 -- Contrainte de fixation des evenements consecutifs e et e+1
        for(e=0;e<n-1;e++) model.add(t[e+1]>=t[e]);
        cout<<"   Contrainte de fixation des evenements consecutifs e et e+1     OK" << endl;


///////////////////////////////////////////////////////////////

//// Nouvelle contrainte de contiguité/ non-Preemption

   /// Contrainte zie <= 1+Xie
//*
        for(i=0;i<N;i++){
	   for(ep=0;ep<=e-1;ep++)  model.add(z[i][ep]<= (1-(z[i][0])));
           for(ep=e;ep<=n-1;ep++)  model.add(z[i][ep]<= (1+(z[i][0])));
	   for(e=1;e<=n-1;e++){
              for(ep=0;ep<=e-1;ep++)  model.add(z[i][ep]<= (1-(z[i][e]-z[i][e-1])));
              for(ep=e;ep<=n-1;ep++)  model.add(z[i][ep]<= (1+(z[i][e]-z[i][e-1])));
           };
	};
//*/

 // sum_0^e-1(ziv)<=e(1-Xie) 

/*
        for(i=0;i<N;i++)
	   for(e=1;e<=n-1;e++){
              IloExpr e30(env), e31(env);
              for(ep=0;ep<=e-1;ep++)  e30 = e30 + z[i][ep];
              model.add(e30 <=e*(1-(z[i][e]-z[i][e-1])));
              for(ep=e;ep<=n-1;ep++) e31 = e31 + z[i][ep]; 
              model.add(e31 <=e*(1+(z[i][e]-z[i][e-1])));
           };
        
//*/

        cout <<"   contrainte de contiguite     OK" << endl;


/////////////////////////////////////////////////////////////////////

/*
        for(e=0;e<=n-1;e++) 
	   for(f=e+1;f<n;f++)
	      for(j=0;j<N;j++)
                 for(i=j+1;i<N;i++)
                     model.add(t[f]>=t[e]+z[i][e]+z[j][f]-1);
                
        cout <<"   Contrainte de rupture des symetries    OK" << endl;
//*/

//////////////////////////////////////////////////////////////////////

///contrainte de demarrage
 ///*       // Contrainte #6
        for(i=0;i<N;i++) {
             IloExpr e1(env);
             for(e=0;e<n;e++) e1=e1 + z[i][e];
             model.add(e1>=1);
             e1.end();
        };
        cout <<"   Contrainte de demarrage des taches     OK" << endl;
//*/

//////////////////////////////////////////////////////////////////////

///Contrainte de precedence
//*
        for(e=0;e<n;e++)
          for(i=0;i<N;i++)
             for(j=0;j<NbSucc[i];j++){
                  if(NbSucc[i]>0 && Succ[i][j] <N){
                  	IloInt w;
                  	w = Succ[i][j];
                  	for(ep=0;ep<=e;ep++) model.add(z[i][e]+ z[w][ep]<=1);
            	  };
	     };

        cout <<"   Contrainte de precedence     OK" << endl;

//*/
//////////////////////////////////////////////////////////////////////


        // Contrainte #9 --- contrainte de ressource
//*
        for(e=0;e<n;e++)
	   for(k=0;k<K;k++){
		  	
			IloExpr e4(env);
			for(i=0;i<N;i++) e4 = e4+ r[i][k]*z[i][e];
			model.add(e4<=B[k]);
			e4.end();
           };
        cout <<"   Contrainte de ressource     OK" << endl;


//*/

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// Optimize
        IloCplex cplex(model);

	//cplex.exportModel("29jan08/modelEvtUniqjA8_B13.lp");

        //************************PARAMATRES CPLEX****************************
        cplex.setParam(IloCplex::TiLim,500.0);
        cplex.setParam(IloCplex::MIPEmphasis,IloCplex::MIPEmphasisFeasibility);
                //************** MIP Cuts **********************
        /*cplex.setParam(IloCplex::AggCutLim,0);
        cplex.setParam(IloCplex::CutsFactor,1.0);
        cplex.setParam(IloCplex::Cliques,-1);
        cplex.setParam(IloCplex::Covers,-1);
        cplex.setParam(IloCplex::DisjCuts,-1);
        cplex.setParam(IloCp#include <time.h>lex::FlowCovers,0);
        cplex.setParam(IloCplex::FlowPaths,-1);
        cplex.setParam(IloCplex::FracCuts,-1);
        cplex.setParam(IloCplex::GUBCovers,-1);
        cplex.setParam(IloCplex::ImplBd,-1);
        cplex.setParam(IloCplex::MIRCuts,-1);
        cplex.setParam(IloCplex::ZeroHalfCuts,-1);
        cplex.setParam(IloCplex::FracPass,0);
        cplex.setParam(IloCplex::FracCand,200);
        cplex.setParam(IloCplex::EachCutLim,0);
        cplex.setParam(IloCplex::CutPass,-1); */
        //********************************************************************

        cout << "   Appel de Cplex -- OK " << endl;
        //cplex.setOut(env.getNullStream());
        //cplex.setWarning(env.getNullStream());
        start = cplex.getTime();
        cplex.solve();
        finish =cplex.getTime();
        cout<<" Fin de la resolution"<<endl<<endl;

	std::ofstream SolOuv("20-Octobre/testsOnOff-1-8(n=1N-TW).txt", std::ios_base::app );
        if(!SolOuv){
          cerr<<" OUVERTURE DU FICHIER DE RESULTAT IMPOSSIBLE"<<endl;
          exit(1);
        };
        SolOuv<<endl<<NFichier<<'\t'<<finish<<'\t';
        SolOuv<<cplex.getNintVars()<<'\t'<<cplex.getNbinVars()<<'\t';
        SolOuv<<cplex.getNsemiContVars()<<'\t'<<cplex.getNsemiIntVars()<<'\t';
        SolOuv<<cplex.getObjValue();
        if(cplex.getStatus()==IloAlgorithm::Infeasible||cplex.getStatus()==IloAlgorithm::InfeasibleOrUnbounded)SolOuv<< "**";
        SolOuv<<'\t';
        SolOuv<<cplex.getStatus();
        if (cplex.getStatus() == IloAlgorithm::Infeasible) SolOuv << "Pas de Solution" << endl;
        SolOuv.close();

        cout<<"--------------------------------------------------------------------------"<<endl;
        cout<<" Status                             : "<<cplex.getStatus()<<endl;
        cout<<" Cmax                               : "<<cplex.getObjValue()<<endl;
        cout<<" Temps de resolution                : "<<finish<<endl;
        cout<<" Nombre de variables intiers        : "<<cplex.getNintVars()<<endl;
        cout<<" Nombre de variables binaires       : "<<cplex.getNbinVars()<<endl;
        cout<<" Nombre de variables Semi-Continues : "<<cplex.getNsemiContVars()<<endl;
        cout<<" Nombre de variables Semi-entiers   : "<<cplex.getNsemiIntVars()<<endl;
        cout<<"--------------------------------------------------------------------------"<<endl;

        cplex.exportModel("toto.lp");

        std::ofstream writeOut("Octobre/test1.txt", std::ios_base::trunc );
        if(!writeOut){
           cerr<<"Impossible de creer le fichier de solution"<<endl;
           exit(1);
        };
        writeOut<<N<<'\t'<<K<<endl;
        for(int k=0;k<K;k++) writeOut<<B[k]<<'\t';
        for(int i=0;i<N;i++){
           writeOut<<endl<<p[i]<<'\t';
           for(int k=0;k<K;k++) writeOut<<r[i][k]<<'\t'; 
           writeOut<<NbSucc[i];
           for(int j=0;j<NbSucc[i];j++) writeOut<<'\t'<<Succ[i][j]; 
        };
   
        for(e=0;e<n;e++){
           writeOut<<endl<<endl<<cplex.getValue(t[e])<<endl;
           for(int i=0;i<N;i++)
           if (cplex.getValue(z[i][e])>=0.5) writeOut<<'\t'<< 1;
	   else  writeOut<<'\t'<<0;
       };
       writeOut<<endl<<cplex.getObjValue();
    }
      catch (IloException& ex)
         {
            cerr << "Error: " << ex << endl;
      }
      catch (...)
         {
            cerr << "Error" << endl;
      }
      env.end();
};


// ***************** PROGRAMME PRINCIPAL ***********************************

int main(int argc, char *argv[]){
   cout<<"-----------------------------------------"<<endl;
   cout<<"---- Formalution a evenement ON OFF  ----"<<endl;
   cout<<"-----------------------------------------"<< endl<<endl;
   cout<<" argc = "<<argc<<" argv = "<<argv[0]<<" argv = "<<argv[1]	     <<endl<<endl;
   NFichier = argv[1];
   NFichier0 = argv[1];
   NFichier = "./instances/j30RcpModif/jA15_B15/" + NFichier +".rcp";
   NFichierT = "./instances/j30RcpModif/jA15_B15/" + NFichier0 + ".rcp.heurW";
   cout<<"Fichier instance          : "<<NFichier<<endl;
   cout<<"Fichier fenetres de temps : "<<NFichierT<<endl;
   TimeWindows();
   Event2();
   return 0;
}
    
//---------------------------------------------------------------------------------------------------
