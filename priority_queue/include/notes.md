# 我们采用惰性二项堆的方法
## 惰性二项堆
惰性二项堆是一种数据结构，它是二项堆的一种变体，主要用于实现优先队列。

它维护一串 "二项树" 的根节点，push 和 merge 懒得整理，直接把树挂到根链表里面。

真正需要堆顶或者删除堆顶的时候，再把同阶树合并。

## 什么是二项树
B0: 1 个节点
B_k+1 : 由两个 B_k 连接而成的树，其中一颗 B_k 的根节点作为另一颗 B_k 的子节点。

## 最大堆版本的二项树
父节点优先级不低于子节点。

## 为什么适合merge
一个普通的二项堆会维持每个 degree 最多只有一棵树，类似二进制
合并两个堆本质就是二进制加法，同Degree的树进位合并

## 什么是 “惰性”

``` push ``` 和 ``` merge ``` 操作不进行合并，直接把树挂到根链表里面。

``` top / pop ``` 的时候才进行合并。

## priority_queue 怎么设计

### Node 节点
```cpp
T value;
Node* child; // 指向第一个子节点
Node* sibling; // 指向下一个兄弟节点
int degree; // 以该节点为根的树的度数
```
孩子用单链表管理。
每次合并的时候，把较小根作为较大根的一个新孩子，挂到child 链表最前面即可。

### PriorityQueue 本体
```cpp
Node* roots; // 根链表
size_t size; // 元素数量
Compare cmp; // 比较器
```

## 辅助函数

### link两棵同阶树
输入degree相同的两棵树，返回合并后的树

1.比较 a->value 和 b->value，确定哪个是较小的根
2.最大的当根，另一个当孩子，挂到child链表最前面
3.小的挂到大的child链表最前面
4.返回新的根节点

### push
创建新的单节点树，挂到根链表最前面，size++

### merge
把另一个堆的根链表直接接到当前堆的根链表最前面，size += other.size

### top

因为惰性二项堆的根链表里面可能有很多树，堆顶一定在这些根里面。所以我们需要遍历根链表，找到优先级最高的根节点。

为了保证复杂度，top前需要先consolidate，合并同阶树，保证每个degree最多只有一棵树。

或者我们维护 ```Node* max_root``` 指向当前堆顶，每次push和merge都更新它。

### consolidate

遍历根链表，每看到一棵树cur ,如果 degree[cur]是空的，就把cur放到 degree[cur->degree] 里，如果 degree[cur->degree] 里已经有树了，就把它们合并成一棵更大的树，继续检查新的树的 degree，直到没有冲突为止。

degree怎么写? 不能用vector
可以用一个固定大小的数组，假设堆里最多有 N 个元素，那么 degree 最多是 log2(N)，所以我们可以定义一个足够大的数组来存储不同 degree 的树。
degree的类型是 Node** degree[MAX_DEGREE]; // MAX_DEGREE 可以是一个常数，比如 64，足够大以容纳所有可能的 degree

### pop
1.如果堆空，抛出异常
2.consolidate
3.找到堆顶节点 max_root
4.从根链表中摘掉 max_root
5.max_root的子树本就是二项树，把这些孩子重新加入根链表
6.删除 max_root，size--
7.consolidate，更新 max_root

### 拷贝构造/析构/clear

clear / destructor 需要递归删除所有节点。从每个root触发，递归删除它的子树。

### 异常安全

如果push,pop,top,merge 过程中发生异常，优先队列状态必须回滚到操作前的状态。，merge中两个堆的状态必须保持不变。


# 开始实操

## 注意事项

Merge 之后要清空 other

## 前期准备 & 思考

根节点怎么存？

本体维护root_head, max_root size_t current_size Compare cmp
---
Node 结构体
value
child 第一个孩子
sibling 下一个兄弟节点  sibling 链表可以用于管理根链表，也可以管理孩子链表

```cpp
B.sibling = A.child
A.child = B
A.degree++
```

degree
---

```cpp
class Compare = std::less<T>
```
这意味着 cmp(a,b) 返回true 意味着 a < b , 所以 b在顶部。

### Node类的构造实操
没啥特别的，需要一个构造函数，接受一个值，初始化 degree=0, child=nullptr, sibling=nullptr

### 优先队列的构造
维护root_head, max_root size_t current_size Compare cmp

构造函数直接初始化这些成员变量，root_head = nullptr, max_root = nullptr, current_size = 0
析构函数直接删掉就行。

### push实操
最简单的一个， Node*new_node = new Node(value);

采用头插，new_node->sibling = root_head->sibling; root_head->sibling = new_node;

然后更新 max_root，如果 new_node 的优先级更高，就更新 max_root = new_node;

休正一下，这个地方要对函数做异常捕获处理问题
先前代码
```cpp
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
```
其中，报错点有两个:
1. new Node(val) 可能抛出异常
    这个不用担心，因为如果 new Node(val) 抛出异常，说明内存分配失败了，这时候 push 操作根本没有成功，优先队列的状态也没有改变，所以不需要回滚。
