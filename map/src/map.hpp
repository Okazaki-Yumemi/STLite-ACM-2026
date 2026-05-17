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

    /*Node的复制构造函数*/
    Node(Node* other){
      /*我们要专门写一个克隆树的，这个地方复制构造我们就只构造自己就可以了*/
      this->parent = nullptr;
      this->left = nullptr;
      this->right = nullptr;
      this->parent = nullptr;
      this->value = other->value;
      this->color = other->color;
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
      iterator new_iterator;
      Node* current_node = this->node;

      new_iterator->owner = this->owner;
      new_iterator->node = this->node;

      if(current_node->right != nullptr){
        this->node = current_node->right;
      }else{
        this->node = current_node->parent;
      }
      return new_iterator;
    }

    /**
     * TODO ++iter
     */
    iterator &operator++() {
      Node* current_node = this->node;
      if(current_node->right != nullptr){
        this->node = current_node->right;
      }else{
        this->node = current_node->parent;
      }
      return *this;
    }

    /**
     * TODO iter--
     */
    iterator operator--(int) {
      iterator new_iterator;
      Node* current_node = this->node;

      new_iterator->owner = this->owner;
      new_iterator->node = this->node;

      this->node = current_node->left;
      
      return new_iterator;
    }

    /**
     * TODO --iter
     */
    iterator &operator--() {
      this->node = this->node->left;
      return *this;
    }

    
    value_type &operator*() const {
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
      return this->node->value;
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
   public:
    const map* owner;
    const Node* node;
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
      const_iterator new_iterator;
      Node* current_node = this->node;

      new_iterator->owner = this->owner;
      new_iterator->node = this->node;

      if(current_node->right != nullptr){
        this->node = current_node->right;
      }else{
        this->node = current_node->parent;
      }
      return new_iterator;
    }

    const_iterator &operator++() {
      Node* current_node = this->node;
      if(current_node->right != nullptr){
        this->node = current_node->right;
      }else{
        this->node = current_node->parent;
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator new_iterator;
      Node* current_node = this->node;

      new_iterator->owner = this->owner;
      new_iterator->node = this->node;

      this->node = current_node->left;
      
      return new_iterator;
    }

    const_iterator &operator--() {
      this->node = this->node->left;
      return *this;
    }

    value_type &operator*() const {
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
      return this->node->value;
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

  Node* clone_tree(const Node& other){
    if(other = nullptr) return nullptr;
    Node* this_one = new Node(other);

    Node* left = clone(other->left);

    Node* right = clone(other->left);

    this_one->left = left;
    left->parent = this_one;
    this_one->right = right;
    right->parent = this_one;

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
    }
  }

  /*辅助函数，克隆子树*/
  void clone_subtree(Node* old , Node* parent){
    if(old == nullptr) return;
    else{
      /*old < parent 左侧*/
      if(cmp(old->value.first , parent->value.first)){
        parent->left = new Node(old);
        clone_subtree(old->left , parent->left);
        clone_subtree(old->right, parent->left);
      }else{
        /*parent < old*/
        parent -> right = new Node(old);
        clone_subtree(old->left , parent->right);
        clone_subtree(old->right, parent->right);
      }
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

  bool equal_key(const Key & key , Node* node){
    return (!cmp(node->value.first , key) && !cmp(key,node->value.first));
  }


  /*写一个统一的Find node*/
  Node* find_node(const Key &key){
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
    Node* search_node = root_node;
    /*这个地方涉及到插入,我们先不要处理*/
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

    it->node = search_node;
    it->owner = this;
    return it;
  }

  const_iterator cbegin() const {
    const_iterator it;
    Node* search_node = root_node;
    while(search_node!= nullptr && search_node->left != nullptr){
      search_node = search_node->left;
    }

    it->node = search_node;
    it->owner = this;
    return it;
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    iterator it;
    Node* search_node = root_node;
    while(search_node!= nullptr && search_node->right != nullptr){
      search_node = search_node->right;
    }

    it->node = search_node;
    it->owner = this;
    return it;
  }

  const_iterator cend() const {
    const_iterator it;
    Node* search_node = root_node;
    while(search_node!= nullptr && search_node->right != nullptr){
      search_node = search_node->right;
    }

    it->node = search_node;
    it->owner = this;
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
    delete(root_node);
    root_node = nullptr;
    this->current_size == 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    iterator it;
    bool judge = insert(value);
    Node* target_node = find_node(value.first);
    it->node = target_node;
    it->owner = this;

    return pair<iterator,bool>(it , judge);
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    Node* node = pos->node;
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
    Node* search_node = find_node(key);
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
    it->node = target_node;
    it->owner = this;
    return it;
  }

  const_iterator find(const Key &key) const {
    Node* target_node = find_node(key);
    const_iterator it;
    it->node = target_node;
    it->owner = this;
    return it;
  }

  /*赋值函数，根据val插入,自动完成插入调整*/
  bool insert(const value_type& val){
    Node* search_node = find_node(val.first);
    /*非空，替换*/
    if(search_node != nullptr){
      search_node->value = val;
      return false; // 发生了替换
    }else{
      /*这个地方得从他父亲去找*/
      search_node = root_node;
      Node* parent = nullptr;
      while(search_node != nullptr){
        /*key大*/
        if(cmp(search_node->value.first , key)) {
          parent = search_node;
          search_node = search_node->right;
        }
        else{
          parent = search_node;
          search_node = search_node->left;
        }
      }
      /*这个地方search_node必定落在nullptr*/
      /*可以创建了*/
      search_node = new Node(val,parent); // 默认红色
      /*然后看大小 search < parent  */
      if(cmp(search_node->value.first , parent->value.first)){
        parent->left = search_node;
      }else{
        parent->right = search_node;
      }
      /*可以开始调整*/
      insert_adjust(search_node);
      /*ok!*/
      return true; //空插入
    }
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
      if(grandpa->left == fahter) uncle = grandpa->right
      else{
        uncle = grandpa->left;
      }
      /*看叔叔什么情况了*/
      if(uncle->color == 0){
        /*叔叔是红色*/
        /*颜色取反*/
        grandpa->color = (grandpa->color == 1)? 0:1;
        uncle->color = (uncle->color == 1)? 0:1;
        fahter->color = (fahter->color == 1)? 0:1;
        /*递归处理爷爷*/
        adjust(grandpa);
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
            this->root_node = this->RR(grandpa);
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
            this->root_node = this->RR(grandpa);
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

    father->parent = son;
    son->parent = grandpa;
    grandpa->right = son;

    return RR(grandpa);
  }

  /*辅助函数，删除某个Node*/
  void map_delete_node(Node* node){
    size_t child_count = 0;
    if(node->right != nullptr) child_count++;
    if(node->left != nullptr) child_count++;
    if(child_count == 2){
      /*找直接后继*/
      Node* search_node = node->right;
      while(search_node->left != nullptr){
        search_node = search_node->left;
      }
      /*search_node 一直左移，走到最下面*/
      /*直接后继代替，删除后续节点*/
      node->value.~pair();
      node->value = new pair<const Key, T>(search_node->value.first,search_node->value.second);
      /*递归删除直接后继*/
      map_delete_node(search_node);
    }else if(child_count == 1){
      /*看看是左还是右侧*/
      size_t left_right = 0;
      if(node->right != nullptr) left_right = 1;
      if(left_right){
        /*右侧*/
        Node* son = node->right;
        son->parent = node->parent;
        if(node->parent->left == node){
          node->parent->left = son;
        }else{
          node->parent->right = son;
        }
        delete node;
      }else{
        /*左侧*/
        Node* son = node->left;
        son->parent = node->parent;
        if(node->parent->left == node){
          node->parent->left = son;
        }else{
          node->parent->right = son;
        }
        delete node;
      }
      /*OK!*/
    }else{
      /* child_count == 0 */
      if(node->color == 0){
        /* 如果是红色 */
        delete node;
      }else{
        /* 如果是黑色，要变成双黑了，去处理吧*/
        node->color == 2;
        /* 处理双黑,然后删除 */
        adjust_delete_node(node);

        if(node->parent->left == node) node->parent->left = nullptr;
        else{
          node->parent->right = nullptr;
        }
        delete node;
      }
    }

  }

  /*辅助函数，调整节点,专门用来调整双黑*/
  void adjust_delete_node(Node* node){
    /*碰到根节点或者红色节点*/
    if(node == root_node || node->color == 0){
      node->color = 1;
      /*消除双黑*/
    }
    Node* father = node->parent;
    Node* brother = nullptr;
    /*我们刚刚已经断开了从父亲到node的路径*/
    if(father->left == node) brother = father->right;
    else{
      brother = father->left;
    }
    /*根据红黑树的性质，brother不可能是nullptr*/
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
        father->color = 2; // 双黑上移
        adjust_delete_node(father); //父亲处理双黑
        return;
      }else{
        /*有红色*/
        if(father->left == brother && bro_left->color == 0){
          //LL型
          del_LL(father);
          return;
        }
        else if(father->left == brother && bro_right->color == 0){
          //LR
          del_LR(father);
          return;
        }
        else if(father->right == brother && bro_left->color == 0){
          //RL
          del_RL(father);
          return;
        }
        else if(father->right == brother && bro_right->color == 0){
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
        brother->left = father;
        father->parent = brother;
        brother->parent = grandpa;
        if(grandpa != nullptr){
          (new_left_right)? grandpa->right:grandpa->left = brother;
        }
      }else{
        /*双黑在右侧*/
        Node* grandpa = father->parent;
        size_t new_left_right = 0;
        if(grandpa != nullptr && grandpa->right == father) new_left_right = 1;

        father->left = brother->right;
        brother->right = father;
        father->parent = brother;
        brother->parent = grandpa;
        if(grandpa != nullptr){
          (new_left_right)? grandpa->right:grandpa->left = brother;
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

    brother->parent = grandpa;
    father->parent = brother;
    if(grandpa != nullptr){
      (left_right)? grandpa->right:grandpa->left = father;
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

    brother->parent = grandpa;
    father->parent = brother;

    if(grandpa != nullptr){
      (left_right)? grandpa->right:grandpa->left = father;
    }
  }
  void del_RL(Node* father){
    Node* grandpa = father->parent;
    size_t left_right = 0;
    if(grandpa!=nullptr && grandpa->right == father) left_right = 1;
    Node* brother = father->right;
    Node* brother_son = brother->right;

    /*RL 变色*/
    brother_son ->color = father->color;
    father->color = 1;

    /*对bro右旋*/
    brother->left = brother_son->right;
    brother_son->right = brother;

    brother->parent = brother_son;
    brother_son->parent = father;
    father->left = brother_son;

    /*再左旋*/
    father->right = brother->left;
    brother->left = father;

    brother->parent = grandpa;
    father->parent = brother;

    if(grandpa != nullptr){
      (left_right)? grandpa->right:grandpa->left = father;
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

    brother->parent = brother_son;
    brother_son->parent = father;
    father->left = brother_son;

    /*再右旋*/
    father->left = brother->right;
    brother->right = father;

    brother->parent = grandpa;
    father->parent = brother;

    if(grandpa != nullptr){
      (left_right)? grandpa->right:grandpa->left = father;
    }
  }
  
};
}
#endif
