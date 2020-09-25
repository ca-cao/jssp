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

// ./main instancia num_canales salida
int main(int argc, char** argv){
    // leer instancia y obtener parametros
    vector<vector<pair<int,int>>> req=getData(argv[1]);
    
    int njobs=req.size(),nmaq=req[0].size();
    individuo x;
    x.create_rand(req);
    x.eval(req);
    x.show();
    return 0;
}


/*
    // imprimir los datos
    for(int i=0;i<njobs;i++){
        for(int j=0;j<nmaq;j++)
            cout<<req[i][j].first<<" "<<req[i][j].second<<"\t";
        cout<<"\n";
    }
*/
