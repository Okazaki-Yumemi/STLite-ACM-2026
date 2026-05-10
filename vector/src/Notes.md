# Vector完成笔记

## 这个地方用来记录Vector类的完成情况，和一些相关的笔记。
## 2026-05-10

### 一些注意事项
vector 不变量：
1. data 指向一段容量为 capacity 的连续内存。
2. size 表示有效元素数量。
3. 任何时刻 0 <= size <= capacity。
4. begin() == data。
5. end() == data + size。
6. 当扩容发生时，所有旧 iterator 失效。


push_back：
如果 size == capacity，先扩容。
然后 data[size] = value。
最后 size++。

pop_back：
如果 size == 0，抛 container_is_empty。
否则 size--。

insert：
先把 iterator 转成 index。
必要时扩容。
从后向前移动元素。
插入 value。
size++。
返回 begin() + index。

erase：
把 index 后面的元素前移。
size--。
返回 begin() + index。

### 如何解决没有默认构造函数的问题？

先申请一块能够容纳 n 个 T 的内存，但不调用构造函数。

当 push_back 或 insert 时，再在指定位置构造一个真正的 T 对象。

当 pop_back 或 erase 时，先调用析构函数销毁对象，然后再移动元素。

那我怎么申请一块内存但不调用构造函数呢？
可以使用 operator new 来分配原始内存，而不是 new T[n]，这样就不会调用构造函数。
```cpp
data = static_cast<T*>(operator new(capacity * sizeof(T)));
```
当需要构造一个 T 对象时，可以使用 placement new：
```cpp
new (data + index) T(value);
```
当需要销毁一个 T 对象时，可以直接调用析构函数：
```cpp
(data + index)->~T();
```