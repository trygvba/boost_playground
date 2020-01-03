//
// Created by Trygve Bærland on 2019-06-14.
//

#ifndef BOOST_PLAYGROUND_MY_CLASS_H
#define BOOST_PLAYGROUND_MY_CLASS_H
#include <unistd.h>

class my_class {
private:
    int a;
public:
    my_class(int aa): a(aa) {}
    int get_a() const;
    void set_a(int b);

    bool operator==(my_class const &o) const { return a == o.get_a();}
    bool operator!=(my_class const &o) const {return !operator==(o);}

};

#endif //BOOST_PLAYGROUND_MY_CLASS_H