2. cmp(max_root->value , new_node->value) 可能抛出异常
    这个需要注意，如果 cmp 抛出异常，说明比较器在比较 max_root->value 和 new_node->value 时发生了错误，这时候我们需要回滚到 push 前的状态。
    回滚的操作就是把新节点从根链表中摘掉，并且如果 new_node 是 max_root，我们还需要恢复 max_root 的值。

我们直接把判断提前
```cpp
    /** Adds one element to the queue. */
    void push(const T& val){
        Node* new_node = new Node(val);
        bool greater_judge = cmp(max_root->value , new_node->value); 
        /* 头插入 */
        new_node->sibling = root_head ->sibling;
        root_head ->sibling = new_node;

        /* 更新指针 */
        if(max_root == nullptr) max_root = new_node;
        else if (greater_judge == true) max_root = new_node;
        
        /* 更新大小 */
        current_size++;

    };
```
这样如果 cmp 抛出异常，优先队列的状态没有改变，不需要回滚。

### size
返回current_size

### empty

返回 current_size == 0;

### consolidate
现在发现consolidate里面的哈希表特别难写，不让用
只能用数组
degree数组的大小应该是 log2(N)，我们可以定义一个常数 MAX_DEGREE，比如 64，足够大以容纳所有可能的 degree。

```cpp
Node* degree[MAX_DEGREE] = {nullptr}; // 初始化为 nullptr
```
但是这个地方似乎不能静态定义，因为我们需要在consolidate函数里使用它，consolidate可能被多次调用，每次调用都需要一个干净的 degree 数组。
我们要new,写法是
```cpp
Node** degree = new Node*[MAX_DEGREE](); // 初始化为 nullptr
```

#### 犯了一点错
consolidate的时候degree的参数写成 ptr->value了，应该是 ptr->degree.

### clear
clear用后续递归删除所有节点，从每个root触发，递归删除它的子树。
```cpp
大概就是先 delete ptr->child 再 delete ptr->sibling 最后 delete ptr
```

## 异常处理说明

```
惰性二项堆把 push / merge 设计得很轻：
- push 只加一个 B0；
- merge 只拼接根链表；
- 都不调用 consolidate。

原因：
1. 结构上允许延迟整理；
2. 复杂度更好；
3. 异常安全更容易，因为避开了大量 cmp。

consolidate 是高风险函数：
- 需要辅助数组；
- 需要大量 cmp；
- 会修改根表和树结构。

因此它不应出现在 merge 中。
它主要服务于 pop，因为删除最大根后必须整理森林并重建 max_root。

真正困难的是：
如果 pop 中 consolidate 的 cmp 抛异常，
如何让 pop 恢复到调用前状态。
这需要后续专门设计，不能靠“之前 cmp 没报错”来假设后续不会报错。
```


# 超级Debug环节我草泥马的
## heap corruption 头号元凶 ```delete &value```
delete ptr的时候会自动调用T value的析构函数，如果T的析构函数里又调用了 delete this->value，就会导致 double free，造成 heap corruption。

## Node复制构造没有初始化空指针
现在
```cpp
Node(const Node& other):value(other.value){
    this->degree = other.degree;
    if(other.child != NULL){
        this->child = new Node(*other.child);
    }
    if(other.sibling != NULL){
        this->sibling = new Node(*other.sibling);
    }
}
```
如果 other.child 是 nullptr，那么 this->child 就没有被初始化，可能是一个野指针，后续访问 this->child 就会导致 heap corruption。
解决方法是在构造函数里初始化 child 和 sibling 为 nullptr。
```cpp
Node(const Node& other):value(other.value), child(nullptr), sibling(nullptr){
    this->degree = other.degree;
    if(other.child != NULL){
        this->child = new Node(*other.child);
    }
    if(other.sibling != NULL){
        this->sibling = new Node(*other.sibling);
    }
}
```

## priority_queue的析构函数只删了第一个根
```cpp
~priority_queue(){
        delete_node(root_head);
        max_root = nullptr;
        current_size = 0;
    };
```
改成我们写好的delete函数就好了

## operator= 无限递归 + use-after-destruction
后面再改
```cpp
priority_queue& operator=(const priority_queue& other){
        if(this == &other) return *this; // 自赋值检查
        this->~priority_queue(); // 销毁当前对象，释放资源
        new (this) priority_queue(other); // 使用 placement new 进行复制构造
        return *this;
    }
```

## push插入逻辑有问题，改了一下，因为我们没有办法做链表哨兵节点，妈的，没有默认构造函数

## merge() 的bug


```cpp
Node* ptr = root_head;
while(ptr != nullptr){
    ptr = ptr->sibling;
}
ptr->sibling = other.root_head;
```

### ptr循环结束之后是 nullptr，所以 ptr->sibling 就会导致 segmentation fault。
改成
```cpp
Node* ptr = root_head;
if(ptr == nullptr){
    root_head = other.root_head;
}else{
    while(ptr->sibling != nullptr){
        ptr = ptr->sibling;
    }
    ptr->sibling = other.root_head;
}
```
### 空堆接管other后，没有清空other




