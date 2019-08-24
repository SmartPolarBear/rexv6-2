#include "boost/test/unit_test.hpp"
#include "klib/vector.h"
#include "iostream"

using klib::Vector;
using std::cin;
using std::cout;
using std::endl;

struct data
{
    int a, b, c;
};

template <typename T>
struct allocator
{
    void release(T *p)
    {
        // cout << "allocator.release" << endl;
        delete[] p;
    }

    T *allocate(unsigned int size)
    {
        // cout << "allocator.allocate" << endl;

        return new T[size];
    }
};

BOOST_AUTO_TEST_CASE(VectorTest)
{
    Vector<data, allocator<data>> vec;
    cout << "push_back" << endl;
    vec.push_back({2, 3, 4});
    BOOST_REQUIRE(vec.size() == 1);
    BOOST_REQUIRE_EQUAL(vec[0].a, 2);
    BOOST_REQUIRE_EQUAL(vec[0].b, 3);
    BOOST_REQUIRE_EQUAL(vec[0].c, 4);
}