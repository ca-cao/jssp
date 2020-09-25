#include"individuo.hpp"
#include<algorithm>
#include<iostream>
#include<fstream>
#include"jsp.hpp"
#include<sstream>
#include<vector>
#include<string>
#include<chrono>
#include<cmath>

using namespace std;

int main(int argc, char** argv){
    // leer instancia y obtener parametros
    vector<vector<pair<int,int>>> req=getData(argv[1]);
    vector<vector<job*>> rc; 
    int njobs=req.size(),nmaq=req[0].size(),col,row;
    
    ofstream  f1,f2;
    f1.open(argv[2]);

    f2.open(argv[3]);
    
    individuo x,best,n8;
    x.create_rand(req);
    x.eval(req);
    n8 = ILS(x,req,300,f1);
    n8.show(f2);
    n8.show_rc(f2);
    return 0;
}
