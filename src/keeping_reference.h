//
// Created by Trygve Bærland on 2019-06-18.
//

#ifndef BOOST_PLAYGROUND_KEEPING_REFERENCE_H
#define BOOST_PLAYGROUND_KEEPING_REFERENCE_H

#include "my_class.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::weak_ptr;
using std::vector;

class some_instances {
private:
    vector<shared_ptr<my_class>> _instances;
public:
    some_instances() {};
    shared_ptr<my_class> add_instance(int ai);
    vector<shared_ptr<my_class>> get_instances() {return _instances;}
};

#endif //BOOST_PLAYGROUND_KEEPING_REFERENCE_H
