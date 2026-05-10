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

## iterator 的讲解:
### iterator 的本质
对 vector 来说，底层是连续内存，所以iterator 本质就是
```
一个指向 data 中 某个位置的 T* 指针包装器
```
### iterator 里面最少需要存什么?
最小模型: iterator 里面只存一个 T* 指针
但是很多 STLite 还会为了检查 iterator 是否属于同一个vector，所以额外保存一个指向 vector 的指针
```cpp
const vector* parent; // 指向所属 vector 的指针
```
### iterator 最小需要支持的操作
1.解引用 *it

    返回元素 T&

2.成员访问 it->member

    返回T*

3.前置递增 ++it

    ptr向后移动一个位置，返回更新后的 iterator

4.后置递增 it++

    先保存当前 iterator 的副本，然后 ptr 向后移动一个位置，最后返回之前保存的副本

5.前置递减 --it

    ptr向前移动一个位置，返回更新后的 iterator

6.后置递减 it--

    ptr向前移动一个位置，返回之前保存的副本

7.加偏移 it + n

    返回一个新的 iterator，指向当前 iterator 向后移动 n 个位置

    原来的 iterator 不变，为什么？ 因为我们需要支持表达式 like it + n, 这要求 it + n 返回一个新的 iterator，而不是修改原来的 iterator。

8.减偏移 it - n

    返回一个新的 iterator，指向当前 iterator 向前移动 n 个位置

    原来的 iterator 不变，为什么？ 同样的道理，it - n 也需要返回一个新的 iterator，而不是修改原来的 iterator。

9.复合赋值 it += n

    ptr向后移动 n 个位置，返回更新后的 iterator

10.复合赋值 it -= n

    ptr向前移动 n 个位置，返回更新后的 iterator

11.两个iterator 相减 it1 - it2

    返回两个 iterator 之间的距离，通常返回difference_type 类型的值，表示 it1 和 it2 之间的元素数量差。

12.两个iterator 相等比较 it1 == it2

13.两个iterator 不等比较 it1 != it2

14.两个iterator 小于比较 it1 < it2

15.两个iterator 大于比较 it1 > it2

16.两个iterator 小于等于比较 it1 <= it2

17.两个iterator 大于等于比较 it1 >= it2


iterator类里面 返回iterator& 的操作方法是
```cpp
iterator& operator++() {
    // 前置递增
    ++ptr; // ptr向后移动一个位置
    return *this; // 返回更新后的 iterator
}
```
为什么是*this? 因为我们需要返回一个 iterator&，而 *this 就是当前对象的引用。通过返回 *this，我们可以支持链式调用，比如 ++it++，这要求前置递增返回一个 iterator&，而后置递增返回一个 iterator。

### const_iterator 的讲解
const_iterator 和 iterator 的区别在于，const_iterator 不允许通过它来修改元素的值。也就是说，
const_iterator 的解引用操作返回的是 const T&，而不是 T&。

除了 operator* 和 operator-> 返回 const T& 和 const T* 以外，const_iterator 的其他操作和 iterator 是一样的。

cbegin 和 cend 和普通的 begin 和 end 的区别在于，cbegin 和 cend 返回的是 const_iterator，而 begin 和 end 返回的是 iterator。这意味着通过 cbegin 和 cend 获取的迭代器不能用于修改元素的值，而通过 begin 和 end 获取的迭代器可以用于修改元素的值.


### iterator操作函数的讲解

insert: insert插入一个元素到指定位置，返回一个指向新插入元素的 iterator.

erase: erase删除指定位置的元素，返回一个指向被删除元素后面那个元素的 iterator.

先讲insert的思路，我们首先把iterator pos 减去 begin,得到index,然后从最后一个元素开始后移，直到index位置，然后在index位置构造一个新的元素，最后size++，返回begin() + index。

这个地方如果size不够了，就先调用一次扩容函数，扩容函数会重新分配内存并且把原来的元素复制过去。扩容完成后，再进行插入操作.

但是我们注意到，如果我们扩容，内存地址会改变，所以得先拿到index，再扩容，再插入。





