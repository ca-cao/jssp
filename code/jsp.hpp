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
vector<vector<pair<int,int>>> getData(string fname);

vector<vector<pair<int,int>>> scale_req(vector<vector<pair<int,int>>> req,double gamma);

individuo ILS(individuo inicial,const vector<vector<pair<int,int>>>& req,int max_seconds,ostream& fout=cout);

individuo climb(individuo inicial, const vector<vector<pair<int,int>>>& req, 
        vector<pair<int,int>> (individuo::*make_vec)(const vector<vector<pair<int,int>>>&));
#endif
