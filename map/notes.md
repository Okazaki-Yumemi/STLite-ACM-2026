# Map

## map的本质

实现一个 key -> value 的有序映射

底层实现通常是红黑树，按照key排序

每个节点存 pair<const Key, T> 

树满足
- BST 顺序
- 红黑树平衡约束
- 中序遍历得到有序的 key

## 这题和前三个容器不同

| 容器               | 核心难点                              |
| ---------------- | --------------------------------- |
| `vector`         | 连续内存、扩容、生命周期                      |
| `priority_queue` | 树形结构、合并、异常安全                      |
| `deque`          | 分块坐标、双端维护、iterator                |
| `map`            | **平衡 BST、旋转修复、按序 iterator、erase** |

难点

- 红黑树插入修复
- 红黑树删除修复
- iterator 的中序前驱 / 后继
- erase 时候的节点替换和 iterator语义
- operator[] 、 at 、 insert 的细节

## 采用标准CLRS 红黑树
**经典红黑树+父指针**

每个节点维护
- parent
- left 
- right
- color 0为红，1为黑，2为双黑（删除时临时状态）
- value

## 节点设计的关键问题

**有些测试没有默认构造函数**

所以不能设计出Node() 然后让里面的 value 进行默认构造，那会爆炸

节点只有在真正插入的时候再用pair构造

先考虑用nullptr表示叶子，不用带value的NIL哨兵


## iterator 的思路，比deque更 "树味"

map 的 iterator 不再是下标，而是 owner + current_node
++ it 找中序后继
    有右子树就找右子树最左
    没有右子树就沿着 parent 上去，直到当前节点是父节点的左子树
-- it 找中序前驱
    有左子树就找左子树最右
    没有左子树就沿着 parent 上去，直到当前节点是父节点的右子树


## end() 怎么表示

- A: 用nullptr
- B: 用一个特殊的哨兵节点,但是会引入哨兵节点构造问题

所以用nullptr 的话、然后用 ++max_node = nullptr \ --end() = 最大节点


## 怎么析构？

Node里面有一个pair<const Key, T> value
所以析构的时候要调用 value 的析构函数
value.~pair() 就行了


## 设计Node

value有拷贝构造函数吗？
插入的时候直接用 pair<const Key, T>(key, value) 构造就行了，不需要先构造一个空的再赋值

value有复制赋值函数吗？

我如果只有key,我怎么插入一个节点？

## 怎么构造value?
node->value = pair<const Key, T>(key, value) 就行了，直接构造
没有value的时候，用默认构造参数，这么写
node->value = pair<const Key, T>(key, T()) 就行了，直接构造

## iterator类的设计

map* owner
Node* node

```cpp
new (&(node->value)) pair<const Key, T>(search_node->value.first,search_node->value.second);
```







# 复盘
## map股骨架和对象生命周期问题

### 空树根节点设计错
```cpp
root_node = new Node(); // 这里会调用默认构造函数，导致value的构造失败
```
正确
```cpp
root_node = nullptr; // 空树没有根节点
```

### pair<const Key,T> 不能赋值

```value_type``` 是 ```pair<const Key, T>```
```cpp
node->value = pair<const Key, T>(key, value); // 这里会调用pair的赋值运算符，但pair<const Key, T>没有赋值运算符
```

所以删除双孩子系欸但的时候，不能
```cpp
node->value = successor->value; // 这里会调用pair的赋值运算符，但pair<const Key, T>没有赋值运算符
```
正确的做法是直接构造
```cpp
Node(value_type(key, value)) // 直接构造
```

需要替换的时候，只能
```cpp
node->value.~pair(); // 先析构原来的value
new (&node->value) pair<const Key, T>(successor->value.first, successor->value.second); // 直接构造新的value
```

### Node的析构和分工
一开始
```cpp
value.~pair(); // Node的析构函数里调用value的析构函数
```
以及最终只调用析构 不 delete 的倾向
- Node::~Node() 不负责递归删除子树，只负责析构当前节点的value
- delete_node(node) 负责递归删除子树并调用Node的析构函数
- delete node; 会自动调用value的析构函数，所以不需要在delete_node里手动调用value.~pair()，只需要递归删除子树并delete node就行了

## 拷贝构造和赋值
### Node(Node* other) 的问题
- parent不好设置
- value_type 不能默认构造再赋值
- 递归拷贝的语义压在Node上不太合适

#### 解决方法
写map层辅助函数
```cpp
Node* clone_tree(const Node* other){}
```
递归复制
- 当前的 value
- 当前的 color
- 左右子树
- 复制和重新维护 parent

## 插入: 空树、根更新、旋转接线
### 第一次必须特殊处理
早期在
```cpp
cmp(search_node->value.first, parent->value.first) 
```
爆炸,就是因为parent是nullptr，说明树是空的，所以第一次插入必须特殊处理，直接把 root_node 指向新节点就行了

正确做法
```cpp
if(current_size == 0){
    root_node = new Node(val);
    root_node->color = 1; // 根节点必须是黑色
    current_size++;
    return;
}
```
### 根必须染黑
我一开始以为
``` 父亲红，爷爷一定存在```
但是崩溃的时候说明了第一次插入的根居然是红色，导致第二次插入的时候直接爆炸了
所以第一次插入必须把根染黑
```cpp
root_node->color = 1; // 根节点必须是黑色
```
红黑树一定要维护好颜色