## 实现过程中，我犯的错误
### 深拷贝的纠错
错误实现：我在这个地方犯了错
不过static_cast的目标应该是T*而不是T吧,因为我们需要一个指向T类型的指针来存储元素。
先前写成T的后果是，编译器会报错，因为我们试图将一个指向T类型的指针转换为一个指向T类型的指针，这在语义上是没有意义的。正确的写法应该是：
```cpp
/* 深拷贝 */
		this->data = static_cast<T*>(operator new(capacity * sizeof(T)));

		for(int i = 0 ; i < size ; i++){
			this->data[i] = other.data[i];
		}
```
这里的深拷贝其实是浅拷贝，因为我们只是复制了指针地址，而没有复制指针指向的内容。正确的深拷贝应该是为每个元素调用复制构造函数，或者使用 placement new 来构造新的对象。
```cpp
/* 正确的深拷贝 */
this->data = static_cast<T*>(operator new(capacity * sizeof(T)));
for(int i = 0 ; i < size ; i++){
    new (this->data + i) T(other.data[i]);
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

### iterator 过程中犯的错误

构造函数加错名字了，太久没写基础忘了
```cpp
iterator(const vector* parent, T* ptr) : parent(parent), ptr(ptr) {}
```

返回类型中有些时候把返回类型 iterator 和 ptr搞混了，实际上应该用
```cpp
iterator operator+(difference_type n) const {
    return iterator(parent, ptr + n);
}
```

这种类似的写法，或者先声明 一个新的 iterator 对象，然后返回它
```cpp 
iterator operator+(difference_type n) const {
    iterator result;
    result.parent = parent;
    result.ptr = ptr + n;
    return result;
}
```

iterator& 类型的操作方法，返回的是 *this

为什么呢？ 因为这个地方返回的是当前对象的引用
*this的类型是 iterator&，所以我们可以直接返回 *this 来支持链式调用.



# debug环节

## 我操你妈内存炸了，现在一个一个排

### vectorone
用了 push_back

用了构造函数

用了析构函数

Iterator 也用了，Constiterator 也用了,begin end 也用了 cbegin cend 也用了 没毛病

push \ pop 没问题

insert没问题

earse没问题

### vectortwo
用了push_back
用了insert
用了erase
没问题，除了慢了点。

### vectorthree



### vectorseven

通过调用发现了是test_insert的问题

这个test_insert的代码是
```cpp
void test_insert() {
	sjtu::vector<std::vector<int>> a;
	for (int i = 0; i < 10; ++i)
		a.push_back(std::vector<int>{1, 2});
	sjtu::vector<std::vector<int>> b = a;
	sjtu::vector<std::vector<int>> c;
	c = a;
	for (int i = 0; i < 10; ++i) {
		b[i].push_back(0);
		c[i].push_back(1);
	}
	for (int i = 0; i < 10; ++i) {
		a.insert(a.begin() + 1, std::vector<int>{1, 2});
		b.insert(b.begin() + 1, a[1]);
		c.insert(c.begin() + 1, b[1]);
		c[1].push_back(3);
		b[1].push_back(4);
	}
	for (int i = 0; i < 20; ++i) {
		for (auto x : a[i])
			printf("%d ", x);
		puts("");
		for (auto x : b[i])
			printf("%d ", x);
		puts("");
		for (auto x : c[i])
			printf("%d ", x);
		puts("");
	}
}
```
函数的流程是
1. 创建一个 vector a，里面有10个元素，每个元素都是一个 vector<int>，初始值为 {1, 2}。
2. 创建一个 vector b，使用 a 的拷贝构造函数。
3. 创建一个 vector c，使用 a 的赋值运算符。
4. 对 b 和 c 中的每个元素进行修改，向每个 vector<int> 中添加一个新的元素，b 中添加 0，c 中添加 1。
5. 对 a、b、c 进行多次插入操作，在每次插入后对 b 和 c 中的第一个元素进行修改，向它们添加新的元素。
6. 最后打印 a、b、c 中的所有元素。

## 发现问题1
insert 函数
在已经构造的位置重复 placement new

我知道要 placement new，于是对移动后的每个位置都 placement new。
但是我忘了先调用析构函数销毁原来的对象了，所以就导致了内存泄漏和未定义行为。

错误代码
```cpp
	iterator insert(iterator pos, const T &value) {
		int index = pos - this->begin();
		/* 检查是否需要扩容 */
		if(curr_size == curr_capacity){
			int new_capacity = std::max(1,2*curr_capacity);
			resize(new_capacity);
		}
		/* 开始从最后移动 */
		for(int i = curr_size ; i > index ; i--){
			
			new (this->data + i) T(this->data[i-1]);
		}
		/* index(包括index) 后的所有元素都被后移了 */
		/* 没有构造函数，直接new一个 */
		
		new(this->data + index)T(value);

		curr_size++;

		return iterator(&this->data[index] , this);
	}
```

正确代码
```cpp
    iterator insert(iterator pos, const T &value) {
		int index = pos - this->begin();
		/* 检查是否需要扩容 */
		if(curr_size == curr_capacity){
			int new_capacity = std::max(1,2*curr_capacity);
			resize(new_capacity);
		}
		/* 开始从最后移动 */
		for(int i = curr_size ; i > index ; i--){
			(this->data + i)->~T();
			new (this->data + i) T(this->data[i-1]);
		}
		/* index(包括index) 后的所有元素都被后移了 */
		/* 没有构造函数，直接new一个 */
		(this->data + index)->~T();
		new(this->data + index)T(value);

		curr_size++;

		return iterator(&this->data[index] , this);
	}
```

可以见得我之前犯了个错误，我以为new的时候会覆盖原来的对象，所以就没有调用析构函数销毁原来的对象，结果就导致了内存泄漏和未定义行为。

但是实际上，我们是必须显式析构的。

## 发现问题2
six测试点我写了自动缩容，但是测试点在恶意攻击缩容，所以我就把自动缩容的代码注释掉了，结果就过了。

