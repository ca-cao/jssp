#include"individuo.hpp"
#include<algorithm>
#include"prule.hpp"
#include<iostream>
#include"jsp.hpp"
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<chrono>
#include<cmath>
#include<ctime>

using namespace std;

int main(int argc, char** argv){
    ofstream reprule,repin;
    reprule.open("rule.ind");
    repin.open("best.ind");
    // leer instancia y obtener parametros
    for(int i=80;i<=80;i++){
        string file("../../instancias/dmu");
        if(i<10)
            file = file + "0" +to_string(i)+".txt";
        else
            file =file +to_string(i)+".txt";
        jsp problem(file);
        prule rule;
        individuo locopt = problem.VNS(300,.1,make_n7);
    }
    return 0;
}
