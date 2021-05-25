#include <string>
#include <iostream>
#include <vector>
#include <future>
#include "ThreadPool.h"

using namespace std;
int test1(int ms)
{
    int xorsum = 0;
    for(int i = 0; i < ms; i++) {
        xorsum = xorsum + i;
    }
    return xorsum;
}
void test2()
{
    int xorsum = 1;
    for(int i = 0; i < 10000; i++) {
        xorsum = xorsum + i;
    }
    cout << xorsum << endl;
}
void test3(int& xorsum)
{
    for(int i = 0; i < 10000; i++) {
        xorsum = xorsum + i;
    }
}

int main()
{
    ThreadPool TP(4);
    auto te = TP.enqueue(test1, 10000);
    auto tt = TP.enqueue(test2);
    int c = 0;
    auto t3 = TP.enqueue(test3, std::ref(c));
    auto t4 = TP.enqueue([](){int j = 0; for(int i = 0; i < 100000; ++i) j+=i; cout << j<< endl;});
    int a = te.get();
    tt.get();
    cout << a << endl;
    cout << c << endl;
    t4.get();
    TP.stopAll();
    return 0;
}
