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
        prule rule,update,avg;
        for(int i=0;i<1;i++){
            individuo best = problem.VNS(rule,300,make_n7);
            cout << best.costo()<< endl;
        }
        /*
        best.load("best80.txt",problem.req);
        best.eval(problem.req);

        rule.init(best);
        locopt = problem.ASGA(&rule);
        update.init(locopt);

        best = problem.local_search(&rule);
        locopt = problem.local_search(&update);

        cout << best.costo()<<" "<<locopt.costo()<<endl;
        cout <<"0 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.local_search(locopt,make_n7);
        cout <<"n7 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.local_search(&rule);
        cout <<"pr "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.VNS(rule,60,.01,make_n7);
        cout <<"3 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.VNS(rule,120,.01,make_n7);
        cout <<"4 "<< i<<" "<< locopt.costo()<<endl;
        */
        

    }
    return 0;
}
