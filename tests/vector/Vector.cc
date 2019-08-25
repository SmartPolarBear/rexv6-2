#include "boost/test/unit_test.hpp"
#include "klib/vector.h"
#include "iostream"
#include "ostream"

using std::ostream;

using klib::Vector;
using std::cin;
using std::cout;
using std::endl;

struct data
{
    int a, b, c;
    bool operator==(data d) const
    {
        return a == d.a && b == d.b && c == d.c;
    }

    friend ostream &operator<<(ostream &os, const data &dt)
    {
        return os << dt.a << " " << dt.b << " " << dt.c << " ";
    }
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

BOOST_AUTO_TEST_SUITE(VectorTest);

BOOST_AUTO_TEST_CASE(PushBackPopBack)
{
    Vector<data, allocator<data>> vec;

    data element = {2, 3, 4}, element2 = {5, 4, 3};
    vec.push_back(element);

    BOOST_REQUIRE_EQUAL(vec.size(), 1);
    BOOST_REQUIRE_GE(vec.capacity(), vec.size());
    BOOST_REQUIRE_EQUAL(vec.empty(), false);
    BOOST_REQUIRE_EQUAL(vec[0], element);

    vec.push_back(element2);
    vec.push_back(element);

    BOOST_REQUIRE_EQUAL(vec.size(), 3);
    BOOST_REQUIRE_GE(vec.capacity(), vec.size());
    BOOST_REQUIRE_EQUAL(vec.empty(), false);
    BOOST_REQUIRE_EQUAL(vec[1], element2);
    BOOST_REQUIRE_EQUAL(vec[2], element);

    vec.pop_back();

    BOOST_REQUIRE_EQUAL(vec.size(), 2);
    BOOST_REQUIRE_GE(vec.capacity(), vec.size());
    BOOST_REQUIRE_EQUAL(vec.empty(), false);
    BOOST_REQUIRE_EQUAL(vec[1], element2);

    vec.pop_back();
    vec.pop_back();

    BOOST_REQUIRE_EQUAL(vec.size(), 0);
    BOOST_REQUIRE_GE(vec.capacity(), vec.size());
    BOOST_REQUIRE_EQUAL(vec.empty(), true);
}

BOOST_AUTO_TEST_SUITE_END()