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
## 学会的知识
static_cast 的讲解:
static_cast 是 C++ 中的一种类型转换运算符，用于在编译时进行类型转换。它比 C 风格的强制类型转换更安全，因为它会检查类型之间的兼容性，并且不允许一些不安全的转换。
在我们的实现中，我们使用 static_cast 来将 operator new 返回的 void* 指针转换为 T* 指针，以便我们可以在这块内存上构造 T 对象。
```cpp
data = static_cast<T*>(operator new(capacity * sizeof(T)));
```
这里的 operator new 返回一个 void* 指针，指向一块足够大的内存来存储 capacity 个 T 对象。我们使用 static_cast 将这个 void* 指针转换为 T* 指针，这样我们就可以通过 data 来访问这块内存，并在上面构造 T 对象。


## 实现过程中，我犯的错误
### 深拷贝的纠错
错误实现：我在这个地方犯了错
不过static_cast的目标应该是T*而不是T吧,因为我们需要一个指向T类型的指针来存储元素。
先前写成T的后果是，编译器会报错，因为我们试图将一个指向T类型的指针转换为一个指向T类型的指针，这在语义上是没有意义的。正确的写法应该是：
```cpp
/* 深拷贝 */
		this->data = static_cast<T*>(operator new(capacity * sizeof(T)));

		for(int i = 0 ; i < size ; i++){
			this->data[i] = other->data[i];
		}
```
这里的深拷贝其实是浅拷贝，因为我们只是复制了指针地址，而没有复制指针指向的内容。正确的深拷贝应该是为每个元素调用复制构造函数，或者使用 placement new 来构造新的对象。
```cpp
/* 正确的深拷贝 */
this->data = static_cast<T*>(operator new(capacity * sizeof(T)));
for(int i = 0 ; i < size ; i++){
    new (this->data + i) T(other->data[i]);
}
```
这样我们就为每个元素创建了一个新的对象，并且调用了复制构造函数来复制内容。

### 纠错
前面已经决定用 raw memory + placement new + 显示析构
但是实现过程中混入了
```cpp
delete data[i]
new T(value)
delete[] data
```
实际上，我们应该使用
```cpp
(data + index)->~T() // 显示调用析构函数
new (data + index) T(value) // placement new 构造对象
operator delete(data) // 释放 raw memory
```
不过返回，访问可以用[]，因为我们已经保证了 data 指向的是一段连续的内存。

这个地方为什么要这么做？
原理是，使用 operator new 分配的内存不会调用构造函数，所以我们需要使用 placement new 来在这块内存上构造对象。同时，使用 operator delete 来释放内存，而不是 delete[]，因为我们没有使用 new[] 来分配内存。

data怎么定义呢？ 这个没有犯错，还好。
```cpp
T* data; // 指向 raw memory 的指针
```
