#include"individuo.hpp"
#include<algorithm>
#include<fstream>
#include"jsp.hpp"
#include<random>
#include<vector>
#include<chrono>
#include<cmath>

using namespace std;
vector< vector< pair <int,int> > > getData(string fname){ 
    ifstream infile;
    infile.open(fname);
    string line;
    int nmaq,njobs,time,maq;
    getline(infile,line);
    istringstream lines(line);
    lines>>njobs;
    lines>>nmaq;

    vector< vector<pair<int,int>> > req(njobs, vector<pair<int,int>>(nmaq));
    int countrow=0,countcol=0;
    while(getline(infile,line)){
        countcol=0;
        istringstream lines(line);
        for(int i=0;i<nmaq;i++){
            lines>>maq;
            lines>>time;
            req[countrow][countcol]=make_pair(maq,time);
            countcol+=1;
        }
        countrow+=1;
    }
    return req;
}

template<class ForwardIt, class Compare>
ForwardIt maxelement(ForwardIt first, ForwardIt last, 
                              Compare comp)
{
        if (first == last) return last;
         
            ForwardIt largest = first;
                ++first;
                    for (; first != last; ++first) {
                                if (comp(*largest, *first)) {
                                                largest = first;
                                                        }
                                    }
                        return largest;
}

// escala todos los tiempos a un factor gamma del maximo
vector<vector<pair<int,int>>> scale_req(vector<vector<pair<int,int>>> req,double gamma){
    int maxtime=0,aux;

    // encontrar el máximo
    for(auto v: req){
    aux = maxelement(v.begin(),v.end(),[](pair<int,int> p1,pair<int,int> p2){
            return (p1.second<p2.second);})->second;
    maxtime = maxtime<aux?aux:maxtime; 
    }
    for(int i=0;i<req.size();i++){
        for(int j=0;j<req[i].size();j++){
            req[i][j].second=ceil(gamma*req[i][j].second);
        }
    }
    return req;

} 


individuo ILS(individuo inicial,const vector<vector<pair<int,int>>>& req,int max_seconds,ostream& fout){
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    inicial.eval(req);
    inicial.N7climb(req);
    //fout<<"0"<<" "<<inicial.cost<<endl;
    individuo best=inicial;
    while(std::chrono::duration_cast<chrono::seconds>(end-start).count()<max_seconds){
        // shake
        inicial.perturb();
        // busqueda en vecindad 
        inicial.N7climb(req);
        inicial.N8climb(req);
        inicial.N7climb(req);
        //inicial.N8climb(req);
        fout<<chrono::duration_cast<chrono::milliseconds>(end-start).count()<<" "<<inicial.costo()<<endl;
        if(inicial<best)
            best=inicial;
        end = std::chrono::steady_clock::now();
    }
    return best;
}

