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

class VeryBigData
{
    Test *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12, *a13, *a14,
        *a15, *a16, *a17, *a18, *a19, *a20, *a21, *a22, *a23, *a24, *a25, *a26, *a27,
        *a28, *a29, *a30, *a31, *a32, *a33, *a34, *a35, *a36, *a37, *a38, *a39, *a40,
        *a41, *a42, *a43, *a44, *a45, *a46, *a47, *a48, *a49, *a50;

    Byte48 b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17;
};


void TestSmallAlloc1()
{
    using namespace APMemory;

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
}

void TestSmallAlloc2()
{
    using namespace APMemory;

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
}

void TestBothAlloc()
{
    using namespace APMemory;

    VeryBigData* vbd = new(Alloc(sizeof(VeryBigData))) VeryBigData;

    std::vector<Byte48*> Obj48; Obj48.reserve(100);

    for (int i = 0; i < 100; ++i)
    {
        Byte48* newObj = new(Alloc(sizeof(Byte48))) Byte48();

        Obj48.push_back(newObj);
    }
    std::cout << "\n\n\n";
    assert(Obj48.size() == 100);

    VeryBigData* vbd2 = new(Alloc(sizeof(VeryBigData))) VeryBigData;
    VeryBigData* vbd3 = new(Alloc(sizeof(VeryBigData))) VeryBigData;

    Delete(vbd2);

    for (int i = 0; i < 100; ++i)
    {
        Byte48* oldObj = Obj48.back();
        Delete(oldObj);
        Obj48.pop_back();
    }
    std::cout << "\n\n\n";

    Delete(vbd3);
    Delete(vbd);
}

void TestNewDelete()
{
    Byte48* one = new Byte48;
    delete one;

    VeryBigData* two = new VeryBigData;
    delete two;

    Test* three = new Test[3];
    delete[] three;
}

int main()
{
    TestNewDelete();
}
