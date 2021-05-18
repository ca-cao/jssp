#include"individuo.hpp"
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
    ofstream ln7,gln7; 
    ln7.open("localn7.dat");
    gln7.open("gapslocaln7.dat");
    individuo x;
    x.create_rand(req);
    x.eval(req);
    cout << "rand "<<x.costo() <<endl;
    x.N7climb(req,false);
    cout << "n7 "<< x.costo() <<endl;
    x.show(ln7);
    x.N7climb(req,true);
    cout << "n8 "<< x.costo() <<endl;
    x.show(gln7);
    return 0;
}
