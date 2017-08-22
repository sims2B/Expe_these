
#include <algorithm>
#include "model_mip_bin.h"


//Solve the Problem with MILP2
int solveBin(const LotProblem& LP,Solution &s){
  try{
    const int n(LP.nbLot), Q(LP.nbSublot),m(LP.nbLine);
  
    IloEnv env;
    IloModel model(env);
    IloNumVarArray t(env,2*n*Q,0,LP.horizon,ILOFLOAT);
    IloNumVar4DMatrix x(env,n), y(env,n);
    //AD and BF correspond to P_{he} and P_{eh}, 
    IloNumVarMatrix P(env,2*n*Q), AD(env,2*n*Q), BF(env,2*n*Q), 
      p(env,n), O(env,2*n*Q), BM(env,2*n*Q), W(env,n);
    IloNumVar Pmax(env,0,m*LP.delta,ILOFLOAT);

    createModel(env,model,LP,x,y,p,P,AD,BF,O,BM,t,Pmax,W);

    IloCplex cplex(model);
    cplex.setParam(IloCplex::EpInt, 0.0);
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads, 1);
    //cplex.setOut(env.getNullStream());
    IloNum start= cplex.getCplexTime();
    bool solFound=cplex.solve();
    IloNum time_exec=cplex.getCplexTime()-start;
    std::cout << "status: "<< cplex.getStatus() << " en " 
	      << time_exec <<  std::endl;
    if (solFound){
      std::cout << "obj model bin=" << cplex.getObjValue() << std::endl;
      modelToSolution(LP,s,cplex,t,x,y,BM);
    }
    env.end();
    return 0;
  }
  catch (IloException& e) {
    std::cout <<e << "\n";
    e.end();
    return 1;
  }
}

//Solve the problem with MILP3 then uses the solution as a MIP start for MILP2
double solveBinSimple(const LotProblem &LP,Solution &s,Solution &s2){
  try{
    const int n(LP.nbLot), Q(LP.nbSublot),m(LP.nbLine);
  
    IloEnv env;
    IloModel model(env);
   
    IloNumVarArray t(env,2*n*Q,0,IloInfinity,ILOFLOAT);
    IloNumVar4DMatrix x(env,n), y(env,n);
    //AD and BF correspond to P_{he} and P_{eh}, 
    IloNumVarMatrix P(env,2*n*Q), AD(env,2*n*Q), BF(env,2*n*Q), 
      p(env,n), O(env,2*n*Q), BM(env,2*n*Q), W(env,n);
    IloNumVar Pmax(env,0,m*LP.delta,ILOFLOAT);
    
    createModel(env,model,LP,x,y,p,P,AD,BF,O,BM,t,Pmax,W);
    IloCplex cplex(model);
   
    bool valide=s.isValid(LP);
    std::cout<< "solution 1 valide? " << valide << "\n";
    if (valide) {
      //s.printSol(LP);
      IloNumVarArray startVar(env);
      IloNumArray startVal(env);
      solutionToModel(env,model,LP,s,t,x,y,BM,O,BF,AD,startVar,startVal);
      cplex.addMIPStart(startVar,startVal);
      startVar.end();
      startVal.end();
      std::cout << "starting MIP start \n";
    }
    cplex.setParam(IloCplex::EpInt, 0.0);
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads, 1);
    //cplex.setOut(env.getNullStream());
    IloNum start= cplex.getCplexTime();
    bool solFound=cplex.solve(); 
    IloNum time_exec=cplex.getCplexTime()-start;
    std::cout << "status: "<< cplex.getStatus() << " en " 
	      << time_exec <<  std::endl;
    if (solFound){
      std::cout << "obj model bin (conso + peak)=" << cplex.getObjValue() 
		<< std::endl;
      modelToSolution(LP,s2,cplex,t,x,y,BM);
      double peak=cplex.getValue(Pmax);
      env.end();
      return peak;
    }
    else {
      env.end();
      return -1.0;
    }
  }
  catch (IloException& e) {
    std::cout <<e << "\n";
    e.end();
    return -1.0;
  }
}

