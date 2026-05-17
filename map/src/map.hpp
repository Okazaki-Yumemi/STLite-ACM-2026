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
    }

    /*Node的复制构造函数不要了*/
    
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

   public:
    
    map* owner;
    Node* node;

    iterator() {
      this->owner = nullptr;
      this->node = nullptr;
    }

    iterator(const iterator &other) {
      // TODO
      this->owner = other.owner;
      this->node = other.node;
    }

    /**
     * TODO iter++
     */
    iterator operator++(int) {
      if(owner == nullptr || node == nullptr) throw invalid_iterator();
      iterator new_iterator;
      Node* current_node = this->node;

      new_iterator.owner = this->owner;
      new_iterator.node = this->node;

      if(current_node->right != nullptr){
        this->node = current_node->right;
        while (this->node->left != nullptr)
        {
          this->node = this->node->left;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->right == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return new_iterator;
    }

    /**
     * TODO ++iter
     */
    iterator &operator++() {
      if(owner == nullptr || node == nullptr) throw invalid_iterator();
      Node* current_node = this->node;
      if(current_node->right != nullptr){
        this->node = current_node->right;
        while (this->node->left != nullptr)
        {
          this->node = this->node->left;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->right == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return *this;
    }

    /**
     * TODO iter--
     */
    iterator operator--(int) {
      if(*this == this->owner->begin()) throw invalid_iterator();
      iterator new_iterator;
      Node* current_node = this->node;

      new_iterator.owner = this->owner;
      new_iterator.node = this->node;
      
      if(current_node == nullptr){
        current_node = owner->root_node;
        while (current_node -> right != nullptr)
        {
          current_node = current_node ->right;
        }
        this->node = current_node;
        return *this;
      }


      if(current_node->left != nullptr){
        this->node = current_node->left;
        while (this->node->right != nullptr)
        {
          this->node = this->node->right;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->left == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      
      return new_iterator;
    }

    /**
     * TODO --iter
     */
    iterator &operator--() {
      if(*this == this->owner->begin()) throw invalid_iterator();
      Node* current_node = this->node;
      if(current_node == nullptr){
        current_node = owner->root_node;
        while (current_node -> right != nullptr)
        {
          current_node = current_node ->right;
        }
        this->node = current_node;
        return *this;
      }

      if(current_node->left != nullptr){
        this->node = current_node->left;
        while (this->node->right != nullptr)
        {
          this->node = this->node->right;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->left == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return *this;
    }

    
    value_type &operator*() const {
      if(this->node == nullptr) throw invalid_iterator();
      return this->node->value;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    bool operator==(const iterator &rhs) const {
      return (this->owner == rhs.owner) && (this->node == rhs.node);
    }

    bool operator==(const const_iterator &rhs) const {
      return (this->owner == rhs.owner) && (this->node == rhs.node);
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !((this->owner == rhs.owner) && (this->node == rhs.node));
    }

    bool operator!=(const const_iterator &rhs) const {
      return !((this->owner == rhs.owner) && (this->node == rhs.node));
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {
      if(this->node == nullptr) throw invalid_iterator();
      return &(this->node->value);
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
   public:
    const map* owner;
    Node* node;
    const_iterator() {
      // TODO
      this->owner = nullptr;
      this->node = nullptr;
    }

    const_iterator(const const_iterator &other) {
      // TODO
      this->owner = other.owner;
      this->node = other.node;
    }

    const_iterator(const iterator &other) {
      // TODO
      this->owner = other.owner;
      this->node = other.node;
    }
    // And other methods in iterator.
    const_iterator operator++(int){
      if(owner == nullptr || node == nullptr) throw invalid_iterator();
      const_iterator new_iterator;
      Node* current_node = this->node;

      new_iterator.owner = this->owner;
      new_iterator.node = this->node;

      if(current_node->right != nullptr){
        this->node = current_node->right;
        while (this->node->left != nullptr)
        {
          this->node = this->node->left;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->right == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return new_iterator;
    }

    const_iterator &operator++() {
      if(owner == nullptr || node == nullptr) throw invalid_iterator();
      Node* current_node = this->node;
      if(current_node->right != nullptr){
        this->node = current_node->right;
        while (this->node->left != nullptr)
        {
          this->node = this->node->left;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->right == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return *this;
    }

    const_iterator operator--(int) {
      if(*this == this->owner->cbegin()) throw invalid_iterator();
      const_iterator new_iterator;
      Node* current_node = this->node;

      new_iterator.owner = this->owner;
      new_iterator.node = this->node;

      if(current_node == nullptr){
        current_node = owner->root_node;
        while (current_node -> right != nullptr)
        {
          current_node = current_node ->right;
        }
        this->node = current_node;
        return *this;
      }

      if(current_node->left != nullptr){
        this->node = current_node->left;
        while (this->node->right != nullptr)
        {
          this->node = this->node->right;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->left == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      
      return new_iterator;
    }

    const_iterator &operator--() {
      if(*this == this->owner->cbegin()) throw invalid_iterator();
      Node* current_node = this->node;

      if(current_node == nullptr){
        current_node = owner->root_node;
        while (current_node -> right != nullptr)
        {
          current_node = current_node ->right;
        }
        this->node = current_node;
        return *this;
      }

      if(current_node->left != nullptr){
        this->node = current_node->left;
        while (this->node->right != nullptr)
        {
          this->node = this->node->right;
        }
      }else{
        while (this->node->parent != nullptr && (this->node->parent->left == this->node)){
          this->node = this->node->parent;
        }
        this->node = this->node->parent;
      }
      return *this;
    }

    value_type &operator*() const {
      if(this->node == nullptr) throw invalid_iterator();
      return this->node->value;
    }

    bool operator==(const iterator &rhs) const {
      return (this->owner == rhs.owner) && (this->node == rhs.node);
    }

    bool operator==(const const_iterator &rhs) const {
      return (this->owner == rhs.owner) && (this->node == rhs.node);
    }

    bool operator!=(const iterator &rhs) const {
      return !((this->owner == rhs.owner) && (this->node == rhs.node));
    }

    bool operator!=(const const_iterator &rhs) const {
      return !((this->owner == rhs.owner) && (this->node == rhs.node));
    }

    value_type *operator->() const
    noexcept {
      if(this->node == nullptr) throw invalid_iterator();
      return &(this->node->value);
    }
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
    root_node = nullptr;
    current_size = 0;
  }



  map(const map &other) {
    /*直接用内置的超级爆爆爆NOOOOOOOOOOOOODEEEEEEEEEEEEEEEEE构造函数整个给你杀了*/
    this->root_node = clone_tree(other.root_node);
    this->cmp = other.cmp;
    this->current_size = other.current_size;
  }

  Node* clone_tree(const Node* other){
    if(other == nullptr) return nullptr;
    Node* this_one = new Node(other->value);

    this_one->color = other->color;

    Node* left = clone_tree(other->left);

    Node* right = clone_tree(other->right);

    this_one->left = left;
    if (left != nullptr) left->parent = this_one;
    this_one->right = right;
    if (right != nullptr)right->parent = this_one;

    return this_one;
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
      this->root_node = clone_tree(other.root_node);
      this->cmp = other.cmp;
      this->current_size = other.current_size;

      return *this;
    }
  }

  /*辅助函数，删除Node的函数*/
  void delete_node(Node* node){
    if(node == nullptr) return;
    delete_node(node->left);
    delete_node(node->right);
    delete node;
  }
  /**
   * TODO Destructors
   */
  ~map() {
    /*从根节点一个一个调用析构函数*/
    delete_node(this->root_node);
  }

  bool equal_key(const Key & key , Node* node) const{
    return (!cmp(node->value.first , key) && !cmp(key,node->value.first));
  }


  /*写一个统一的Find node*/
  Node* find_node(const Key &key) const {
    Node* search_node = root_node;
    while(search_node != nullptr && !equal_key(key,search_node)){
      /*key大*/
      if(cmp(search_node->value.first , key)) search_node = search_node->right;
      else{
        search_node = search_node->left;
      }
    }
    /*返回nullptr 或者 目标节点*/
    return search_node;
  }



  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    /*BST tree 的 查找*/
    Node* result = find_node(key);
    if(result == nullptr) throw index_out_of_bound();
    else{
      return result->value.second;
    }
  }

  const T &at(const Key &key) const {
    /*BST tree 的 查找*/
    Node* result = find_node(key);
    if(result == nullptr) throw index_out_of_bound();
    else{
      return result->value.second;
    }
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    Node* node = find_node(key);

    if(node != nullptr){
      return node->value.second;
    }else{
      value_type val = pair<const Key,T>(key,T());
      pair<Node*,bool> result = insert_val(val);
      return result.first->value.second;
    }
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    /*BST tree 的 查找*/
    Node* result = find_node(key);
    if(result == nullptr) throw index_out_of_bound();
    else{
      return result->value.second;
    }
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    iterator it;
    Node* search_node = root_node;
    while(search_node!= nullptr && search_node->left != nullptr){
      search_node = search_node->left;
    }

    it.node = search_node;
    it.owner = this;
    return it;
  }

  const_iterator cbegin() const {
    const_iterator it;
    Node* search_node = root_node;
    while(search_node!= nullptr && search_node->left != nullptr){
      search_node = search_node->left;
    }

    it.node = search_node;
    it.owner = this;
    return it;
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    iterator it;
    it.node = nullptr;
    it.owner = this;
    return it;
  }

  const_iterator cend() const {
    const_iterator it;
    it.node = nullptr;
    it.owner = this;
    return it;
  }

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
    delete_node(root_node);
    root_node = nullptr;
    this->current_size = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    iterator it;
    pair<Node*, bool> result = insert_val(value);

    it.node = result.first;
    it.owner = this;

    return pair<iterator, bool>(it, result.second);
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if(pos == this->end() || pos.owner != this) throw invalid_iterator();
    Node* node = pos.node;
    map_delete_node(node);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    Node* search_node = this->find_node(key);
    if(search_node == nullptr) return 0 ;
    return 1;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    Node* target_node = find_node(key);
    iterator it;
    it.node = target_node;
    it.owner = this;
    return it;
  }

  const_iterator find(const Key &key) const {
    Node* target_node = find_node(key);
    const_iterator it;
    it.node = target_node;
    it.owner = this;
    return it;
  }

  /*赋值函数，根据val插入,自动完成插入调整*/
  pair<Node*,bool> insert_val(const value_type& val){
    if(this->current_size == 0){
      this->root_node = new Node(val);
      this->current_size ++;
      this->root_node->color = 1;
      return pair<Node*,bool>(root_node,true);
    }

    /*一次查找*/
    Node* search_node = this->root_node;
    Node* parent = nullptr;
    while(search_node != nullptr){
      parent = search_node;

      /*key 相等*/
      if(equal_key(val.first,search_node)){
        return pair<Node*,bool>(search_node,false);
      }

      /*当前的 < 新的*/
      if(cmp(search_node->value.first , val.first)){
        search_node = search_node->right;
      }else{
        search_node = search_node->left;
      }
    }

    /* searchnode 滚到 nullptr了 */
    Node * new_node = new Node(val,parent);

    if(cmp(new_node->value.first , parent->value.first)){
      parent->left = new_node;
    }else{
      parent->right = new_node;
    }

    /*插入修复*/
    insert_adjust(new_node);

    this->current_size ++;
    return pair<Node*,bool>(new_node,true);
  }


  /*调整函数*/
  void insert_adjust(Node* node){
    if(node == this->root_node){
      /*是根？染黑色，回去*/
      node->color = 1;
      return;
    }
    /*不是根*/
    node->color = 0; /*染红色*/
    Node* fahter = node->parent;
    if(fahter->color == 1){
      /*我们这个地方特殊的插入只会违背连续两个红色节点的规则，所以只用检查这一个*/
      /*父亲黑，那没事了*/
      return;
    }else{
      /*父亲是红色的*/
      /*因为父亲节点是红色的，父亲节点不可能是根节点，所以我们可以肯定grandpa存在*/
      Node* grandpa = fahter->parent;
      Node* uncle = nullptr;
      if(grandpa->left == fahter) uncle = grandpa->right;
      else{
        uncle = grandpa->left;
      }
      /*看叔叔什么情况了*/
      if(uncle != nullptr && uncle->color == 0){
        /*叔叔是红色*/
        /*颜色取反*/
        grandpa->color = (grandpa->color == 1)? 0:1;
        uncle->color = (uncle->color == 1)? 0:1;
        fahter->color = (fahter->color == 1)? 0:1;
        /*递归处理爷爷*/
        insert_adjust(grandpa);
      }else{
        /*叔叔是黑色*/
        if(grandpa->left == fahter && fahter->left == node){
          /*LL*/
          Node* great_grandpa = grandpa->parent;
          if(great_grandpa == nullptr){
            /*太爷爷是空的*/
            /*证明爷爷是根节点*/
            this->root_node = this->LL(grandpa);
          }else{
            /*太爷爷不是空的，看是左侧还是右侧*/
            size_t left_right = 0;
            if(great_grandpa->right == grandpa) left_right = 1;
            if(left_right){
              great_grandpa->right = LL(grandpa);
            }else{
              great_grandpa->left = LL(grandpa);
            }
          }
          /*结束*/
          return;
        }
        else if(grandpa->right == fahter && fahter ->right == node){
          /*RR*/
          Node* great_grandpa = grandpa->parent;
          if(great_grandpa == nullptr){
            /*太爷爷空*/
            this->root_node = this->RR(grandpa);
          }else{
            size_t left_right = 0;
            if(great_grandpa->right == grandpa) left_right = 1;
            if(left_right){
              great_grandpa->right = RR(grandpa);
            }else{
              great_grandpa->left = RR(grandpa);
            }
          }
          /*结束*/
          return;
        }
        else if(grandpa->right == fahter && fahter->left == node){
          /*RL*/
          Node* great_grandpa = grandpa->parent;
          if(great_grandpa == nullptr){
            /*太爷爷空*/
            this->root_node = this->RL(grandpa);
          }else{
            size_t left_right = 0;
            if(great_grandpa->right == grandpa) left_right = 1;
            if(left_right){
              great_grandpa->right = RL(grandpa);
            }else{
              great_grandpa->left = RL(grandpa);
            }
          }
          /*结束*/
          return;
        }
        else if(grandpa->left == fahter && fahter->right == node){
          /*LR*/
          Node* great_grandpa = grandpa->parent;
          if(great_grandpa == nullptr){
            /*太爷爷空*/
            this->root_node = this->LR(grandpa);
          }else{
            size_t left_right = 0;
            if(great_grandpa->right == grandpa) left_right = 1;
            if(left_right){
              great_grandpa->right = LR(grandpa);
            }else{
              great_grandpa->left = LR(grandpa);
            }
          }
          /*结束*/
          return;
        }
      }
    }
  }


  /*注意，这个地方还得修改太爷爷的left or right 指针*/
  Node* LL(Node* grandpa){
    Node* father = grandpa->left;
    Node* son = father->left;
    /*首先*/
    grandpa->left = father->right;
    /*然后*/
    father->right = grandpa;

    if(grandpa->left != nullptr) grandpa->left->parent = grandpa;

    father->parent = grandpa->parent;
    grandpa->parent = father;

    /*染色*/
    size_t father_color = father->color;
    size_t grandpa_color = grandpa->color;
    father->color = grandpa_color;
    grandpa->color = father_color;
    return father;
  }
  /*注意，这个地方还得修改太爷爷的left or right 指针*/
  Node* RR(Node* grandpa){
    Node* father = grandpa->right;
    Node* son = father->right;

    /*首先*/
    grandpa->right = father->left;
    father->left = grandpa;

    if(grandpa->right != nullptr) grandpa->right->parent = grandpa;

    father->parent = grandpa->parent;
    grandpa->parent = father;

    /*染色*/
    size_t father_color = father->color;
    size_t grandpa_color = grandpa->color;
    father->color = grandpa_color;
    grandpa->color = father_color;
    return father;
  }
  /*注意，这个地方还得修改太爷爷的left or right 指针*/
  Node* LR(Node* grandpa){
    Node* father = grandpa->left;
    Node* son = father->right;

    father->right = son->left;
    son->left = father;

    if(father->right != nullptr) father->right->parent = father;

    father->parent = son;
    son->parent = grandpa;
    grandpa->left = son;

    return LL(grandpa);
  }
  /*注意，这个地方还得修改太爷爷的left or right 指针*/
  Node* RL(Node* grandpa){
    Node* father = grandpa->right;
    Node* son = father->left;

    father->left = son->right;
    son->right = father;

    if(father->left != nullptr) father->left->parent = father;

    father->parent = son;
    son->parent = grandpa;
    grandpa->right = son;

    return RR(grandpa);
  }
  /*
  双孩子删除专用：
  把 node 与其直接后继 successor 的“树位置”交换。

  交换后：
  - successor 占据 node 原来的位置
  - node 占据 successor 原来的位置
  - node 会变成“至多只有一个右孩子”的节点
  - 后续直接调用 map_delete_node(node) 即可完成真正删除

  注意：
  颜色也要交换。
  因为颜色属于“树位置”的红黑性质，而不是 key 本身。
*/
  void swap_node_with_successor(Node* node, Node* successor){
    Node* node_parent = node->parent;
    Node* node_left = node->left;
    Node* node_right = node->right;

    Node* succ_parent = successor->parent;
    Node* succ_right = successor->right;

    /* 颜色跟着位置走，所以交换颜色 */
    size_t temp_color = node->color;
    node->color = successor->color;
    successor->color = temp_color;

    /*
      情况 1：
      successor 就是 node->right
    */
    if(succ_parent == node){
      /* successor 接到 node 原来的父亲处 */
      successor->parent = node_parent;

      if(node_parent == nullptr){
        root_node = successor;
      }else if(node_parent->left == node){
        node_parent->left = successor;
      }else{
        node_parent->right = successor;
      }

      /* successor 占据 node 原位置 */
      successor->left = node_left;
      if(node_left != nullptr){
        node_left->parent = successor;
      }

      successor->right = node;
      node->parent = successor;

      /*
        node 移到 successor 原位置。
        successor 作为直接右孩子，本来没有 left；
        node 也必须没有 left。
      */
      node->left = nullptr;
      node->right = succ_right;
      if(succ_right != nullptr){
        succ_right->parent = node;
      }
    }
    /*
      情况 2：
      successor 在 node->right 的更深处
    */
    else{
      /*
        先让 node 占据 successor 原来的位置
        successor 是直接后继，因此它不会有 left 子树。
      */
      if(succ_parent->left == successor){
        succ_parent->left = node;
      }else{
        succ_parent->right = node;
      }

      node->parent = succ_parent;
      node->left = nullptr;
      node->right = succ_right;
      if(succ_right != nullptr){
        succ_right->parent = node;
      }

      /* 再让 successor 占据 node 原来的位置 */
      successor->parent = node_parent;

      if(node_parent == nullptr){
        root_node = successor;
      }else if(node_parent->left == node){
        node_parent->left = successor;
      }else{
        node_parent->right = successor;
      }

      successor->left = node_left;
      if(node_left != nullptr){
        node_left->parent = successor;
      }

      successor->right = node_right;
      if(node_right != nullptr){
        node_right->parent = successor;
      }
    }
  }

  /*辅助函数，删除某个Node*/
  void map_delete_node(Node* node){
    if(node == nullptr) return;
    size_t child_count = 0;
    if(node->right != nullptr) child_count++;
    if(node->left != nullptr) child_count++;

    if(node == root_node){
      /*根节点特区*/
      if(child_count == 0){
        delete root_node;
        root_node = nullptr;
        current_size --;
        return;
      }
      if(child_count == 1){
        Node* son = nullptr;
        if(node->left != nullptr) son = node->left;
        else{
          son = node->right;
        }
        delete root_node;
        son->color = 1;
        son->parent = nullptr;
        current_size --;
        root_node = son;
        return;
      }
        /*什么都不做，自己掉到下面 child_count = 2去*/
    }
    if(child_count == 2){
      /*找直接后继*/
      Node* successor = node->right;
      while(successor->left != nullptr){
        successor = successor->left;
      }
      swap_node_with_successor(node,successor);
      map_delete_node(node);
    }else if(child_count == 1){
      /*看看是左还是右侧*/
      size_t left_right = 0;
      if(node->right != nullptr) left_right = 1;
      if(left_right){
        /*右侧*/
        Node* son = node->right;
        son->parent = node->parent;
        son->color = 1; // 变色
        if(node->parent->left == node){
          node->parent->left = son;
        }else{
          node->parent->right = son;
        }

        delete node;
        this->current_size --;
      }else{
        /*左侧*/
        Node* son = node->left;
        son->parent = node->parent;
        son->color = 1; //变色
        if(node->parent->left == node){
          node->parent->left = son;
        }else{
          node->parent->right = son;
        }
        delete node;
        this->current_size --;
      }
      /*OK!*/
    }else{
      /* child_count == 0 */
      if(node->color == 0){
        /* 如果是红色 */
        Node* father = node->parent;
        if(father->left == node) father->left = nullptr;
        else{
          father->right = nullptr;
        }
        delete node;
        this->current_size --;
        return;
      }else{
        /* 如果是黑色，要变成双黑了，去处理吧*/
        node->color = 2;
        /* 处理双黑,然后删除 */
        adjust_delete_node(node);

        if(node->parent != nullptr){
          if(node->parent->left == node) node->parent->left = nullptr;
          else{
            node->parent->right = nullptr;
          }
        }
        delete node;
        this->current_size --;
      }
    }
  }

  /*辅助函数，调整节点,专门用来调整双黑*/
  void adjust_delete_node(Node* node){
    /*碰到根节点或者红色节点*/
    if(node == root_node || node->color == 0){
      node->color = 1;
      /*消除双黑*/
      return;
    }
    Node* father = node->parent;
    Node* brother = nullptr;

    if(father->left == node) brother = father->right;
    else{
      brother = father->left;
    }
    /*处理brother*/
    if(brother == nullptr){
      if(father->color == 0){
        father->color = 1;
        return;
      }else{
        father->color = 2;
        adjust_delete_node(father);
        return;
      }
    }
    if(brother ->color == 1){
      bool all_black = true;
      
      Node* bro_left = brother->left;
      Node* bro_right = brother->right;
      /*都是黑色为false*/
      if(bro_left != nullptr && bro_left->color == 0){
        all_black = false;
      }
      if(bro_right != nullptr && bro_right -> color == 0){
        all_black = false;
      }

      if(all_black){
        brother->color = 0 ; //兄弟染红
        if(father->color == 0){
          /*父亲是红色*/
          father->color = 1;
          return;
        }else{
          /*父亲是黑色*/
          father->color = 2;
          adjust_delete_node(father);
          return;
        }
        return;
      }else{
        /*有红色*/
        if(father->left == brother && bro_left != nullptr &&bro_left->color == 0){
          //LL型
          del_LL(father);
          return;
        }
        else if(father->left == brother&& bro_right != nullptr && bro_right->color == 0){
          //LR
          del_LR(father);
          return;
        }
        else if(father->right == brother&& bro_left != nullptr && bro_left->color == 0){
          //RL
          del_RL(father);
          return;
        }
        else if(father->right == brother && bro_right != nullptr && bro_right->color == 0){
          //RR
          del_RR(father);
          return;
        }
      }
    }else{
      /*兄弟是红色*/
      //定位双黑方向
      size_t left_right = 0;
      /*双黑在右侧*/
      if(father->left == brother){
        left_right = 1;
      }
      size_t father_color = father->color;
      size_t brother_color = brother->color;
      /*换色*/
      brother->color = father_color;
      father->color = brother_color;
      if(left_right == 0){
        /*双黑在左侧*/
        
        Node* grandpa = father->parent;
        /*这个变量此时被重用，用来表示father在grandpa的哪边*/
        size_t new_left_right = 0;
        if(grandpa != nullptr && grandpa->right == father) new_left_right = 1;
        /*父亲左旋*/
        father->right = brother->left;
        if(brother->left != nullptr) brother->left->parent = father;
        brother->left = father;
        father->parent = brother;
        brother->parent = grandpa;
        if(grandpa != nullptr){
          if(new_left_right){
            grandpa->right = brother;
          }else{
            grandpa->left = brother;
          }
        }else{
          /*father原本是根*/
          this->root_node = brother;
        }
      }else{
        /*双黑在右侧*/
        Node* grandpa = father->parent;
        size_t new_left_right = 0;
        if(grandpa != nullptr && grandpa->right == father) new_left_right = 1;

        father->left = brother->right;
        if(brother->right != nullptr) brother->right->parent = father;
        brother->right = father;
        father->parent = brother;
        brother->parent = grandpa;
        if(grandpa != nullptr){
          if(new_left_right){
            grandpa->right = brother;
          }else{
            grandpa->left = brother;
          }
        }else{
          /*father原本是根*/
          this->root_node = brother;
        }
      }
      /*旋转处理完了*/
      adjust_delete_node(node);
    }
  }

  /*注意，这个地方不用修改爷爷指针，我已经嵌套在内部了*/
  void del_LL(Node* father){
    Node* grandpa = father->parent;
    size_t left_right = 0;
    if(grandpa!=nullptr && grandpa->right == father) left_right = 1;
    Node* brother = father->left;
    Node* brother_son = brother->left;

    brother_son->color = brother->color;
    brother->color = father->color;
    father->color = 1; //变成黑色

    //LL型，右旋
    father->left = brother->right;
    brother->right = father;

    if(father->left != nullptr) father->left->parent = father;

    brother->parent = grandpa;
    father->parent = brother;
    if(grandpa != nullptr){
      if(left_right){
            grandpa->right = brother;
          }else{
            grandpa->left = brother;
          }
    }else{
      root_node = brother;
    }
  }
  void del_RR(Node* father){
    Node* grandpa = father->parent;
    size_t left_right = 0;
    if(grandpa!=nullptr && grandpa->right == father) left_right = 1;
    Node* brother = father->right;
    Node* brother_son = brother->right;

    brother_son->color = brother->color;
    brother->color = father->color;
    father->color = 1; //变成黑色

    //RR 左旋
    father->right = brother->left;
    brother->left = father;

    if(father->right != nullptr) father->right->parent = father;

    brother->parent = grandpa;
    father->parent = brother;

    if(grandpa != nullptr){
      if(left_right){
            grandpa->right = brother;
          }else{
            grandpa->left = brother;
          }
    }else{
      root_node = brother;
    }
  }
  void del_RL(Node* father){
    Node* grandpa = father->parent;
    size_t left_right = 0;
    if(grandpa!=nullptr && grandpa->right == father) left_right = 1;
    Node* brother = father->right;
    Node* brother_son = brother->left;

    /*RL 变色*/
    brother_son ->color = father->color;
    father->color = 1;

    /*对bro右旋*/
    brother->left = brother_son->right;
    brother_son->right = brother;

    if(brother->left != nullptr) brother->left->parent = brother;

    brother->parent = brother_son;
    brother_son->parent = father;
    father->right = brother_son;

    /*再左旋*/
    father->right = brother_son->left;
    brother_son->left = father;

    if(father->right != nullptr) father->right->parent = father;

    brother_son->parent = grandpa;
    father->parent = brother_son;

    if(grandpa != nullptr){
      if(left_right){
            grandpa->right = brother_son;
          }else{
            grandpa->left = brother_son;
          }
    }else{
      root_node = brother_son;
    }
  }
  void del_LR(Node* father){
    Node* grandpa = father->parent;
    size_t left_right = 0;
    if(grandpa!=nullptr && grandpa->right == father) left_right = 1;
    Node* brother = father->left;
    Node* brother_son = brother->right;

    /*LR 变色*/
    brother_son->color = father->color;
    father->color = 1; 

    /*对bro左旋*/
    brother->right = brother_son->left;
    brother_son->left = brother;

    if(brother->right != nullptr) brother->right->parent = brother;

    brother->parent = brother_son;
    brother_son->parent = father;
    father->left = brother_son;

    /*再右旋*/
    father->left = brother_son->right;
    brother_son->right = father;

    if(father->left != nullptr) father->left->parent = father;

    brother_son->parent = grandpa;
    father->parent = brother_son;

    if(grandpa != nullptr){
      if(left_right){
            grandpa->right = brother_son;
          }else{
            grandpa->left = brother_son;
          }
    }else{
      root_node = brother_son;
    }
  }

  size_t debug_height(Node* node){
    if(node == nullptr) return 0;
    size_t l = debug_height(node->left);
    size_t r = debug_height(node->right);
    return (l > r ? l : r) + 1;
  }


};
};
#endif
