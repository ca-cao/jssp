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
    jsp problem(argv[1]);
    
    ofstream  ind_file,rc_file,lon_file;
    ind_file.open("fit.ind");
    rc_file.open("fit.rc");
    lon_file.open("lonmap.nx");

    
    individuo x;
    x.create_rand(problem.req);
    x = problem.ILS(x,make_n7,atoi(argv[2]),lon_file);
    x.show(ind_file);
    x.show_rc(rc_file);
    return 0;
}