int createModel(IloEnv& env,IloModel & m, const LotProblem &LP,IloNumVar4DMatrix &x, 
		IloNumVar4DMatrix &y, IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD, 
		IloNumVarMatrix &BF, IloNumVarMatrix &O, IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &Pmax,IloNumVarMatrix &W){
  return createVar(env,LP,x,y,p,P,AD,BF,O,BM,W) || 
    createConstraint(env,m,LP,x,y,p,P,AD,BF,O,BM,t,Pmax,W);
}

int createVar(IloEnv& env,const LotProblem &LP,IloNumVar4DMatrix &x, IloNumVar4DMatrix &y,  IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD, IloNumVarMatrix &BF, IloNumVarMatrix &O, IloNumVarMatrix &BM, IloNumVarMatrix &W){
  const int n(LP.nbLot), Q(LP.nbSublot),m(LP.nbLine), T(LP.nbHP);

  int i,k,q,e;
  for (i=0;i<n;++i) {
    x[i]=IloNumVar3DMatrix(env,Q);
    y[i]=IloNumVar3DMatrix(env,Q);
    W[i]=IloNumVarArray(env,Q,0,LP.delta*LP.demand[i],ILOFLOAT);
    p[i]=IloNumVarArray(env,Q,0,LP.demand[i],ILOFLOAT);
    for (q=0;q<Q;++q) {
      x[i][q]=IloNumVarMatrix(env,2*n*Q);
      y[i][q]=IloNumVarMatrix(env,2*n*Q);
      for (e=0;e<2*n*Q;++e) {
	x[i][q][e]=IloNumVarArray(env,m,0,1,ILOINT);
	y[i][q][e]=IloNumVarArray(env,m,0,1,ILOINT);
	
      }
    }
  }
  for (e=0;e<2*n*Q;++e) {
    P[e]=IloNumVarArray(env,m,0,1,ILOFLOAT);
    AD[e]=IloNumVarArray(env,T,0,1,ILOINT);
    BF[e]=IloNumVarArray(env,T,0,1,ILOINT);
    O[e]=IloNumVarArray(env,m,0,1,ILOFLOAT);
    BM[e]=IloNumVarArray(env,m,0,1,ILOINT);
  }

  return 0;
}

