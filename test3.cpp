#include <iostream>

#include <list>

using namespace std;

int main(){
    std::list<int> l;
    l.push_back(8);
    l.push_back(24);
    l.push_back(30);

    for(auto i : l ){
        std::cout << i << endl;
    }
    
    return 0;
}