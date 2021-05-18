#include"individuo.hpp"
#include"nsga2.hpp"
#include<algorithm>
#include<iostream>
#include"jsp.hpp"
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<chrono>
#include<cmath>

using namespace std;

int main(int argc, char** argv){
    // leer instancia y obtener parametros
    jsp problem(argv[1]);
    individuo x,y,z;
    x.create_rand(problem.req);
    x.eval(problem.req);
    ofstream n7local,gapslocal,rc7,rcg;
    n7local.open("n7local.ind");
    rc7.open("n7local.rc");
    gapslocal.open("gapslocal.ind");
    rcg.open("gapslocal.rc");
    cout << x.costo() <<endl;
    y = problem.local_search(x,make_n7);
    cout << y.costo() <<endl;
    y = problem.local_search(y,make_n7);
    cout << y.costo() <<endl;
    z = problem.local_search(y,make_vec2);
    cout << z.costo() <<endl;
    y.show(n7local);
    y.show_rc(rc7);
    z.show(gapslocal);
    z.show_rc(rcg);
    return 0;
}
