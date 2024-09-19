#include <iostream>

class test {
    public:
        int i = 3;

        static int j;
        static const int k = 300;

        static void f(){
            std::cout << "f" << std::endl;
        }

        operator bool() const {
            return false;
        }
};

int test::j = 100;

int main(){

    test t;
    if(t){
        std::cout << "This shouldn't be executed!" << std::endl;
    }

    if(!t){
        std::cout << "This should be executed!" << std::endl;
    }

    std::cout << t.i << std::endl;

    std::cout << test::j << std::endl;

    std::cout << test::k << std::endl;

    test::f();

    return 0;
}