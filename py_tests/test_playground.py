#!/usr/bin/env python

from ..lib._playground import *

def test_parent_child():
    p1 = Parent(1)
    assert p1.child is None
    c1 = Child(101)
    assert c1.parent is None
    c1.parent = p1
    assert c1.parent == p1
    p1.child = c1
    assert p1.child == c1
    assert c1.parent == p1
    p2 = Parent(2)
    c2 = Child(102)
    p2.child = c2
    assert p1 != p2 # Means p1 and p2 have different children
    assert c1 != c2 # Means c1 and c2 have different parents
    print(f"ID of p1: {p1.id}")
    print(f"ID of p2: {p2.id}")
    p1.child = c2
    assert p1.child == p2.child

def test_keeping_references():
    insts = SomeInstances()
    i0 = insts.add_instance(101)
    assert isinstance(i0, MyClass)
    i1 = insts.add_instance(102)
    linsts = [i0, i1]
    for l, r in zip(insts.instances, linsts):
        assert l == r
        assert id(l) == id(r)   # NOTE: Different objects, even though underlying C++ is the same.
    
