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

        Node(const Node& other):value(other.value), child(nullptr), sibling(nullptr){
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
        if(other.root_head == nullptr){
            this->root_head = nullptr;
            this->max_root = nullptr;
            current_size = 0;
            return;
        }
        /*这个地方如果 * nullptr会爆炸，得拦截*/
        this->root_head = new Node(*other.root_head);
        /*max root 再遍历一次找吧*/
        Node* ptr = root_head;
        this->max_root = nullptr;
        while(ptr != nullptr){
            if(this->max_root == nullptr) max_root = ptr;
            else{
                /*a < b*/
                max_root = cmp(ptr->value , max_root->value)? max_root:ptr; 
            }
            ptr = ptr->sibling;
        }
    };
    
    ~priority_queue(){
        Node::delete_node(root_head);
        max_root = nullptr;
        current_size = 0;
    };
    
    /*深拷贝赋值函数*/
    priority_queue& operator=(const priority_queue& other){
        if(this == &other) return *this; // 自赋值检查
        this->~priority_queue(); // 销毁当前对象，释放资源
        new (this) priority_queue(other); // 使用 placement new 进行复制构造
        return *this;
    };

    /** Adds one element to the queue. */
    void push(const T& val){
        /*先比较再尝试构造，这样连New的内存都不用担心了*/
        bool greater_judge = (max_root == nullptr)||(cmp(max_root->value , val));

        Node* new_node = new Node(val);
         
        /* 头插入 */
        if(root_head == nullptr) root_head = new_node;
        else{
            new_node->sibling = root_head;
            root_head = new_node;
        }
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
        if(this->current_size == 0) throw container_is_empty();

        /*
         * pop 的难点在于：
         * 原来的写法会先摘根、接 children，再在 consolidate() 中调用 cmp。
         * 如果 cmp 在 consolidate 中抛异常，堆已经被改坏，无法满足强异常安全。
         *
         * 这里改成“两阶段提交”：
         * 1. planning 阶段：只读取真实节点，不改任何指针/degree；
         *    在虚拟 degree 桶中模拟 consolidate，并完成所有可能抛异常的 cmp。
         * 2. commit 阶段：planning 成功后，不再调用 cmp，
         *    按记录好的 link 计划真正改 child/sibling/degree，重建根表。
         *
         * 若 planning 阶段 cmp 抛异常，真实堆完全未修改，异常可直接原样传出。
         */

        struct LinkPlan {
            Node* parent;
            Node* child;
        };

        Node* old_max = max_root;
        const int max_size = static_cast<int>(log2(current_size)) + 3;
        Node** degree_bucket = nullptr;
        LinkPlan* link_plan = nullptr;
        int link_count = 0;
        Node* planned_max = nullptr;

        try {
            degree_bucket = new Node*[max_size]();
            /* 合并次数不会超过参与 consolidate 的树数，current_size 是安全上界。 */
            link_plan = new LinkPlan[current_size];

            /*
             * 把一棵“虚拟根树”塞进 degree 桶。
             * cur_degree 是虚拟 degree：planning 阶段不修改 Node::degree。
             */

            /* 先模拟 old_max 的孩子。真实 pop 中 children 会被接到根链表前面。 */
            for(Node* candidate = old_max->child; candidate != nullptr; candidate = candidate->sibling){
                Node* cur = candidate;
                int cur_degree = cur->degree;

                while(degree_bucket[cur_degree] != nullptr){
                    Node* target = degree_bucket[cur_degree];
                    degree_bucket[cur_degree] = nullptr;

                    if(cmp(cur->value, target->value)){
                        /* target 更大：cur 将来挂到 target 下面。 */
                        link_plan[link_count++] = LinkPlan{target, cur};
                        cur = target;
                    }else{
                        /* cur 更大或相等：target 将来挂到 cur 下面。 */
                        link_plan[link_count++] = LinkPlan{cur, target};
                    }
                    ++cur_degree;
                }
                degree_bucket[cur_degree] = cur;
            }

            /* 再模拟原根链表中除 old_max 以外的根。 */
            for(Node* candidate = root_head; candidate != nullptr; candidate = candidate->sibling){
                if(candidate == old_max) continue;

                Node* cur = candidate;
                int cur_degree = cur->degree;

                while(degree_bucket[cur_degree] != nullptr){
                    Node* target = degree_bucket[cur_degree];
                    degree_bucket[cur_degree] = nullptr;

                    if(cmp(cur->value, target->value)){
                        link_plan[link_count++] = LinkPlan{target, cur};
                        cur = target;
                    }else{
                        link_plan[link_count++] = LinkPlan{cur, target};
                    }
                    ++cur_degree;
                }
                degree_bucket[cur_degree] = cur;
            }

            /* planning 阶段顺便算出 pop 后的新 max_root。 */
            for(int i = 0 ; i < max_size ; ++i){
                if(degree_bucket[i] != nullptr){
                    if(planned_max == nullptr || cmp(planned_max->value, degree_bucket[i]->value)){
                        planned_max = degree_bucket[i];
                    }
                }
            }
        }
        catch(...){
            delete[] degree_bucket;
            delete[] link_plan;
            throw;
        }

        /* -------------------- commit：从这里开始不再调用 cmp -------------------- */

        /* 先断开原根链表中的 sibling 关系。 */
        for(Node* ptr = root_head; ptr != nullptr; ){
            Node* next = ptr->sibling;
            ptr->sibling = nullptr;
            ptr = next;
        }

        /* 再断开 old_max 的孩子链表；这些孩子将作为独立树参与重建。 */
        for(Node* ptr = old_max->child; ptr != nullptr; ){
            Node* next = ptr->sibling;
            ptr->sibling = nullptr;
            ptr = next;
        }

        /* 按 planning 阶段记录的顺序真正执行 link。 */
        for(int i = 0 ; i < link_count ; ++i){
            Node* parent = link_plan[i].parent;
            Node* child = link_plan[i].child;
            child->sibling = parent->child;
            parent->child = child;
            parent->degree++;
        }

        /* 用最终 degree 桶重建根链表。 */
        root_head = nullptr;
        Node* tail = nullptr;
        for(int i = 0 ; i < max_size ; ++i){
            if(degree_bucket[i] != nullptr){
                if(root_head == nullptr){
                    root_head = degree_bucket[i];
                    tail = degree_bucket[i];
                }else{
                    tail->sibling = degree_bucket[i];
                    tail = degree_bucket[i];
                }
            }
        }
        if(tail != nullptr) tail->sibling = nullptr;

        max_root = planned_max;
        old_max->child = nullptr;
        old_max->sibling = nullptr;
        delete old_max;
        --current_size;

        delete[] degree_bucket;
        delete[] link_plan;
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
        Node::delete_node(root_head);
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
        if(this == &other) return;
        if(other.size() == 0) return;
        if(this->size() == 0){
            this->root_head = other.root_head;
            this->current_size = other.current_size;
            this->max_root = other.max_root;
            other.root_head = nullptr;
            other.current_size = 0;
            other.max_root = nullptr;
            return;
        }
        /*惰性二项堆的merge只拼根链表*/
        /*先判断 新的 max是谁*/
        /*用false表示自己的小、true表示用对方的吧*/
        bool max_judge = false;
        /*自己的为空、 或者自己的不空、对方的也不空，自己小，就用对方的，如果俩都空也用对方的*/
        if(this->max_root == nullptr || (other.max_root != nullptr && cmp(this->max_root->value , other.max_root->value))) max_judge = true;
        /*把对方的加入链表*/
        Node* ptr = root_head;
        if(ptr == nullptr){
            root_head = other.root_head;
        }else{
            while(ptr->sibling != nullptr){
                ptr = ptr->sibling;
            }
            /*走到尾巴*/
            /*把对方的下一个直接挂上去*/
            ptr->sibling = other.root_head;
        }
        /*防止误删新的*/
        other.root_head = nullptr;
        /*接管*/
        this->max_root = max_judge? other.max_root:this->max_root;
        this->current_size += other.size();
        other.clear();
        return;
    };

    /*把 B 挂在 A 下面*/
    void consolidate(){
        /*懒得算了，直接构造一个巨大无比的哈希表，用这个来撞*/
        if(root_head == nullptr || current_size == 0){
            max_root = nullptr ;
            return;
        }
        int max_size = log2(current_size)+2;
        Node** degree = new Node*[max_size]();

        /*开始遍历根链表*/
        Node* ptr = root_head;
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
                    degree[ptr->degree] = nullptr;
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
        root_head = nullptr;
        Node* tail = nullptr;
        /*最大值也得改了*/
        this->max_root = nullptr;
        for(int i = 0 ; i < max_size ; i++){
            if(degree[i] != nullptr){
                if(this->max_root == nullptr || cmp(this->max_root->value , degree[i]->value) ) max_root = degree[i];
                if(root_head == nullptr){
                    root_head = degree[i];
                    tail = degree[i];
                    continue;
                }
                tail->sibling = degree[i];
                tail = tail->sibling;
            }
        }
        if(tail != nullptr) tail->sibling = nullptr;
        delete[] degree;
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