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
    int max_iter = atoi(argv[2]),popsize = atoi(argv[3]);
    individuo x,y,z;
    auto pfront = problem.nsga2(popsize,max_iter);
    cout << "# "<<pfront.size()<<endl;
    for(auto sol :pfront)
        cout<<sol.obj[0]<<" "<<sol.obj[1]<<" "<<sol.front<<endl;
    return 0;
}
