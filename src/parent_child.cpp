//
// Created by Trygve Bærland on 2019-06-17.
//
#include <iostream>
#include "parent_child.h"
using namespace std;

void parent::set_child(shared_ptr<child> &achild) {
    cout << "Beginning of setting child to parent\n";
    my_child = achild;
    auto temp = shared_from_this();
    achild->set_parent(temp);
    cout << "\tDone assigning child to parent\n";
}

bool parent::operator==(const parent &o) const {
    cout << "Comparing two parents by checking if they have the same child\n";
    return equals(get_child(), o.get_child()) && get_id() == o.get_id();
}

bool child::operator==(const child &o) const {
    cout << "Comparing two children by checking if they have the same parent\n";
    return id == o.get_id();
}
