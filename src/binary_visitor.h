//
// Created by Trygve Bærland on 2020-03-20.
//
#pragma once
#include <memory>
#include <boost/variant.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace bin_visit {
    class A {
    public:
        A(int a): a{a}, mtx_{std::make_shared<boost::shared_mutex>()} {}

        bool operator==(const A& o) const { return a == o.a; }
    private:
        int a;
        std::shared_ptr<boost::shared_mutex> mtx_;
    };

    class B {
    public:
        B(char b): b{b}, mtx_{std::make_shared<boost::shared_mutex>()} {}

        bool operator==(const B& o) const { return b == o.b; }
    private:
        char b;
        std::shared_ptr<boost::shared_mutex> mtx_;
    };

    using AB = boost::variant<A, B>;

    class AB_comp_visitor: public boost::static_visitor<bool> {
    public:
        template<typename L, typename R>
        bool operator()(const L& lhs, const R& rhs) const { return false; }

        template<typename V>
        bool operator()(const V& lhs, const V& rhs) const { return lhs == rhs; }
    };
}
