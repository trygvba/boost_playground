//
// Created by Trygve Bærland on 2019-06-17.
//

#ifndef BOOST_PLAYGROUND_PARENT_CHILD_H
#define BOOST_PLAYGROUND_PARENT_CHILD_H

#include <memory>

using std::shared_ptr;
using std::weak_ptr;

class parent;
class child;

template<typename T>
inline bool equals(const shared_ptr<T>& t1, const shared_ptr<T>& t2){
    return !t1.owner_before(t2) && !t2.owner_before(t1);
}

class parent: public std::enable_shared_from_this<parent>{
private:
    shared_ptr<child> my_child;
    int id;
public:
    parent(int aid=0): id(aid), my_child(nullptr) {};
    parent(const parent&) = delete;
    parent(parent &&) = delete;
    parent& operator=(const parent&) = delete;
    parent& operator=(parent &&) = delete;

    shared_ptr<child> get_child() const { return my_child; }
    void set_child(shared_ptr<child> &a_child);
    int get_id() const {return id;}
    int get_count() const { return shared_from_this().use_count() - 1; }
    bool operator==(const parent &o) const;
    bool operator!=(const parent &o) const {return !operator==(o);}
};

class child: public std::enable_shared_from_this<child> {
private:
    weak_ptr<parent> my_parent;
    int id;
public:
    child(int aid=0): id(aid) {};
    void set_parent(shared_ptr<parent> const &a_parent){ my_parent = a_parent; }
    shared_ptr<parent> get_parent() const { return my_parent.lock(); }
    int get_id() const { return id; }
    int get_count() const { return shared_from_this().use_count() - 1; };

    bool operator==(const child &o) const;
    bool operator!=(const child &o) const { return !operator==(o); }
};
#endif //BOOST_PLAYGROUND_PARENT_CHILD_H
