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
    individuo local_search(prule* rule);
    individuo ILS(prule* rule,int max_seconds,double pflip,ostream& fout=cout);
    //vector<individuo> nsga2(int popsize,int max_iter);
    individuo ASGA(prule* rule,unsigned seed = 1);
};

#endif
