#ifndef FAP_HPP
#define FAP_HPP
#include"individuo.hpp"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;
// regresa la matriz con los pesos de las aristas
vector<vector<pair<int,int>>> getData(string fname);

void show_rc(vector<vector<job*>> rc,const vector<vector<pair<int,int>>>& req,ostream& fout = cout);


#endif
