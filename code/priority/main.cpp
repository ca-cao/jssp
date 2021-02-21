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
    // leer instancia y obtener parametros
    for(int i=80;i<=80;i++){
        string file("../../instancias/dmu");
        if(i<10)
            file = file + "0" +to_string(i)+".txt";
        else
            file =file +to_string(i)+".txt";
        jsp problem(file);

        individuo best;
        //best.load("best80.txt",problem.req);
        prule rule;
        for(int i=1;i<=10;i++){
            rule.rand_init(problem.req,3);
            individuo test = problem.ILS(&rule,0,double(i)*.1,1);
            cout << "#"<<test.costo()<<endl;
        }
        //individuo test = problem.local_search(&rule);
        
    }
   
        //individuo localopt = problem.local_search(test,make_activec);
        //ind.open("randind",std::ofstream::out | std::ofstream::trunc);
        //test.eval(problem.req);
        //test.show();

    //jsp problem(argv[1]);
    //individuo test;
    //test = problem.ASGA(new minstart() );
    //test.show();
    //cout<<<<test.costo()<<endl;
    //test.pr_build(problem.req);
    //test.get_rc();
    
    //test.show();
    //test.show_rc();
    return 0;
}
