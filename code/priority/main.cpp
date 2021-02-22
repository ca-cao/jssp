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

        individuo best,bestalt;
        //best.load("best80.txt",problem.req);
        //best.eval(problem.req);
        prule rule,bestrule;
        //bestrule.init(best);
        bestalt = problem.ILS(&rule,300,.6);
        cout<<bestalt.costo()<<endl;

        //recv.show(reprule);
        //problem.local_search(best,make_n7);
        //best.show(repin);
        /*
        for(int i=0;i<10;i++){
            best.create_rand(problem.req);
            clock_t lsn7 = clock();
            best = problem.local_search(best,make_n7);
            lsn7 = clock() - lsn7;
            auto Vecn7 = make_n7(best);
            int vecn7 = Vecn7.size();
            rule.rand_init(problem.req);
            problem.ASGA(&rule);
            int vecpr = rule.changes.size();
            clock_t lspr = clock();
            bestalt = problem.local_search(&rule);
            lspr = clock() - lspr;
            cout <<best.costo()<<" "<<bestalt.costo()<<" lsn7/lspr: "<<lspr/(lsn7*1.0)
                <<" vecpr/vecn7 "<<vecpr/(1.0*vecn7)<<"\n";
        }
        */
    }
    return 0;
}
   
        /*
        for(int i=1;i<=10;i++){
            //rule.rand_init(problem.req,3);
            individuo test = problem.ILS(&rule,0,double(i)*.1,1);
            cout << "#"<<test.costo()<<endl;
        }
        //individuo test = problem.local_search(&rule);
        */
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