# 复盘总结
## 二项树基础
节点和Degree以2的幂次增长，合并同阶树就像二进制加法一样，容易理解。
## 数据结构设计
Node
```cpp
T value;
Node* child; // 指向第一个子节点
Node* sibling; // 指向下一个兄弟节点
int degree; // 以该节点为根的树的度数
```
根节点之间通过 sibling 链表连接，孩子节点也通过 sibling 链表连接，degree 记录以该节点为根的树的度数。

PriorityQueue
```cpp
Node* root_head; // 根链表头指针
Node* max_root; // 指向当前堆顶的指针
size_t current_size; // 当前元素数量
Compare cmp; // 比较器
```
维护根链表，堆顶指针，元素数量和比较器。

## Node相关函数
```cpp
Node(const T& val):value(val), child(nullptr), sibling(nullptr), degree(0) {}
Node(const Node& other):value(other.value), child(nullptr), sibling(nullptr), degree(other.degree) {
    if(other.child != nullptr){
        this->child = new Node(*other.child);
    }
    if(other.sibling != nullptr){
        this->sibling = new Node(*other.sibling);
    }
}
~Node() {
    delete child; // 递归删除子树
    delete sibling; // 递归删除兄弟节点
}

static delete_node(Node* node) {
    if(node == nullptr) return;
    delete_node(node->child); // 递归删除子树
    delete_node(node->sibling); // 递归删除兄弟节点
    delete node; // 删除当前节点
}
```
Node的构造函数和复制构造函数，确保正确初始化指针，避免野
指针导致的错误。

## proiority_queue 构造和资源管理
```cpp
priority_queue(): root_head(nullptr), max_root(nullptr), current_size(0) {}
priority_queue(const priority_queue& other): root_head(nullptr), max_root(nullptr), current_size(0), cmp(other.cmp) {
    if(other.root_head != nullptr){
        root_head = new Node(*other.root_head);
        // 复制构造会递归复制整个树结构
        // 需要更新 max_root 指针
        Node* ptr = root_head;
        while(ptr != nullptr){
            if(max_root == nullptr || cmp(max_root->value, ptr->value)){
                max_root = ptr;
            }
            ptr = ptr->sibling;
        }
    }
}
~priority_queue() {
    delete_node(root_head); // 递归删除所有节点
    root_head = nullptr;
    max_root = nullptr;
    current_size = 0;
}

proiority_queue& operator=(const priority_queue& other) {
    if(this == &other) return *this; // 自赋值检查
    this->~priority_queue(); // 销毁当前对象，释放资源
    new (this) priority_queue(other); // 使用 placement new 进行复制构造
    return *this;
}
```

题目相关函数
# push
```cpp
void push(const T& val){
    Node* new_node = new Node(val);
    bool greater_judge = cmp(max_root->value , new_node->value); 
    /* 头插入 */
    new_node->sibling = root_head;
    root_head = new_node;

    /* 更新指针 */
    if(max_root == nullptr) max_root = new_node;
    else if (greater_judge == true) max_root = new_node;
    
    /* 更新大小 */
    current_size++;
};
```
push操作创建一个新的单节点树，插入到根链表头部，并更新 max_root 和 current_size。
在这里我们提前调用 cmp 来判断是否需要更新 max_root，这样如果 cmp 抛出异常，优先队列的状态没有改变，不需要回滚。

# top
```cpp
const T& top() const {
    if(empty()) throw std::runtime_error("Priority queue is empty");
    return max_root->value;
}
```

# merge
```cpp
void merge(priority_queue& other){
        if(this == &other) return; /*自合并检查*/
        if(other.size() == 0) return;
        if(this->size() == 0){ /*空堆直接接管*/
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
```
merge操作将另一个堆的根链表拼接到当前堆的根链表末尾，并更新 max_root 和 current_size。最后清空 other 堆的状态。

# pop
```cpp
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
```

## 一开始的方法
尝试整堆备份，但是导致pop复杂太高

## 最终方案 Planning + Commit
### 阶段1 Planning
只读
提前模拟pop后的consolidate过程，记录所有可能抛异常的 cmp 结果，规划好 link 计划，但不修改任何指针/degree。
如果 cmp 抛异常，堆完全未修改，异常可直接原样传出

做什么？
取出 old_max 的孩子们； 根链表里面除了old_max的所有根
用虚拟degree桶模拟合并，每次调用cmp
记录每次合并的 link 计划（parent/child），但不修改任何指针/degree
规划出 pop 后的新 max_root

### 阶段2 Commit
1. 断开旧根链表；
2. 断开 old_max 的孩子链；
3. 按 link_plan 执行真实合并；
4. 根据 degree_bucket 重建根链表；
5. 更新 max_root；
6. 删除 old_max；
7. current_size--。

# size
```cpp
return current_size;
```

# empty
```cpp
return current_size == 0;
```

# clear
```cpp
void clear(){
    Node::delete_node(root_head);
    root_head = nullptr;
    max_root = nullptr;
    current_size = 0;
};
```

# consolidate
是一个过渡的函数，最后没有用
# merge_single
同上