#pragma once
#include <iostream>

namespace playground {
    class Base {
    protected:
        int val = 0;
    public:
        virtual ~Base() {}
        virtual std::string to_string() const = 0;
    };

    std::ostream& operator<<(std::ostream& o, const Base& b) {
        return o << b.to_string();
    }


    class D1: public Base {
    public:
        D1() { val=3; };
        virtual std::string to_string() const {
            return "Derived #1: " + std::to_string(val);
        }
    };

    class D2: public Base {
    public:
        D2() { val = 4; };
        virtual std::string to_string() const {
            return "Derived #2: " + std::to_string(val);
        }
    };
}