int createConstraint(IloEnv& env, IloModel &model, const LotProblem &LP,IloNumVar4DMatrix &x, IloNumVar4DMatrix &y, IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD, IloNumVarMatrix &BF, IloNumVarMatrix &O, IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &Pmax,IloNumVarMatrix &W){
  const uint n(LP.nbLot), Q(LP.nbSublot),m(LP.nbLine), T(LP.nbHP);

  uint i,e,q,j,f,k,h;

  //Tardiness
  IloNumVar Tard(env,0,IloInfinity);
  model.add(Tard>=t[2*n*Q-1] - LP.horizon);

  IloExpr expr(env);
  for (i=0;i<n;++i){
    for (q=0;q<Q;++q)
      expr+= W[i][q];
  }

  /*IloExpr exprb(env);
    for (e=0;e<2*n*Q;++e) 
    for (k=0;k<m;++k)
    exprb+=P[e][k];
  */
  model.add(IloMinimize(env,ALPHA*expr+BETA*Pmax+GAMMA*Tard));
  //  model.add(IloMinimize(env,Pmax)); essai de ne minimiser que le pic
  //  model.add(IloMinimize(env,exprb+Pmax)); essai de minimiser la somme du pic 
  //  avec le nombre d'evenements en heures pleines
  //exprb.end();
  expr.end();

  
  //ordonne event
  for (e=0;e<2*n*Q-1;e++)
    model.add(t[e]<=t[e+1]); 


  //souslot ordonnancer une fois max (Q nb max de sous lot mais il peut y en avoir moins)
  for (i=0;i<n;++i)  {
    for (q=0;q<Q;++q) {
      IloExpr expr1(env);
      IloExpr expr2(env);
      for (e=0;e<2*n*Q;++e) {
	for (k=0;k<m;++k) {
	  expr1+=x[i][q][e][k];
	  expr2+=y[i][q][e][k];
	}
      }
      model.add(expr1 <= 1);
      model.add(expr2 <= 1);
      expr1.end();
      expr2.end();
    }
  }    
  

  //un lot ne peut finir s'il n'a pas commencer
  for (k=0;k<m;++k) {
    for (i=0;i<n;++i) {
      for (q=0;q<Q;++q) {
	IloExpr expr(env);
	for (e=0;e<2*n*Q;++e) 
	  expr+=x[i][q][e][k]-y[i][q][e][k];
	model.add(expr == 0);
	expr.end();
      }
    }
  }

  //un lot ne peut finir avant d'avoir commencer
  for (k=0;k<m;++k) {
    for (i=0;i<n;++i) {
      for (q=0;q<Q;++q)	{
	for (e=0;e<2*n*Q;++e)	{
	  IloExpr expr(env);
	  for (f=0;f<=e;++f)
	    expr+=x[i][q][f][k]-y[i][q][f][k];
	  model.add(expr >= 0);
	  expr.end();
	}
      }
    }
  }


  //un sous lot doit etre affecté pour produire
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {  
      IloExpr expr(env);
      for (e=0;e<2*n*Q;++e) {
	for (k=0;k<m;++k)
	  expr+=x[i][q][e][k];
      }
      model.add(LP.demand[i]*expr>=p[i][q]);
      expr.end();
    }
  }

  //minimum size batch
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {  
      IloExpr expr(env);
      for (e=0;e<2*n*Q;++e) {
	for (k=0;k<m;++k)
	  expr+=x[i][q][e][k];
      }
      model.add(expr*minSize<=p[i][q]);
      expr.end();
    }
  }


  //une machine n'est pas utilisé en meme temps pour deux sous lot
  for (e=1;e<2*n*Q;++e) {
    for (k=0;k<m;++k) {
      IloExpr expr(env);
      for (i=0;i<n;++i) {
	for (q=0;q<Q;++q)
	  expr+=x[i][q][e][k]-y[i][q][e][k];
      }
      model.add(O[e][k]==O[e-1][k]+expr);
      expr.end();
    }
  }

  for (k=0;k<m;++k) {
    IloExpr expr(env);
    for (i=0;i<n;++i) {
      for (q=0;q<Q;++q)
	expr+=x[i][q][0][k];
    }
    model.add(O[0][k]==expr);
    expr.end();
  }
    
  //DEADLINE GENERALE DU PROJET
  //model.add(t[2*n*Q-1]<=LP.horizon);
    
  //setup!!!!!!!!!!!!!
  for (i=0;i<n;++i) {
    for (j=0;j<n;++j) {
      if (i!=j && LP.setup[i][j]!=0){
	for (f=0;f<2*n*Q;++f) {
	  for (e=f;e<2*n*Q;++e)  {
	    for (k=0;k<m;++k) {
	      IloExpr expr(env);
	      for (q=0;q<Q;++q)
		expr+=y[i][q][f][k]+x[j][q][e][k];
	      model.add(t[e]>=t[f]+LP.setup[i][j]*(expr-1));
	      expr.end();
	    }
	  }
	}
      }
    }
  }

  //machine autorisée
    
  for (i=0;i<n;++i) {
    for (k=0;k<LP.subset[i].size();k++){
      IloExpr expr(env);
      for (q=0;q<Q;++q) {
	for (e=0;e<2*n*Q;++e) 
	  expr+=x[i][q][e][LP.subset[i][k]];
      }
      model.add(expr==0);
      expr.end();
    }
  }
   


  //durée sous lot en fonction de la machine
  for (i=0;i<n;++i)  {
    for (q=0;q<Q;++q)	 {
      for (k=0;k<m;++k)  {
	for (e=0;e<2*n*Q;++e)  {
	  for (f=e;f<2*n*Q;++f)  {
	    if (LP.speed[i][k]!=0) 
	      model.add(t[f]>=t[e]+p[i][q]*1/LP.speed[i][k]
			-M*(2-x[i][q][e][k]-y[i][q][f][k]));
	  }
	}
      }
    }
  }
    
  for (i=0;i<n;++i)  {
    for (q=0;q<Q;++q)	 {
      for (k=0;k<m;++k)  {
	for (e=0;e<2*n*Q;++e)  {
	  for (f=e;f<2*n*Q;++f)  {
	    if (LP.speed[i][k]!=0) 
	      model.add(t[f]<=t[e] +p[i][q]*1/LP.speed[i][k]
			+M*(2-x[i][q][e][k]-y[i][q][f][k]));
	  }
	}
      }
    }
  }
  
  //la demande de chaque lot est respectée
  for (i=0;i<n;++i)
    {
      IloExpr expr(env);
      for (q=0;q<Q;++q)
	expr+=p[i][q];
      model.add(expr==LP.demand[i]);
      expr.end();
    }


  //contrainte l'evenement e est dans la periode d'heure pleine t
  //séparée en deux: e est avant la fin de l'heure pleine t 
  //                 et e est apres le debut de l'P t

  for (e=0;e<2*n*Q-1;++e){
    for (h=0;h<T;++h)
      model.add(AD[e][h] >= (t[e+1] - LP.beginHP[h])/(2*LP.horizon));
  }
  
  for (e=0;e<2*n*Q;++e){
    for (h=0;h<T;++h){
      model.add(LP.endHP[h]*BF[e][h] >= LP.endHP[h] - t[e]);
	    
      for (k=0;k<m;++k)
	model.add(P[e][k] >= AD[e][h] + BF[e][h] +O[e][k]-2);
    }
  }
  

   
  //pic de conso max
  for (e=0;e<2*n*Q;++e)
    {
      IloExpr expr(env);
      for (k=0;k<m;++k)
	expr+=P[e][k];
      expr*=LP.delta;
      model.add(Pmax>=expr);
      expr.end();
    }

  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (k=0;k<m;++k){
	if (LP.speed[i][k]!=0){
	  IloExpr expr(env);
	  for (e=0;e<2*n*Q;++e)
	    expr+=x[i][q][e][k];
	  model.add(W[i][q] >= LP.delta*p[i][q]/LP.speed[i][k]-M*(1-expr));
	  expr.end();
	}
      }
    }
  }
 
   
  //OPERATION DE MAINTENANCE
  //si il y a une maintenance à faire sur la machine k alors un et un seul début
  for (k=0;k<m;++k){
    if (LP.durMaint[k]!=0){
      IloExpr expr(env);
      for (e=0;e<2*n*Q;++e){
	expr+=BM[e][k];
      }
      model.add(expr==1);
      expr.end();
    }
  }
    
  //l'operation doit etre avant son "start max"
  for (e=0;e<2*n*Q;++e)
    {
      for (k=0;k<m;++k)
	model.add(t[e]<=LP.horizon*(2-BM[e][k])-LP.durMaint[k]*BM[e][k]);
      //model.add(t[e]<=LP.horizon*-LP.durMaint[k]*BM[e][k]);
    }
    
  //si on commence une operation de maintenance sur la machine k, 
  //aucune tache ne peut commencer sur k avant la fin de cette operation
  //et aucune tache ne peut etre pendant la maintenance
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (e=0;e<2*n*Q;++e) {
	for (f=e;f<2*n*Q;++f) {
	  for (k=0;k<m;++k)
	    model.add(t[f] >= t[e] + 
		      LP.durMaint[k]*(BM[e][k]+x[i][q][f][k]-1));
	}
      }
    }
  }

  for (e=0;e<2*n*Q;++e) {
    for (k=0;k<m;++k)
      model.add(BM[e][k]+O[e][k]<=1);
  }
    
  return 0;
}


