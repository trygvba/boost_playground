//
// Created by Trygve Bærland on 2019-06-14.
//

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "my_class.h"
#include "parent_child.h"
#include "keeping_reference.h"

BOOST_PYTHON_MODULE(_playground){
    using namespace boost::python;
    scope().attr("__doc__") = "My playing with Boost.Python.";
    docstring_options doc_options(true, true, false);

    class_<my_class, bases<>, std::shared_ptr<my_class>, boost::noncopyable>
        ("MyClass", init<int>())
        .add_property("a", &my_class::get_a, &my_class::set_a)
        .def(self==self)
        .def(self!=self)
        ;

    typedef std::vector<shared_ptr<my_class>> vMyClass;
    class_<vMyClass>("MyClassList")
            .def(vector_indexing_suite<vMyClass, true>())
            ;

    class_<parent, bases<>, std::shared_ptr<parent>, boost::noncopyable>
            ("Parent", init<int>())
            .add_property("child", &parent::get_child, &parent::set_child)
            .add_property("id", &parent::get_id)
            .add_property("count", &parent::get_count)
            .def(self==self)
            .def(self!=self)
            ;

    class_<child, bases<>, std::shared_ptr<child>, boost::noncopyable>
            ("Child", init<int>())
            .add_property("parent", &child::get_parent, +[](child& self, std::shared_ptr<parent> &aparent) {self.set_parent(aparent);})
            .add_property("id", &child::get_id)
            .add_property("count", &child::get_count)
            .def(self == self)
            .def(self!=self)
            ;

    class_<some_instances, bases<>, std::shared_ptr<some_instances>, boost::noncopyable>
            ("SomeInstances", init<>())
            .def("add_instance", &some_instances::add_instance)
            .add_property("instances", &some_instances::get_instances)
            ;
}
