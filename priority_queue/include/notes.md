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
