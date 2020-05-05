#include <doctest/doctest.h>
#include "binary_visitor.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

TEST_SUITE("binary_visitation") {
    using namespace bin_visit;
    TEST_CASE("AB_tests") {
        AB a1 = A(1);
        AB a2 = A(2);
        A a3(1);
        AB b1 = B('a');
        AB b2 = B('b');
        B b3('a');

        AB_comp_visitor vis;
        CHECK_EQ(false, boost::apply_visitor(vis, a1, a2));
        CHECK_EQ(true, boost::apply_visitor(vis, a1, (AB) a3));
        CHECK_EQ(false, boost::apply_visitor(vis, b1, b2));
        CHECK_EQ(true, boost::apply_visitor(vis, b1, (AB) b3));
        CHECK_EQ(false, boost::apply_visitor(vis, a1, b2));
    }
}

