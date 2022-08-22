// Alessandro Pegoraro, 2022

#include <iostream>

#include "APMemory.h"

class Test
{
    int test;
    double testD;

public:

    Test() : test(0), testD(0.) { std::cout << "Built!\n"; }
    Test(int _test, double _testD) : test(_test), testD(_testD) { std::cout << "Built With Pars!\n"; }
    ~Test() { std::cout << "Destroyed!\n"; }
};

void TestFunc()
{
    Test* standardAllocTest = new Test(5, 3.14159);
    std::cout << "standardAllocTest Size = " << sizeof(*standardAllocTest) << '\n';

    Test* apAllocTest = new(APMemory::Alloc(sizeof(Test))) Test(5, 3.14159);
    std::cout << "apAllocTest Size = " << sizeof(*apAllocTest) << '\n';

    delete standardAllocTest;

    APMemory::Delete(apAllocTest);
}


int main()
{
    //TestFunc();

    APMemory::InitMemoryManager(1024);

    Test* first = new(APMemory::Alloc(sizeof(Test))) Test();
    int* second = new(APMemory::Alloc(sizeof(int))) int();
    Test* third = new(APMemory::Alloc(sizeof(Test))) Test();
    double* fourth = new(APMemory::Alloc(sizeof(double))) double();
    Test* fifth = new(APMemory::Alloc(sizeof(Test))) Test();
    double* sixth = new(APMemory::Alloc(sizeof(double))) double();

    std::cout << "Numero di blocchi: " << APMemory::GetNumberOfMemBlocks() << '\n';

    APMemory::Delete(third);
    std::cout << "Numero di blocchi: " << APMemory::GetNumberOfMemBlocks() << '\n';
    APMemory::Delete(fourth);
    std::cout << "Numero di blocchi: " << APMemory::GetNumberOfMemBlocks() << '\n';

    fourth = new(APMemory::Alloc(sizeof(double))) double();
    std::cout << "Numero di blocchi: " << APMemory::GetNumberOfMemBlocks() << '\n';

    APMemory::Delete(first);
    std::cout << "Apposto il primo: " << APMemory::GetNumberOfMemBlocks() << '\n';
    APMemory::Delete(second);
    std::cout << "Apposto il secondo: " << APMemory::GetNumberOfMemBlocks() << '\n';
    APMemory::Delete(fourth);
    std::cout << "Apposto il quarto: " << APMemory::GetNumberOfMemBlocks() << '\n';
    APMemory::Delete(fifth);
    std::cout << "Apposto il quinto: " << APMemory::GetNumberOfMemBlocks() << '\n';
    APMemory::Delete(sixth);
    std::cout << "Apposto il sesto: " << APMemory::GetNumberOfMemBlocks() << '\n';
    std::cout << "Numero di blocchi: " << APMemory::GetNumberOfMemBlocks() << '\n';

    APMemory::ShutdownMemoryManager();
}