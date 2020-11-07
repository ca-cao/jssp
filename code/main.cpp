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
    
    ofstream  f1,f2;
    f1.open("fit.ind");
    f2.open("fit.rc");
    
    individuo x;
    x.create_rand(problem.req);
    x = problem.ILS(x,make_n7,atoi(argv[2]));
    x.show(f1);
    x.show_rc(f2);
    return 0;
}
