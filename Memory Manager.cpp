// Alessandro Pegoraro, 2022

#include <iostream>
#include <vector>
#include <cassert>

#include "APMemory.h"

class Test
{
    int test;
    double testD;

public:

    Test() : test(0), testD(0.) { /*std::cout << "Built!\n";*/ }
    Test(int _test, double _testD) : test(_test), testD(_testD) { /*std::cout << "Built With Pars!\n";*/ }
    ~Test() { /*std::cout << "Destroyed!\n";*/ }
};

class Byte48
{
    double x, y, z, w, v, u;
};

void TestFunc()
{
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


void TestSmallAlloc1()
{
    using namespace APMemory;

    InitMemoryManager(1024);

    std::vector<Test*> testObjs;
    testObjs.reserve(200);

    for (int i = 0; i < 100; ++i)
    {
        Test* newObj = new(Alloc(sizeof(Test))) Test();

        testObjs.push_back(newObj);
    }

    std::cout << "\n\n\n";

    assert(testObjs.size() == 100);

    for (int i = 0; i < 50; ++i)
    {
        Test* oldObj = testObjs.back();
        Delete(oldObj);
        testObjs.pop_back();
    }

    std::cout << "\n\n\n";

    assert(testObjs.size() == 50);

    for (int i = 0; i < 150; ++i)
    {
        Test* newObj = new(Alloc(sizeof(Test))) Test();

        testObjs.push_back(newObj);
    }

    std::cout << "\n\n\n";

    assert(testObjs.size() == 200);

    for (int i = 0; i < 200; ++i)
    {
        Test* oldObj = testObjs.back();
        Delete(oldObj);
        testObjs.pop_back();
    }

    assert(testObjs.size() == 0);

    ShutdownMemoryManager();
}

void TestSmallAlloc2()
{
    using namespace APMemory;

    InitMemoryManager(1024);

    std::vector<Test*> Obj16;
    std::vector<int*> Obj04;
    std::vector<Byte48*> Obj48;
    Obj16.reserve(150);
    Obj04.reserve(250);
    Obj48.reserve(100);


    for (int i = 0; i < 100; ++i)
    {
        Test* newObj = new(Alloc(sizeof(Test))) Test();

        Obj16.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj16.size() == 100);

    for (int i = 0; i < 50; ++i)
    {
        int* newObj = new(Alloc(sizeof(int))) int;

        Obj04.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj04.size() == 50);

    for (int i = 0; i < 100; ++i)
    {
        Byte48* newObj = new(Alloc(sizeof(Byte48))) Byte48();

        Obj48.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj48.size() == 100);



    for (int i = 0; i < 50; ++i)
    {
        int* oldObj = Obj04.back();
        Delete(oldObj);
        Obj04.pop_back();
    }
    std::cout << "\n\n\n";
    assert(Obj04.size() == 0);

    for (int i = 0; i < 50; ++i)
    {
        Byte48* oldObj = Obj48.back();
        Delete(oldObj);
        Obj48.pop_back();
    }
    std::cout << "\n\n\n";
    assert(Obj48.size() == 50);



    for (int i = 0; i < 50; ++i)
    {
        Test* newObj = new(Alloc(sizeof(Test))) Test();

        Obj16.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj16.size() == 150);

    for (int i = 0; i < 250; ++i)
    {
        int* newObj = new(Alloc(sizeof(int))) int;

        Obj04.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj04.size() == 250);

    for (int i = 0; i < 50; ++i)
    {
        Byte48* newObj = new(Alloc(sizeof(Byte48))) Byte48;

        Obj48.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj48.size() == 100);



    for (int i = 0; i < 100; ++i)
    {
        Byte48* oldObj = Obj48.back();
        Delete(oldObj);
        Obj48.pop_back();
    }
    std::cout << "\n\n\n";
    assert(Obj48.size() == 0);

    for (int i = 0; i < 250; ++i)
    {
        int* oldObj = Obj04.back();
        Delete(oldObj);
        Obj04.pop_back();
    }
    std::cout << "\n\n\n";
    assert(Obj04.size() == 0);

    for (int i = 0; i < 150; ++i)
    {
        Test* oldObj = Obj16.back();
        Delete(oldObj);
        Obj16.pop_back();
    }
    std::cout << "\n\n\n";
    assert(Obj16.size() == 0);


    ShutdownMemoryManager();
}


int main()
{
    //TestFunc();
    TestSmallAlloc2();

}