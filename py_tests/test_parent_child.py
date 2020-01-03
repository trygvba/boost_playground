#!/usr/bin/env python
import sys
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


def test_reference_count():
    p1 = Parent(1)
    assert sys.getrefcount(p1) == 2
    assert p1.count == 1, "only p1"
    p2 = p1
    assert sys.getrefcount(p2) == 3
    assert p1.count == 2, "p1 and p2"
    c1 = Child(101)
    assert c1.count == 1, "only c1"
    c1.parent = p1
    p1.child = c1
    assert c1.count == 3, "c1, p1 and p2"
    del p1
    assert c1.count == 2, "c1 and p2"
    del p2
    assert c1.count == 1, "only c1"
    
