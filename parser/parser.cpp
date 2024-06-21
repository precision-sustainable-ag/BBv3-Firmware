#define TESTING

#include <stdio.h>
#include <iostream>
#include <config.hpp>
#include <ClearCore.h>

using namespace std;

int main(){
    printf("C Format Working\n");
    cout << "C++ Format Working\n";
    ClearCore::TestCMake();
}