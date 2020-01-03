//
// Created by Trygve Bærland on 2019-06-18.
//
#include "keeping_reference.h"

shared_ptr<my_class> some_instances::add_instance(int ai){
    auto tm = std::make_shared<my_class>(ai);
    _instances.push_back(tm);
    return tm;
}
