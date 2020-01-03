add_library(MyClass STATIC my_class.cpp)
add_library(ParentChild STATIC parent_child.cpp)
add_library(KeepingReference STATIC keeping_reference.cpp)
target_link_libraries(KeepingReference MyClass)