###  插入旋转的时候漏维护 "中间子树" 的parent
例如 ```LL```
```cpp
// LL
grandpa->left = parent->right; 
father->right = grandpa; 
```
但是中间那颗子树,必须要
```cpp
if(grandpa->left != nullptr){
    grandpa->left->parent = grandpa; // 维护中间子树的parent
}
```
我的 ``LL``\``RR``\``LR``\``RL`` 都漏了维护中间子树的parent，导致旋转后树结构混乱，最终爆炸

### 中途因为张的太像，错误用过几次函数，例如RR写成RL

## 最困难的删除
### 双黑状态最初的理解，可以用color = 2
我们最后采用的是
```cpp
color = 2; // 双黑状态
```
让被删的黑叶子先作为幽灵节点留在树中，进入
```
adjust_delete_node(node) // 处理双黑状态
```
修复完再真正摘掉并delete
- 能够继续用node->parent来调整
- 能够判断双黑在左侧还是右侧
- 不需要额外传 (father, is_left) 之类的参数

### ```node->color == 2 ``` 写成了比较不是赋值
typo
```cpp
node->color == 2; // 错误的比较，应该是赋值
```
正确
```cpp
node->color = 2; // 正确的赋值
```

### 删除修复出口缺return
```cpp
if(node == root_node || node->color == 0){
    node->color = 1; // 双黑转黑
}
```
这里缺了 return，导致修复完后继续往下走，最终爆炸

### 删除的时候兄弟是空的，一开始以为正常，但是树已经失衡
```cpp
brother == nullptr; // 兄弟是空的，说明兄弟是叶子节点
```

后面我干到很奇怪，我质疑
    非根双黑节点的brother为什么会是nullptr?
如果非根双黑节点的brother是nullptr，这会使得不同路径的黑色节点数量不一致，导致树失衡，最终爆炸，所以我发现了这个错误
这意味着之前
- 单孩子删除没有染黑
- 删除修复提前/过度上移
- 树已经别破了

### 单孩子删除必须把唯一孩子染黑
合法红黑树中，如果一个节点只有一个非空孩子，一般是黑父+红子

删除黑父之后
- 红子上来
- 必须染黑
```cpp
son->color = 1; // 把唯一孩子染黑
```

### 删除红叶的时候要先断父亲指针，再delete
早期写过
```cpp
delete node; // 直接删除红叶，忘了先断父亲指针
```

正确做法
```cpp
if(father->left == node){
    father->left = nullptr; // 先断父亲指针
}else{
    father->right = nullptr;
}
delete node; // 再删除红叶
```

### 根节点删除要特判
- 根 + 0 孩子， 删除后树空了，直接 root_node = nullptr 就行了
- 根 + 1 孩子， 删除后孩子上来，必须染黑，parent = nullptr;
- 根 + 2 孩子， 掉下去走正常流程

## 双孩子删除，最关键的修复
一开始写的是
```cpp
node->value.~pair();
new (&node->value) pair<const Key, T>(successor->value.first, successor->value.second);
map_delete_node(successor); // 这里直接删除 successor，忘了先把 successor 从树上摘掉
```
- 查找正确
- correctness 能过
但是 Memcheck 说 ``` invalid read of size 4```
- 如果外界持有后继节点的iterator,会把原来的iterator变成悬空指针，导致访问时崩溃

### 最后改成了交换节点位置、删除原节点本体

删除双孩子节点的时候
- 找直接后继 successor
- 不是复制value
- 而是把整个successor 和 node 位置交换
- 递归删除node

# iterator 的细节
## end() 不是返回最大节点

一开始的end返回最大元素，导致
find(max_key) == end() // 错误

**正确**
end().node == nullptr // end() 的 node 是 nullptr

## ++ 不是有右去右侧，否则去父亲
- 有右子树 去右子树最左
- 没有右子树 沿着 parent 上去，直到当前节点是父节点的左子树
- 再走到父节点
- 如果找不到 就是 end()

## --end() 是最大节点
必须在operator-- 里面做特判

## ++end() --begin() *end() end()->
抛出异常

# 性能
## TLE的原因不是插入，而是删除把树删歪了
一开始猜测
### 可能是插入慢
我们把insert_val 的返回值改成了
```cpp
pair<Node*,bool>
```
一次下行完成查重、找插入位置
### 后面树高诊断
打印插入100000次后的树高 height = 31
删除过程中
```bash
i = 10000, height = 11504
i = 20000, height = 14695
```
删除修复把红黑树变成链表了

问题在
```cpp
兄弟黑、兄弟孩子全黑
```
一开始写的是
```cpp
brother->color = 0; // 兄弟染红
father->color = 2; // 父亲进入双黑状态
adjust_delete_node(father); // 继续修复父亲
```
问题是，这个忘记了区分父亲原本是红还是黑了
正确逻辑

- father red 直接染黑结束
- father black 进入双黑继续修复


1. root_node = nullptr 表示空树
2. pair<const Key,T> 不能整体赋值
3. 整树深拷贝应由 clone_tree 完成
4. 根节点必须永远黑
5. 旋转要维护中间子树的 parent
6. 删除黑叶双黑修复要有正确终止条件
7. 单孩子删除后孩子要染黑
8. 双孩子删除不能 copy value 再删后继，会破坏 iterator 稳定性
9. ++ / -- 必须按中序前驱后继写
10. 删除平衡性错误会从 O(log n) 退化到 O(n)，TLE 不是常数问题