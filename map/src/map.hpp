/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;

  /*节点类，需要我们维护pa*/
  class Node
  {
  public:
    Node* parent;
    Node* left;
    Node* right;
    size_t color; /* 0 red|1 black|2 double black*/
    value_type value;

    /*默认构造函数不存在*/
    /*至少传入一个value*/
    Node(value_type val):value(val){
      parent = nullptr;
      left = nullptr;
      right = nullptr;
      color = 0; /*默认为红节点*/
    }
    /*有跌的构造函数*/
    Node(value_type val, Node* papa):value(val){
      parent = papa;
      left = nullptr;
      right = nullptr;
      color = 0;
    }
    /*析构*/
    ~Node(){
      parent = nullptr;
      left = nullptr;
      right = nullptr;
      color = 0;
      value.~pair();
    }

    /*Node的复制构造函数*/
    Node(Node* other){
      /*Node的复制我们直接一个Node复制整颗树出来,用于map里面的构造*/
      if(other == nullptr) return;
      else{
        this->color = other->color;
        this->value = other->value;
        this->left = new Node*(other->left);
        this->right = new Node*(other->right);
        this->parent = new Node*(other->parent);
      }
    }
  };



  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
   private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
   public:
    iterator() {
      // TODO
    }

    iterator(const iterator &other) {
      // TODO
    }

    /**
     * TODO iter++
     */
    iterator operator++(int) {}

    /**
     * TODO ++iter
     */
    iterator &operator++() {}

    /**
     * TODO iter--
     */
    iterator operator--(int) {}

    /**
     * TODO --iter
     */
    iterator &operator--() {}

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {}

    bool operator==(const iterator &rhs) const {}

    bool operator==(const const_iterator &rhs) const {}

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {}

    bool operator!=(const const_iterator &rhs) const {}

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {}
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
   public:
    const_iterator() {
      // TODO
    }

    const_iterator(const const_iterator &other) {
      // TODO
    }

    const_iterator(const iterator &other) {
      // TODO
    }
    // And other methods in iterator.
    // And other methods in iterator.
    // And other methods in iterator.
  };

  /*
  *map的参数
  *
  */
  Node* root_node;
  Compare cmp;
  size_t current_size;
  /**
   * TODO two constructors
   */
  map() {
    root_node = new Node*();
    current_size = 0;
  }

  map(const map &other) {
    /*直接用内置的超级爆爆爆NOOOOOOOOOOOOODEEEEEEEEEEEEEEEEE构造函数整个给你杀了*/
    this->root_node = new Node(other.root_node);
    this->cmp = other.cmp;
    this->current_size = other.current_size;
  }

  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {
    if(this == &other) return *this;
    else{
      /*删除自己*/
      delete_node(this->root_node);
      /*开始复制*/
      this->root_node = new Node(other.root_node);
      this->cmp = other.cmp;
      this->current_size = other.current_size;
    }
  }

  /*辅助函数，删除Node的函数*/
  void delete_node(Node* node){
    if(node == nullptr) return;
    delete_node(node->left);
    delete_node(node->right);
    node->~Node();
  }
  /**
   * TODO Destructors
   */
  ~map() {
    /*从根节点一个一个调用析构函数*/
    delete_node(this->root_node);
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    /*BST tree 的 查找*/
    Node* search_node = root_node;
    while(search_node && !cmp(search_node->value.first,key) && !cmp(key,search_node->value.first)){
      if(cmp(search_node->value.first , key )) search_node = search_node->right;
      else{
        search_node = search_node->left;
      }
    }
    if(search_node == nullptr){
      throw index_out_of_bound();
    }else{
      return search_node->value.second;
    }
  }

  const T &at(const Key &key) const {
    /*BST tree 的 查找*/
    Node* search_node = root_node;
    while(search_node && !cmp(search_node->value.first,key) && !cmp(key,search_node->value.first)){
      if(cmp(search_node->value.first , key )) search_node = search_node->right;
      else{
        search_node = search_node->left;
      }
    }
    if(search_node == nullptr){
      throw index_out_of_bound();
    }else{
      return search_node->value.second;
    }
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    Node* search_node = root_node;
    /*这个地方涉及到插入,我们先不要处理*/
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    /*BST tree 的 查找*/
    Node* search_node = root_node;
    while(search_node && !cmp(search_node->value.first,key) && !cmp(key,search_node->value.first)){
      if(cmp(search_node->value.first , key )) search_node = search_node->right;
      else{
        search_node = search_node->left;
      }
    }
    if(search_node == nullptr){
      throw index_out_of_bound();
    }else{
      return search_node->value.second;
    }
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {}

  const_iterator cbegin() const {}

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {}

  const_iterator cend() const {}

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return this->current_size == 0;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
    return this->current_size;
  }

  /**
   * clears the contents
   */
  void clear() {
    delete_node(this->root_node->left);
    delete_node(this->root_node->right);
    this->root_node->value.~pair();
    this->current_size = 0 ;
    this->root_node->left = nullptr;
    this->root_node->right = nullptr;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {}

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {}

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    Node* search_node = root_node;
    while(search_node != nullptr && !cmp(search_node->value.key , key) && !cmp(key , search_node->value.key)){
      if(cmp(search_node->value.first , key )) search_node = search_node->right;
      else{
        search_node = search_node->left;
      }
    }
    if(search_node == nullptr) return 0;
    return 1;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {}

  const_iterator find(const Key &key) const {}
};

}

#endif
