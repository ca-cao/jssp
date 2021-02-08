#include<vector>
#include<iostream>

using namespace std;

void print(vector<int> a){
    for(auto x:a)
        cout<<x<<endl;
    cout<<endl;
}
typedef vector<int>::iterator iterator;
int main(){
    vector<int> a(100,0);
    vector<int> b;
    for(int i=0;i<a.size();i++)
        a[i]=i;
    b.insert(b.end(),a.begin()+48,a.begin()+53);
    print(b);
    cout<< (b.end()-b.begin())<<endl;
    cout << a <<endl;
}