int modelToSolution(const LotProblem& LP,Solution &s,IloCplex& cplex,IloNumVarArray& t,IloNumVar4DMatrix& x,IloNumVar4DMatrix& y,  IloNumVarMatrix &BM){
  int i,q,j,e;
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;

  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (e=0;e<2*n*Q;++e){
	for (j=0;j<m;++j){
	  if (cplex.getValue(x[i][q][e][j])-1<=POSITIVE_ZERO && cplex.getValue(x[i][q][e][j])-1>=NEGATIVE_ZERO)
	    { s.st[i][q]=cplex.getValue(t[e]);
	      if (cplex.getValue(y[i][q][e][j])!=1)
		s.a[i][q]=j;
	    }
	  if (cplex.getValue(y[i][q][e][j])-1<=POSITIVE_ZERO && cplex.getValue(y[i][q][e][j])-1>=NEGATIVE_ZERO)
	    s.ft[i][q]=cplex.getValue(t[e]);
	}
      }
    }
  }
  
  for (j=0;j<m;++j){
    for (e=0;e<2*n*Q;++e){
      if (cplex.getValue(BM[e][j])==1) 
	s.dm[j]=cplex.getValue(t[e]);
    }
  }
  return 0;  
} 

int solutionToModel(IloEnv& env,IloModel & model,const LotProblem& LP,Solution& s,IloNumVarArray& t,IloNumVar4DMatrix& x, IloNumVar4DMatrix& y, IloNumVarMatrix& BM, IloNumVarMatrix &O
		    ,IloNumVarMatrix &BF, IloNumVarMatrix &AD,IloNumVarArray &startVar,IloNumArray &startVal){
  std::vector<mytuple> event_idx;
  
  const int n(LP.nbLot), m(LP.nbLine), Q(LP.nbSublot);
  int i,k,q;

  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      if (s.a[i][q]!=-1){
	event_idx.push_back(std::make_tuple(s.st[i][q],i,q,s.a[i][q],0));
	event_idx.push_back(std::make_tuple(s.ft[i][q],i,q,s.a[i][q],1));
      }
    }
  }

  std::sort(event_idx.begin(), event_idx.end(),comparator);
  for (i=0;i<event_idx.size();++i){ 
    for (int k=0;k<LP.nbHP;k++) {
      if (i<event_idx.size()-1){
	if (std::get<0>(event_idx[i+1])-POSITIVE_ZERO>LP.beginHP[k]) {
	  startVar.add(AD[i][k]);
	  startVal.add(1);
	}
	else if (std::get<0>(event_idx[i+1])+POSITIVE_ZERO<=LP.beginHP[k]) {
	  startVar.add(AD[i][k]);
	  startVal.add(0);
	}
      }
      if (std::get<0>(event_idx[i])+POSITIVE_ZERO<=LP.endHP[k]){
	startVar.add(BF[i][k]);
	startVal.add(1);
      }	
      else if (std::get<0>(event_idx[i])-POSITIVE_ZERO>LP.endHP[k]){
	startVar.add(BF[i][k]);
	startVal.add(0);
      }
    }
  }
  

  for (k=0;k<m;++k) {
    if (s.dm[k]!=-1.0)
      event_idx.push_back(std::make_tuple(s.dm[k],-1,-1,k,-1));
  }
  int evt=0;
  std::sort(event_idx.begin(), event_idx.end(),comparator);
  for (i=0;i<event_idx.size();++i){ 
    if (std::get<4>(event_idx[i])!=-1){
      if (std::get<4>(event_idx[i])==0){
	startVar.add(x[std::get<1>(event_idx[i])]
		     [std::get<2>(event_idx[i])]
		     [evt][std::get<3>(event_idx[i])]);
	startVal.add(1);
	startVar.add(O[evt][std::get<3>(event_idx[i])]);
	startVal.add(1);	  
	++evt;
      }
      else {
	startVar.add(y[std::get<1>(event_idx[i])]
		     [std::get<2>(event_idx[i])]
		     [evt][std::get<3>(event_idx[i])]);
	startVal.add(1);
	++evt;
      }
    }
    else {
      if (evt==0) {
	startVar.add(BM[0][std::get<3>(event_idx[i])]);
	startVal.add(1);
      }
      else { 
	startVar.add(BM[evt-1][std::get<3>(event_idx[i])]);
	startVal.add(1);
      }
    }
  }
  return 0;
}


bool comparator(const mytuple& l, const mytuple& r){ 
  if (std::get<0>(l)-std::get<0>(r)>NEGATIVE_ZERO && 
      std::get<0>(l)-std::get<0>(r)<POSITIVE_ZERO) {
      if (std::get<3>(l)==std::get<3>(r))
	return (std::get<4>(l)>std::get<4>(r));
      else return (std::get<4>(l)<std::get<4>(r));
  }
  else return (std::get<0>(l) < std::get<0>(r));
}
