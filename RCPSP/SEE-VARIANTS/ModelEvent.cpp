#include "entete2.h"
ILOSTLBEGIN  

//---------------------------------------------------------------------------------------------------
IloNumVarArray t; 
//IloIntVarMatrix x, y;
IloNumVarMatrix x, y;
IloNumVarMatrix b;

//---------------------------------------------------------------------------------------------------
void Event2() {
    IloInt e, f ,i, j, k;
    IloModel model(env);

    try {
	define_data(env);
        Affiche();
        TimeWindows();
        ShowTimeWindows();
        n=N+1;
        cout<<"N = "<<N<<", n = "<<n<<endl;
        t = IloNumVarArray(env,n, 0, infini);
        b = IloNumVarMatrix(env,n);
//        x = IloIntVarMatrix(env,N); 
//        y = IloIntVarMatrix(env,N); 
        x = IloNumVarMatrix(env,N); 
        y = IloNumVarMatrix(env,N); 

        for (e=0;e<n;e++) b[e] = IloNumVarArray(env,K, 0, infini);

        for (i=0;i<N;i++){     
//           x[i] = IloIntVarArray(env,n,0,1); 
//           y[i] = IloIntVarArray(env,n,0,1); 
           x[i] = IloNumVarArray(env,n,0,1); 
           y[i] = IloNumVarArray(env,n,0,1); 
        };


 	cout<<"ici 1 ."<<endl;
	//*
        for(i=0;i<N;i++)
          for(e=0;e<n;e++){
              //model.add(t[e]<= ((LS[i]+p[i])*x[i][e]));
	      model.add(t[e]<= LS[i]*x[i][e] + LS[N-1]*(1 - x[i][e]));
              model.add(t[e]>= ES[i]*x[i][e]);
	      model.add(t[e]<= (LS[i]+p[i])*y[i][e]+LS[N-1]*(1 - y[i][e]));
	      model.add(t[e]>= (ES[i]+p[i])*y[i][e]);
          };//*/
        cout<<"ici 2"<<endl;
         model.add(x[0][0]==1);
         model.add(y[0][0]==1);
         model.add(x[N-1][n-1]==1);
         model.add(y[N-1][n-1]==1);
         model.add(t[n-1]>=ES[N-1]);
         model.add(t[n-1]<=LS[N-1]);
         for(e=1;e<n;e++){
            model.add(x[0][e]==0);
            model.add(y[0][e]==0);  
         };
         cout<<"ici 3"<<endl;
         for(e=0;e<n-1;e++){
            model.add(x[N-1][e]==0);
            model.add(y[N-1][e]==0);  
         }; 
         cout<<"ici 4"<<endl;

         cout <<"   Couple      ---     OK" << endl;


       /////////////////////////////////////////////

        //Objective Function: Minimize Cost
        model.add(IloMinimize(env,t[n-1]));
        cout <<"   Fonction Objective     ---     OK" << endl;


        // Contrainte #2
        model.add(t[0] == 0);
        cout <<"   Contrainte N°2     ---     OK" << endl;


        // Contrainte #3
        for(e=0;e<n;e++)
	   for(f=e;f<n;f++)//ici
		for(i=0;i<N;i++) model.add(t[f] >= t[e] + p[i] * x[i][e] - p[i] * (1-y[i][f]) );
	   cout <<"   Contrainte N°3     ---     OK" << endl;


       // Contrainte #4
       
        for(e=0;e<n-1;e++) model.add(t[e+1]>=t[e]);
        cout <<"   Contrainte N°4     ---     OK" << endl;



 /*/
        for(e=0;e<n;e++)
	   for(f=e+1;f<n;f++)
	      for(i=0;i<N-1;i++)
                 for(j=i+1;j<N;j++)
                 //for(j=0;j<NbSucc[i];j++){
                    //IloInt w;
                    //w = Succ[i][j];
                    //model.add(t[f]>=t[e]+x[i][e]+x[w][f]-1);
                    model.add(t[f]>=t[e]+x[i][e]+x[j][f]-1);
                 //};
        

        for(e=0;e<n-1;e++)
	   for(f=e+1;f<n;f++)
	      for(i=0;i<N;i++)
                 for(j=0;j<NbSucc[i];j++){
                    IloInt w;
                    w = Succ[i][j];
                    model.add(t[f]>=t[e]+y[i][e]+y[w][f]-1);
                 };
        */



 ///*       // Contrainte #5
        for(i=0;i<N;i++) {
             IloExpr e1(env);
             for(e=0;e<n;e++) e1=e1 + x[i][e];
             model.add(e1==1);
             //model.add(e1<=1);
             //model.add(e1>=1);
             e1.end();
        };
      // */
       
        cout <<"   Contrainte N°5     ---     OK" << endl;


        // Contrainte #6
// /*       
          for(i=0;i<N;i++){
             IloExpr e2(env);
             for(e=0;e<n;e++) e2=e2+y[i][e];
             model.add(e2==1);
             //model.add(e2<=1);
             //model.add(e2>=1);
             e2.end();
        };
  /*      
        for(e=1;e<n;e++){
             IloExpr e2(env);
             for(i=0;i<N+2;i++) e2=e2+y[i][e];
             model.add(e2==1);
             //model.add(e2<=1);
             //model.add(e2>=1);
             e2.end();
        };*/
//*/


        cout <<"   Contrainte N°6     ---     OK" << endl;

//ici 6 evenement de fin posterieur a l'evenement de debut
     	
       for(i=0;i<N;i++)
           for(e=0;e<n;e++){
                IloExpr e6(env), e7(env);
                IloInt v=0;
                for(v=0;v<=e;v++) e6 = e6 +x[i][v];
                for(v=0;v<=e;v++) e7 = e7 +y[i][v];
		model.add(e7<=e6);
                e6.end();e7.end();
           };

   //à ici 6
/*
// alternative Sigrid
for (i=0;i<N;i++) {

   IloExpr ex1(env);
   for (e=0;e<n;e++) 
     ex1+=e*y[i][e];
   for (e=0;e<n;e++)
     ex1-=e*x[i][e];
   if (p[i]==0)
   	model.add(ex1>=0);
   else
        model.add(ex1>=1);
   ex1.end();
}
*/


        // Contrainte #7
       for(i=0;i<N;i++)
           for(j=0;j<NbSucc[i];j++)
	      for(e=0;e<n;e++){
		    IloExpr e3(env), e4(env);
		    IloInt u, v, w;
                    w=Succ[i][j];
		    for(u=e;u<n;u++) e3 = e3 + y[i][u];
	            for(v=0;v<e;v++)  e4 = e4 + x[w][v];
	            model.add(e3+e4<=1);
	            e3.end();
		    e4.end();
	      };

/*for(e=0;e<=n;e++) 
    for(i=0;i<N+1;i++)
       for(j=0;j<NbSucc[i];j++){
          IloExpr e20(env);
          IloInt ep, w;
          w=Succ[i][j];
          for(ep=0;ep<e;ep++) e20 = e20 + x[w][ep];
          model.add(y[i][e] + e20<=1);
          e20.end();
      };
*/



        cout <<"   Contrainte N°7     ---     OK" << endl;


        // Contrainte #8
        
        for(k=0;k<K;k++){
             IloExpr e5(env);
             for(i=0;i<N;i++) e5 = e5 + r[i][k] * x[i][0];
             model.add(b[0][k] == e5);
	     e5.end();
        };
	cout <<"   Contrainte N°8     ---     OK" << endl;

        // Contrainte #9
        for(e=1;e<n;e++)
	   for(k=0;k<K;k++){
			IloExpr e4(env);
			for(i=0;i<N;i++) e4 = e4+ r[i][k]*x[i][e] - r[i][k]*y[i][e];
			model.add(b[e][k]==b[e-1][k] + e4);
			e4.end();
           };
        cout <<"   Contrainte N°9     ---     OK" << endl;


        // Contrainte #10
        for(e=0;e<n;e++)
	   for(k=0;k<K;k++) model.add(b[e][k]<=B[k]);
        cout <<"   Contrainte N°10     ---     OK" << endl;

        //* // Contrainte #10 --- contrainte de couplage des evenements
/*
	for(e=0;e<n;e++){
           IloExpr e50(env),e60(env);
           for(i=0;i<N;i++) e50 = e50+x[i][e];
           for(i=0;i<N;i++) e60 = e60+y[i][e];
           model.add(e50<=1);
           model.add(e60<=1);
        };

//*/

	// Optimize
        IloCplex cplex(model);

	//cplex.exportModel("29jan08/modelEvtUniqjA8_B13.lp");

        //************************PARAMATRES CPLEX****************************
	cplex.setParam(IloCplex::Threads, 1);
        cplex.setParam(IloCplex::TiLim,600.0);
        cplex.setParam(IloCplex::MIPEmphasis,IloCplex::MIPEmphasisFeasibility);
                //************** MIP Cuts **********************
        /*
        cplex.setParam(IloCplex::AggCutLim,0);
        cplex.setParam(IloCplex::CutsFactor,1.0);
        cplex.setParam(IloCplex::Cliques,-1);
        cplex.setParam(IloCplex::Covers,-1);
        cplex.setParam(IloCplex::DisjCuts,-1);
        cplex.setParam(IloCplex::FlowCovers,0);
        cplex.setParam(IloCplex::FlowPaths,-1);
        cplex.setParam(IloCplex::FracCuts,-1);
        cplex.setParam(IloCplex::GUBCovers,-1);
        cplex.setParam(IloCplex::ImplBd,-1);
        cplex.setParam(IloCplex::MIRCuts,-1);
        cplex.setParam(IloCplex::ZeroHalfCuts,-1);
        cplex.setParam(IloCplex::FracPass,0);
        cplex.setParam(IloCplex::FracCand,200);
        cplex.setParam(IloCplex::EachCutLim,0);
        cplex.setParam(IloCplex::CutPass,-1);
        */
        //********************************************************************

        cout << "   Appel de Cplex -- OK " << endl;
        //cplex.setOut(env.getNullStream());
        //cplex.setWarning(env.getNullStream());
        start = cplex.getTime();
        cplex.solve();
        finish =cplex.getTime();
        cout<<" Fin de la resolution"<<endl<<endl;


        std::ofstream SolOuv("SEE-disaggr-PL.txt", std::ios_base::app );
        if(!SolOuv){
          cerr<<" OUVERTURE DU FICHIER DE RESULTAT IMPOSSIBLE"<<endl;
          exit(1);
        };
        SolOuv<<endl<<NFichier<<'\t'<<finish<<'\t';
        SolOuv<<cplex.getNintVars()<<'\t'<<cplex.getNbinVars()<<'\t';
        SolOuv<<cplex.getNsemiContVars()<<'\t'<<cplex.getNsemiIntVars()<<'\t';
        SolOuv<<cplex.getObjValue();
        if ( cplex.getStatus() == IloAlgorithm::Infeasible ||cplex.getStatus() == IloAlgorithm::InfeasibleOrUnbounded )
          SolOuv<< "***";
        SolOuv<<'\t';
        //if(isNull(cplex.getObjValue())) SolOuv<<"***";
        SolOuv<<cplex.getStatus();
        
        
        cout<<"--------------------------------------------------------------------------"<<endl;
        cout<<" Status                             : "<<cplex.getStatus()<<endl;
        cout<<" Cmax                               : "<<cplex.getObjValue()<<endl;
        cout<<" Temps de resolution                : "<<finish<<endl;
        cout<<" Nombre de variables intiers        : "<<cplex.getNintVars()<<endl;
        cout<<" Nombre de variables binaires       : "<<cplex.getNbinVars()<<endl;
        cout<<" Nombre de variables Semi-Continues : "<<cplex.getNsemiContVars()<<endl;
        cout<<" Nombre de variables Semi-entiers   : "<<cplex.getNsemiIntVars()<<endl;
        if (cplex.getStatus() == IloAlgorithm::Infeasible) SolOuv << "Pas de Solution" << endl;
        SolOuv.close();
        cout<<"--------------------------------------------------------------------------"<<endl;

  
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
   cout<<"-------   Formalution a evenement   -----"<<endl;
   cout<<"-----------------------------------------"<< endl<<endl;
   cout<<" argc = "<<argc<<" argv = "<<argv[0]<<" argv = "<<argv[1]	     <<endl<<endl;
   NFichier = argv[1];
   NFichier0 = argv[1];
   NFichier = NFichier +".rcp";
   NFichierT = NFichier0 + ".rcp.heurW";
   // NFichier = NFichier+".pat";//ici
   //NFichier = "./instances/j30RcpModif/jA15_B25/" + NFichier +".rcp";
   //NFichierT = "./instances/j30RcpModif/jA15_B25/" + NFichier0 + ".rcp.heurW";
   //NFichier = "./instances/vignier/" + NFichier +".pat";
   //NFichierT = "./instances/vignier/" + NFichier0 + ".rcp.heurW";
   cout<<"Fichier instance          : "<<NFichier<<endl;
   cout<<"Fichier fenetres de temps : "<<NFichierT<<endl;
   //getchar();
   Event2();

   return 0;
}
    
//---------------------------------------------------------------------------------------------------
