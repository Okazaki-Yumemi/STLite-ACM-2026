# 1.Deque是什么
Deque（Double Ended Queue）是一种双端队列数据结构，允许在两端进行插入和删除操作。它结合了栈和队列的特性，可以在头部和尾部高效地进行元素的添加和移除。

核心能力
- 头部插入/删除：在 O(1) 时间内在头部插入或删除元素。
- 尾部插入/删除：在 O(1) 时间内在尾部插入或删除元素。
- 随机访问：在 O(1) 时间内通过下标访问元素。
- 迭代器支持：提供迭代器以便于遍历和操作元素。
- 随机位置插入/删除：在 O(n) 时间内在任意位置插入或删除元素。

# 2.为什么vector不适合做deque?
vector底层是一块连续内存
```
[data0] [data1] [data2] ... [dataN]
```
尾插很自然，但头插需要移动所有元素，导致O(n)时间复杂度。

# 3.那能不能用Linked List实现Deque?
下标随机访问需要O(n)时间，迭代器加n也需要O(n)，不满足要求。

# 4.deque的经典折中: 分块数组

一组固定大小的小数组block,再用一个索引结构来管理这些block。这样可以在头尾插入删除时只操作少量元素，同时保持随机访问的效率。

可以想象为
```
block0: [ ][ ][A][B]
block1: [C][D][E][F]
block2: [G][H][ ][ ]
```
逻辑序列是
```A B C D E F G H
```

虽然物理内存不是连续的，但是每个block内部是连续的

# 5.这样为什么能够兼顾?
- 头尾插入/删除：只需在当前block的边界进行操作，或者在需要时分配新的block，保持O(1)时间复杂度。
- 随机访问：通过索引结构快速定位到对应的block和block内的偏移，实现O(1)时间复杂度。

如果每一块的大小是Block_SIZE，那么访问第i个元素时，可以通过以下方式计算：
```
全局逻辑偏移 = front_offset + i
block_index = 全局逻辑偏移 / Block_SIZE
block_offset = 全局逻辑偏移 % Block_SIZE
```

# 6.一个更具体的抽象模型
```
map:
[block_prt0] [block_prt1] [block_prt2] ... [block_prtM]
```
每个block_ptr指向一个block，block内部是一个固定大小的数组。
```cpp
template<typename T>
T block[Block_SIZE];
```

# 7.常见的数据成员
- block_map: 存储block指针的数组。
- 当前block数量和容量：记录当前分配的block数量以及block_map的容量。

- front_offset: 记录deque头部元素在逻辑序列中的偏移，用于计算元素的全局位置。
- front_ptr: 指向当前头部元素所在的block。

- back_ptr: 指向当前尾部元素所在的block。
- back_offset: 记录deque尾部元素在逻辑序列中的偏移。

- size: 记录deque中元素的总数量。

# 8.迭代器设计
deque 的 iterator不连续，所以iterator至少要知道
- 当前在哪个block
- 当前在block内的偏移
- 所属deque是谁

# 9. insert和erase的实现

- 哪边元素少就从哪边移动元素，减少移动的数量。

# 开始实现吧！

## 容器本体
```cpp
block* block_lists //块指针表，每个元素指向一个block
size_t map_capacity //块指针表的容量

size_t first_block //第一个元素所在block的索引
size_t first_offset //第一个元素在block内的偏移

size_t current_size //当前元素数量
```

第一版不同时维护front_ptr和back_ptr，而是通过first_block和first_offset来计算。

iterator设计
```cpp
deque* owenr
size_t index //全局逻辑偏移
```

所以对于begin()，index = 0；对于end()，index = current_size。

operator*() return (*owenr)[index]

## 考虑到没有默认构造函数
block不能是 [T][T][T]... 因为一旦申请了block，就会调用T的默认构造函数来初始化每个元素，这对于没有默认构造函数的类型来说是不可行的。

所以要把一个block理解为一段足够放Block_SIZE的内存空间，每个格子有两种状态
- 已经构造了一个T对象
- 还没有构造对象

## clear 析构所有活对象，释放所有block，然后回复到初始状态。

## block类
```cpp
	class block
	{
	public:
		size_t block_capacity;
		T* data;

		block(size_t capacity ){
			this->block_capacity = capacity;
			/*开始构造*/
			this->data = static_cast<T*>(operator new(block_capacity * sizeof(T)));
		};
		~block(){
			/*把析构的任务交给deque管理了*/
			operator delete(data)
		};
	};
	
```
block不参与管理元素，只负责分配和释放内存，元素的构造和析构由deque来管理。


## 写push之前我们先把resize写了



# MD 我这个地方有一点混着写，删除\构造的时候

```cpp
new (new_block_lists[new_block_index].data + new_new_offset) T(val);
```
和
```cpp
new (new_block_lists[new_block_index].data[new_new_offset] ) T(val);
```
是一样的，前者是对指针进行偏移，后者是对数组进行访问，最终得到的都是一个T对象的地址。


