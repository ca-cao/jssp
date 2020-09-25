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
    ofstream  bfile,wfile,bfilerc,wfilerc;
    bfile.open("best.dat");
    wfile.open("worst.dat");
    bfilerc.open("bestrc.dat");
    wfilerc.open("worstrc.dat");
    bool add_gaps = true;
    int nruns = 100;
    individuo best,worst,x;
    x.create_rand(req);
    x.eval(req);
    best = x;
    worst = x;
    unsigned long int b=0,a;
    // no gaps
    for(int i=0;i<nruns;i++){
        x.create_rand(req);
        x.N7climb(req,!add_gaps);
        a = x.costo();
        // aqui ya esta optimizado
        x.N7climb(req,add_gaps);
        b += (a>x.costo());
        if(x<best)
            best = x;
        if(worst<x)
            worst = x;
    }
    cout<<"total "<<b<<endl;
    best.show(bfile);
    
    worst.show(wfile);

    return 0;
}
