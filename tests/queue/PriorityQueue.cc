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

BOOST_AUTO_TEST_CASE(MinHeapMaxHeap)
{
    PriorityQueue<int> heap;
    PriorityQueue<int, Vector<int>, klib::greater<int>> heap2;
    int data[] = {12, 3234, 23, 3424, 505, 32132, 32456, 23321, -3213, 12323};
    for (auto d : data)
    {
        heap.push(d);
        heap2.push(d);
    }

    BOOST_REQUIRE_EQUAL(heap.size(), std::size(data));
    BOOST_REQUIRE_EQUAL(heap2.size(), std::size(data));
    BOOST_REQUIRE(!heap.empty());
    BOOST_REQUIRE(!heap2.empty());

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

    BOOST_REQUIRE_EQUAL(heap.size(), 0);
    BOOST_REQUIRE_EQUAL(heap2.size(), 0);
    BOOST_REQUIRE(heap.empty());
    BOOST_REQUIRE(heap2.empty());
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
    PriorityQueue<int> heap;
    int data[] = {12, 3234, 23, 3424, 505, 32132, 32456, 23321, -3213, 12323};
    for (auto d : data)
    {
        heap.push(d);
    }
    PriorityQueue<int> heap2(heap);

    BOOST_REQUIRE_EQUAL(heap.size(), heap2.size());
    BOOST_REQUIRE_EQUAL(heap.empty(), heap2.empty());

    while (!heap.empty())
    {
        BOOST_REQUIRE_EQUAL(heap.top(), heap2.top());
        heap.pop();
        heap2.pop();
    }

    BOOST_REQUIRE_EQUAL(heap.size(), heap2.size());
    BOOST_REQUIRE_EQUAL(heap.empty(), heap2.empty());
}

BOOST_AUTO_TEST_SUITE_END()