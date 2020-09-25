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

// ./best instancia best.txt 
int main(int argc, char** argv){
    // leer instancia y obtener parametros
    vector<vector<pair<int,int>>> req=getData(argv[1]);
    vector<vector<job*>> rc; 
    int njobs=req.size(),nmaq=req[0].size(),col,row;
    
    individuo best;
    best.load(argv[2],req);
    best.eval(req);
    best.get_rc();
    best.perturb();
    best.eval(req);
    best.get_rc();
    //best.N8climb(req);
    //best.N7climb(req);
    best.show();
    best.show_rc();
    
    if(argc >1){
        ofstream  f1,f2;
        f1.open(argv[3]);
    }
    return 0;
}
