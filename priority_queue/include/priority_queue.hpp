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

        Node(T val):value(val){
            degree = 0;
            child = nullptr;
            sibling = nullptr;
        };

        Node(const Node& other):value(other.value){
            this->degree = other.degree;
            /*拷贝构造函数得递归调用？*/
            if(other.child != NULL){
                this->child = new Node(*other.child);
            }
            if(other.sibling != NULL){
                this->sibling = new Node(*other.sibling);
            }
        }

        ~Node(){
            delete &value;
            degree = 0;
            child = nullptr;
            sibling = nullptr;
        };

        static void delete_node(Node* ptr){
            /*递归出口*/
            if(ptr == nullptr) return;
            /*删除孩子*/
            delete_node(ptr->child);
            /*删除兄弟*/
            delete_node(ptr->sibling);
            /*删除自己*/
            delete ptr;
        };
    };
    
    /* 优先队列的参数 */
    Node* root_head;
    Node* max_root;
    size_t current_size;
    Compare cmp;

    priority_queue(){
        root_head = nullptr;
        max_root = nullptr;
        current_size = 0;
    };
    /*复制构造函数*/
    priority_queue(const priority_queue& other){
        this->current_size =  other.current_size;
        this->cmp = other.cmp;
        /*Node的构造函数已经可以自己出来一个网络，因此我们拷贝头节点就可以了*/
        this->root_head = new Node(*other.root_head);
        /*max root 再遍历一次找吧*/
        Node* ptr = root_head;
        this->max_root = nullptr;
        while(ptr->sibling != nullptr){
            ptr = ptr->sibling;
            if(this->max_root == nullptr) max_root = ptr;
            else{
                /*a < b*/
                max_root = cmp(ptr->value , max_root->value)? max_root:ptr; 
            }
        }
    };
    
    ~priority_queue(){
        delete root_head;
        max_root = nullptr;
        current_size = 0;
    };
    
    /*深拷贝赋值函数*/
    priority_queue& operator=(const priority_queue& other){
        if(this == &other) return *this;
        priority_queue* tmp = new priority_queue(other);
        this->~priority_queue();
        *this = *tmp;
        return *this ;
    };

    /** Adds one element to the queue. */
    void push(const T& val){
        /*先比较再尝试构造，这样连New的内存都不用担心了*/
        bool greater_judge = (max_root == nullptr)||(cmp(max_root->value , val));

        Node* new_node = new Node(val);
         
        /* 头插入 */
        new_node->sibling = root_head ->sibling;
        root_head ->sibling = new_node;

        /* 更新指针 */
        if (greater_judge == true) max_root = new_node;
        
        /* 更新大小 */
        current_size++;

    };

    /**
     * Returns a read-only reference of the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    const T& top() const{
        if(current_size == 0) throw container_is_empty();
        return max_root->value;
    };

    /**
     * Removes the first element in the queue.
     *
     * @throws container_is_empty when the first element does not exist.
     */
    void pop(){
        /*先用复制写法来看看*/
        if(this->current_size == 0) throw container_is_empty();

        priority_queue* backup = new priority_queue(*this);

        /*深拷贝备份了，虽然这样时间复杂度肯定过不了，不过先试试*/
        try
        {
            /*把max删掉，然后size--,对接下来用consolidate*/
            Node* del =  max_root;
            max_root = max_root->sibling;
            delete del;
            this->consolidate(); /* consolidate帮助我们管理最大值 */
            this->current_size--;
        }
        catch(const std::exception& e)
        {
            /*出问题了，还回去*/
            *this = *backup;
            throw;
        }
    };

    /** Returns the number of elements in the queue. */
    size_t size() const{
        return this->current_size;
    };

    /** Returns whether there is any element in the queue. */
    bool empty() const{
        return this->current_size == 0;
    };

    /** Clears all elements in the queue. */
    void clear(){
        /*这个地方直接在 Node类内部写好，不写在外面了，方便指针调用*/
        /*有提到过我们要保存“状态”，那我们拷贝一份？*/
        /*clear不用传递异常，从根节点开始递归删就行了*/
        Node::delete_node(root_head->sibling);
        this->current_size = 0;
        this->max_root = nullptr;
        this->root_head = nullptr;
    };

    /**
     * @brief Merges two priority queues into one.
     *
     * The merged data shall be stored in the current priority queue and the
     * other priority queue shall be cleared after merging.
     *
     * The time complexity shall be O(log n) or better.
     */
    void merge(priority_queue& other){
        if(other.size() == 0) return;
        if(this->size() == 0){
            this->root_head = other.root_head;
            this->current_size = other.current_size;
            this->max_root = other.max_root;
            return;
        }
        /*惰性二项堆的merge只拼根链表*/
        /*先判断 新的 max是谁*/
        /*用false表示自己的小、true表示用对方的吧*/
        bool max_judge = false;
        /*自己的为空、 或者自己的不空、对方的也不空，自己小，就用对方的，如果俩都空也用对方的*/
        if(this->max_root == nullptr || (other.max_root != nullptr && cmp(this->max_root->value , other.max_root->value))) max_judge = false;
        else max_judge = true;

        /*把对方的加入链表*/
        Node* ptr = root_head;
        while(ptr->sibling != nullptr){
            ptr = ptr->sibling;
        }
        /*走到尾巴*/
        /*把对方的下一个直接挂上去*/
        ptr->sibling = other.root_head->sibling;
        /*防止误删新的*/
        other.root_head->sibling = nullptr;
        /*接管*/
        this->max_root = max_judge? other.max_root:this->max_root;
        this->current_size += other.size();
        other.clear();
        return;
    };

    /*把 B 挂在 A 下面*/
    void consolidate(){
        /*懒得算了，直接构造一个巨大无比的哈希表，用这个来撞*/
        int max_size = log2(current_size)+2;
        Node** degree = new Node*[max_size]();

        /*开始遍历根链表*/
        Node* ptr = root_head->sibling;
        while(ptr != nullptr){
            /*先存储下一个*/
            Node* next_node = ptr->sibling;
            ptr->sibling = nullptr;
            /*不需要合并*/
            if(degree[ptr->degree] == nullptr){
                /*放进去*/
                /*这里要注意，我们桶里面放的节点就都是孤儿节点了，没有sibling*/
                degree[ptr->degree] = ptr;
            }else{
                /*这个地方涉及到多次合并，我们直接用循环*/
                while(degree[ptr->degree] != nullptr){
                    /*拿出来*/
                    Node* target = degree[ptr->degree];
                    /*合并*/
                    ptr = merge_single(ptr,target);
                    /*这个地方要入桶吗? 在while循环之后入桶*/
                    /*流程就是发现一个地方不是空的，就一直循环，直到找到空桶*/
                }
                degree[ptr->degree] = ptr;
            }
            /*前进一格*/
            ptr = next_node;
        }
        /* 重构 */
        ptr = root_head;
        /*最大值也得改了*/
        this->max_root = nullptr;
        for(int i = 0 ; i < max_size ; i++){
            if(degree[i] != nullptr){
                if(this->max_root == nullptr || cmp(this->max_root->value , degree[i]->value) ) max_root = degree[i];
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
       /*src和target的sibling都是孤儿*/
        if(cmp(src->value,target->value)){
            /*如果target更大*/
            src->sibling = target->child;
            target->child = src;
            target->degree++;
            return target;
       }else{
            /*src大*/
            target->sibling = src->child;
            src->child = target;
            src->degree++;
            return src;
       }
        
    }
};

}  // namespace sjtu

#endif