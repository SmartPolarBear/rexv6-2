#include "boost/test/unit_test.hpp"
#include "klib/queue.h"
#include "iostream"
#include "ostream"
#include "algorithm"

using klib::PriorityQueue;
using klib::Vector;

using std::cin;
using std::cout;
using std::endl;
using std::ostream;
using std::swap;

BOOST_AUTO_TEST_SUITE(PriorityQueueTest);

BOOST_AUTO_TEST_CASE(BASIC)
{
    PriorityQueue<int> heap;
    PriorityQueue<int, Vector<int>, klib::greater<int>> heap2;
    int data[] = {12, 3234, 23, 3424, 505, 32132, 32456, 23321, -3213, 12323};
    for (auto d : data)
    {
        heap.push(d);
        heap2.push(d);
    }

    int buf[10] = {0}, cnt = 0;
    while (!heap.empty())
    {
        int t = heap.top();
        heap.pop();
        buf[cnt++] = t;
    }

    for (int i = 1; i < cnt; i++)
    {
        BOOST_REQUIRE_GT(buf[i - 1], buf[i]);
    }

    std::fill(std::begin(buf), std::end(buf), 0);
    cnt = 0;

    while (!heap2.empty())
    {
        int t = heap2.top();
        heap2.pop();
        buf[cnt++] = t;
    }

    for (int i = 1; i < cnt; i++)
    {
        BOOST_REQUIRE_LT(buf[i - 1], buf[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()