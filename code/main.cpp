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
    f1.open("fit.ind");
    f2.open("fit.rc");
    cout<<tanh(10.0/1000)<<endl;
    
    individuo x;
    x.create_rand(req);
    x = ILS(x,req,300);
    x.show(f1);
    x.show_rc(f2);
    return 0;
}
