#ifndef JSP_HPP
#define JSP_HPP
#include"individuo.hpp"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;
// regresa la matriz con los pesos de las aristas
instance getData(string fname);
vector<pair<int,int>> make_n7(individuo x);

class jsp{
    public:
        jsp(string fname);
        ~jsp();
        instance req;
        instance scale_req(double gamma);
        individuo local_search(individuo x,vector<pair<int,int>> (*vec)(individuo));
        individuo ILS(individuo inicial,vector<pair<int,int>> (*vec)(individuo),int max_seconds,ostream& fout=cout);
};
#endif
