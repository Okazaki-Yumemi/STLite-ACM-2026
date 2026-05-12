#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cmath>       // in case you need it
#include <cstddef>     // for size_t
#include <functional>  // for std::less

#include "exceptions.hpp"

namespace sjtu {

/**
 * @brief A container automatically sorting its contents, similar to
 * std::priority_queue but with extra functionalities.
 *
 * The extra functionalities are:
 * - Merge two priority queues into one (with good time complexity).
 * - Clear all elements in the queue.
 * - Limited exception safety for some operations (e.g. push, pop, top, merge)
 * when the comparator throws exceptions from `Compare` only.
 *
 * This @priority_queue does not support passing an underlying container as a template parameter.
 * Also, it does not support passing a comparator object as a constructor argument.
 *
 */
template <class T, class Compare = std::less<T>>
class priority_queue {
   public:

   /* 内置节点类 */
    class Node
    {
    public:

        T value;
        int degree;
        Node* child;
        Node* sibling;

        Node(T val){
            value = val;
            degree = 0;
            child = nullptr;
            sibling = nullptr;
        };

        ~Node(){
            delete value;
            degree = 0;
            child = nullptr;
            sibling = nullptr;
        };

    };
    
    /* 优先队列的参数 */
    Node* root_head;
    Node* max_root;
    size_t current_size;
    Compare cmp;

    priority_queue(){
        root_head = New Node();
        max_root = nullptr;
        current_size = 0;
    };
    priority_queue(const priority_queue&);
    ~priority_queue(){
        delete root_head;
        max_root = nullptr;
        current_size = 0;
    };

    priority_queue& operator=(const priority_queue&);

    /** Adds one element to the queue. */
    void push(const T& val){
        Node* new_node = new Node(val);
        
        /* 头插入 */
        new_node->sibling = root_head ->sibling;
        root_head ->sibling = new_node;

        /* 更新指针 */
        if(max_root == nullptr) max_root = new_node;
        else if (cmp(max_root->value , new_node->value) == true) max_root = new_node;
        
        /* 更新大小 */
        current_size++;

    };

    /**
     * Returns a read-only reference of the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    const T& top() const;

    /**
     * Removes the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    void pop();

    /** Returns the number of elements in the queue. */
    size_t size() const{
        return current_size;
    };

    /** Returns whether there is any element in the queue. */
    bool empty() const{
        return current_size == 0;
    };

    /** Clears all elements in the queue. */
    void clear();

    /**
     * @brief Merges two priority queues into one.
     *
     * The merged data shall be stored in the current priority queue and the
     * other priority queue shall be cleared after merging.
     *
     * The time complexity shall be O(log n) or better.
     */
    void merge(priority_queue&);

    /*把 B 挂在 A 下面*/
    void consolidate(){
        /*懒得算了，直接构造一个巨大无比的哈希表，用这个来撞*/
        int max_size = cmath::log2(current_size)+2;
        Node** degree = new Node*[max_size]();

        /*开始遍历根链表*/
        Node* ptr = root_head->sibling;
        while(ptr != nullptr){
            /*不需要合并*/
            if(degree[ptr->degree] == nullptr){
                /*放进去*/
                degree[ptr->degree] = ptr;
            }else{
                /* 拿出来 */
                Node* target = degree[ptr->degree];
                degree[ptr->degree] = nullptr;
                /*在这个地方调用一个新的函数，返回一个新的节点，让ptr直接等于新的节点，继续检查*/
                ptr = merge_singe(ptr,target);
                /*如果发生了合并，不需要走到下一个，仍然检查自己*/
                continue;
            }
            /*前进一格*/
            ptr = ptr->next;
        }
        /* 重构 */
        ptr = root_head;
        for(int i = 0 ; i < max_size ; i++){
            if(degree[i] != nullptr){
                ptr->sibling = degree[i];
                ptr = ptr->sibling;
            }
        }

    };

    Node* merge_single(Node* src , Node* target){
        /*
        目的是把target挂在src下面
        最后返回一个新的src。
        不能改变src的指向
        */
        target->sibling = src->child;
        src -> child = target;
        return src;
    }



};

}  // namespace sjtu

#endif