#ifndef JSP_HPP
#define JSP_HPP
#include"individuo.hpp"
#include"prule.hpp"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;
// regresa la matriz con los pesos de las aristas
instance getData(string fname);
vector<pair<int,int>> make_n7(const individuo& x);
//vector<pair<int,int>> make_activec(const individuo& x);

class jsp{
public:
    jsp(string fname);
    ~jsp();
    instance req;
    instance scale_req(double gamma);
    // si seed =0 utiliza el tiempo como semilla
    individuo local_search(prule* rule,unsigned seed =0);
    individuo ILS(prule* rule,int max_seconds,double pflip,unsigned seed=0,ostream& fout=cout);
    individuo ASGA(prule* rule,unsigned seed = 0);
    individuo local_search(individuo x,vector<pair<int,int>> (*vec)(const individuo&));
    individuo ILS(individuo inicial,vector<pair<int,int>> (*vec)(const individuo&),int max_seconds,ostream& fout=cout);
    vector<individuo> nsga2(int popsize,int max_iter);
    individuo VNS(prule rule,int max_seconds,vector<pair<int,int>> (*vec)(const individuo&),int nflip=4);
    //vector<individuo> nsga2(int popsize,int max_iter);
};

#endif
