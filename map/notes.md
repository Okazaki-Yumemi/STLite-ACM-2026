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

## iterator类的设计

map* owner
Node* node