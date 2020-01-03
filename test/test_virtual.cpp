#include "virtual_stuff.h"
#include <doctest/doctest.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

using std::vector;
using namespace playground;
TEST_SUITE("virtual") {
    TEST_CASE("unique_ptr") {
        vector<std::unique_ptr<Base>> ptrs;
        ptrs.push_back(std::make_unique<D1>());
        ptrs.push_back(std::make_unique<D1>());
        ptrs.push_back(std::make_unique<D2>());
        ptrs.push_back(std::make_unique<D2>());

        std::for_each(ptrs.begin(), ptrs.end(),
            [](auto& item) {std::cout << *item << "\n";});
    }
}
