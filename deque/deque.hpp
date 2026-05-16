#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */

		deque* owner;
		std::ptrdiff_t index;
		iterator(){
			this->owner = nullptr;
			this->index = 0;
		}
		iterator(deque* parent , size_t pos){
			this->owner = parent;
			this->index = (ptrdiff_t) pos;
		}

		iterator operator+(const int &n) const {
			//TODO
			iterator new_itr(this->owner,(size_t)this->index + (size_t) n);
			return new_itr;
		}
		iterator operator-(const int &n) const {
			//TODO
			iterator new_itr(this->owner,(size_t)this->index - (size_t) n);
			return new_itr;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if(this->owner != rhs.owner) throw invalid_iterator();
			return this->index - rhs.index;
		}
		iterator operator+=(const int &n) {
			//TODO
			iterator new_itr(this->owner,(size_t)this->index + (size_t) n);
			return new_itr;
		}
		iterator operator-=(const int &n) {
			//TODO
			iterator new_itr(this->owner,(size_t)this->index - (size_t) n);
			return new_itr;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator old ;
			old.index = this->index;
			old.owner = this->owner;
			this->index++;
			return old;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			this->index++;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator old ;
			old.index = this->index;
			old.owner = this->owner;
			this->index--;
			return old;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			this->index--;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			deque parent = *(this->owner);
			size_t block_index = parent.first_block + (this->index + first_offset)/(parent.BLOCK_CAPACITY);
			size_t new_offset = this->index + parent.first_offset - (block_index - parent.first_block)*parent.BLOCK_CAPACITY;
			return parent.block_lists[block_index].data[new_offset];
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			deque parent = *(this->owner);
			size_t block_index = parent.first_block + (this->index + first_offset)/(parent.BLOCK_CAPACITY);
			size_t new_offset = this->index + parent.first_offset - (block_index - parent.first_block)*parent.BLOCK_CAPACITY;
			return &(parent.block_lists[block_index].data[new_offset]);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return (this->owner == rhs.owner)&&(this->index == rhs.index);
		}
		bool operator==(const const_iterator &rhs) const {
			return (this->owner == rhs.owner)&&(this->index == rhs.index);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !((this->owner == rhs.owner)&&(this->index == rhs.index));
		}
		bool operator!=(const const_iterator &rhs) const {
			return !((this->owner == rhs.owner)&&(this->index == rhs.index));
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			deque* owner;
			std::ptrdiff_t index;
			const_iterator() {
				// TODO
				this->owner = nullptr;
				this->index = 0;
			}
			const_iterator(deque* parent , size_t pos){
				this->owner = parent;
				this->index = (ptrdiff_t) pos;
			}
			const_iterator(const iterator &other) {
				// TODO
				this->owner = other.owner;
				this->index = other.index;
			}
			const_iterator operator+(const int &n) const {
				//TODO
				const_iterator new_itr(this->owner,(size_t)this->index + (size_t) n);
				return new_itr;
			}
			const_iterator operator-(const int &n) const {
				//TODO
				const_iterator new_itr(this->owner,(size_t)this->index - (size_t) n);
				return new_itr;
			}
			int operator-(const const_iterator &rhs) const {
				//TODO
				if(this->owner != rhs.owner) throw invalid_iterator();
				return this->index - rhs.index;
			}
			const_iterator operator+=(const int &n) {
				//TODO
				const_iterator new_itr(this->owner,(size_t)this->index + (size_t) n);
				return new_itr;
			}
			const_iterator operator-=(const int &n) {
				//TODO
				const_iterator new_itr(this->owner,(size_t)this->index - (size_t) n);
				return new_itr;
			}
			const_iterator operator++(int) {
				const_iterator old ;
				old.index = this->index;
				old.owner = this->owner;
				this->index++;
				return old;
			}
			const_iterator& operator++() {
				this->index++;
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator old ;
				old.index = this->index;
				old.owner = this->owner;
				this->index--;
				return old;
			}
			const_iterator& operator--() {
				this->index--;
				return *this;
			}
			const T& operator*() const {
				deque parent = *(this->owner);
				size_t block_index = parent.first_block + (this->index + first_offset)/(parent.BLOCK_CAPACITY);
				size_t new_offset = this->index + parent.first_offset - (block_index - parent.first_block)*parent.BLOCK_CAPACITY;
				return parent.block_lists[block_index].data[new_offset];
			}
			const T* operator->() const noexcept {
				deque parent = *(this->owner);
				size_t block_index = parent.first_block + (this->index + first_offset)/(parent.BLOCK_CAPACITY);
				size_t new_offset = this->index + parent.first_offset - (block_index - parent.first_block)*parent.BLOCK_CAPACITY;
				return &(parent.block_lists[block_index].data[new_offset]);
			}
			bool operator==(const iterator &rhs) const {
				return (this->owner == rhs.owner)&&(this->index == rhs.index);
			}
			bool operator==(const const_iterator &rhs) const {
				return (this->owner == rhs.owner)&&(this->index == rhs.index);
			}
			bool operator!=(const iterator &rhs) const {
				return !((this->owner == rhs.owner)&&(this->index == rhs.index));
			}
			bool operator!=(const const_iterator &rhs) const {
				return !((this->owner == rhs.owner)&&(this->index == rhs.index));
			}
	};
	/**
	 * TODO Constructors
	 */


	/*设置block size,默认为8*/
	size_t BLOCK_CAPACITY = 8;

	class block
	{
	public:
		size_t block_capacity;
		T* data;

		/*block的默认构造*/
		block(){
			this->block_capacity = BLOCK_CAPACITY;
			/*开始构造*/
			this->data = static_cast<T*>(operator new(block_capacity * sizeof(T)));
		};

		/*block不用写复制构造，它不管理元素*/

		~block(){
			/*把析构的任务交给deque管理了*/
			operator delete(data)
		};
	};
	
	/*deque 的参数,维护一个表*/
	block* block_lists;
	/*block map的size*/
	size_t deque_capacity ; // block的最大数目
	/*第一个元素所在的block索引*/
	size_t first_block; //第一个元素所在的block索引
	size_t first_offset; //第一个元素所在block内的偏差
	
	size_t current_size; //元素数目

	deque(size_t BLOCKMAP_CAPACITY,size_t BLOCK_CAPACITY_outside) {
		/*deque的默认构造*/
		/*外界是否修改BLOCK的容量？*/
		this->BLOCK_CAPACITY =BLOCK_CAPACITY_outside
		this->deque_capacity =BLOCKMAP_CAPACITY;
		this->block_lists = new block[this->deque_capacity];
		/*从中间开始放*/
		first_block = this->deque_capacity/2;
		/*偏移为0*/
		first_offset = 0;
		/*元素数目为0*/
		current_size = 0;
	}
	deque(const deque &other) {
		/*拷贝构造*/
		/*先复制参数*/
		this->deque_capacity = other.deque_capacity;
		this->BLOCK_CAPACITY = other.BLOCK_CAPACITY;
		this->first_block = other.first_block;
		this->first_offset = other.first_offset;
		this->current_size = other.current_size;
		/*开始拷贝内存*/
		this->block_lists = new block[this->deque_capacity];

		/*开始一个一个抄*/
		for(int i = 0 ; i <this->current_size ; i++ ){
			/*计算block idx*/
			size_t block_index = first_block+(i+first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = i+first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;
			/* 构造 */
			T val = other.block_lists[block_index].data[new_offset];
			new (this->block_lists[block_index].data+new_offset) T(val);
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		/*析构函数，我们要首先从fisrt_block开始一个一个释放T元素的内存，然后一个一个释放block，最后删除自己*/
		for(int i = 0 ; i < this->current_size ; i++){
			size_t block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;
			/*显式析构*/
			(this->block_lists[block_index].data + new_offset)->~T();
		}

		/*删除block*/
		delete[] block_lists;
		//这个函数自动调用所有block的析构函数
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if(this == &other){
			return *this;
		}

		/*删除老数据*/
		for(int i= 0 ; i < this->current_size ; i++){
			size_t block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

			/*析构*/
			(this->block_lists[block_index].data+new_offset)->~T();
		}
		delete[] block_lists; // 自动删除block

		/*拷贝*/
		this->current_size = other.current_size;
		this->deque_capacity = other.deque_capacity;
		this->first_block = other.first_block;
		this->first_offset = other.first_offset;
		this->BLOCK_CAPACITY = other.BLOCK_CAPACITY;

		this->block_lists = new block[this->deque_capacity];

		/*抄*/
		for(int i = 0 ; i < this->current_size ; i++){
			/*计算block idx*/
			size_t block_index = first_block+(i+first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = i+first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;
			/* 构造 */
			T val = other.block_lists[block_index].data[new_offset];
			new (this->block_lists[block_index].data+new_offset) T(val);
		}
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		
		size_t block_index = first_block + (pos + first_offset)/(this->BLOCK_CAPACITY);

		size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*offset 不会超界*/
		/*我们能保证first合法,因此只可能尾部超出*/
		if(block_index >= this->deque_capacity) throw index_out_of_bound();

		/*解析-> 先定位block, 再 拿到data ，再下标访问*/
		return this->block_lists[block_index].data[new_offset];
	}
	const T & at(const size_t &pos) const {

		size_t block_index = first_block + (pos + first_offset)/(this->BLOCK_CAPACITY);

		size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*offset 不会超界*/
		/*我们能保证first合法,因此只可能尾部超出*/
		if(block_index >= this->deque_capacity) throw index_out_of_bound();

		/*解析-> 先定位block, 再 拿到data ，再下标访问*/
		return this->block_lists[block_index].data[new_offset];
	}
	T & operator[](const size_t &pos) {
		/*这三个没区别*/
		size_t block_index = first_block + (pos + first_offset)/(this->BLOCK_CAPACITY);

		size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*offset 不会超界*/
		/*我们能保证first合法,因此只可能尾部超出*/
		if(block_index >= this->deque_capacity) throw index_out_of_bound();

		/*解析-> 先定位block, 再 拿到data ，再下标访问*/
		return this->block_lists[block_index].data[new_offset];
	}
	const T & operator[](const size_t &pos) const {
		/*这四个没区别*/
		size_t block_index = first_block + (pos + first_offset)/(this->BLOCK_CAPACITY);

		size_t new_offset = i + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*offset 不会超界*/
		/*我们能保证first合法,因此只可能尾部超出*/
		if(block_index >= this->deque_capacity) throw index_out_of_bound();

		/*解析-> 先定位block, 再 拿到data ，再下标访问*/
		return this->block_lists[block_index].data[new_offset];
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if(this->current_size == 0) throw container_is_empty();
		/*拿第一个，相当于pos = 0*/
		return this->at(0);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if(this->current_size == 0) throw container_is_empty();
		/*拿最后一个个，相当于pos = size - 1*/
		return this->at(this->current_size - 1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		iterator it;
		it.owner = this;
		it.index = 0;
		return it;
	}
	const_iterator cbegin() const {
		const_iterator it;
		it.owner = this;
		it.index = 0;
		return it;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		iterator it;
		it.owner = this;
		it.index = this->current_size - 1;
		return it;
	}
	const_iterator cend() const {
		const_iterator it;
		it.owner = this;
		it.index = this->current_size - 1;
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return current_size == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return current_size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		/*从fisrt_block开始一个一个释放T元素的内存，然后一个一个释放block，最后删除自己*/
		for(int i = 0 ; i < this->current_size ; i++){
			size_t block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = i + first_offset - (block_index-first_block)  * this->BLOCK_CAPACITY;
			/*显式析构*/
			(this->block_lists[block_index].data[new_offset])->~T();
		}
		/*清0之后重置size等参数*/
		current_size = 0;
		first_block = this->deque_capacity / 2;
		first_offset = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		if(pos.owner != this || pos.index > current_size) throw invalid_iterator();
		size_t index = pos.index;
		/*检查需不需要扩容*/

		/*插入之后，最后一个元素的位置*/
		size_t block_index = first_block + (current_size + first_offset)/(this->BLOCK_CAPACITY);
		size_t new_offset = current_size + first_offset - (block_index - first_block)*this->BLOCK_CAPACITY;

		if(block_index == deque_capacity){
			/*扩容*/
			this->resize(this->deque_capacity*2);
		}
		/*从current_size 到 index位置，从最后移动 */
		if(index == current_size){
			/*如果是最后一个，就不用管了*/
			block_index = first_block + (current_size + first_offset)/(this->BLOCK_CAPACITY);
			new_offset = current_size + first_offset - (block_index - first_block)*this->BLOCK_CAPACITY;
			new (this->block_lists[block_index].data[new_offset]) T(value);
			this->current_size++;
			return iterator(this,index);
		}
		/*开始从最后一个移动*/
		for(int i = this->current_size ; i > index ; i--){
			block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			new_offset = i + first_offset - (block_index - first_block)*this->BLOCK_CAPACITY;
			if(i != current_size) (this->block_lists[block_index].data[new_offset]) ->~T();
			size_t front_one_block = first_block + (i - 1 + first_offset)/(this->BLOCK_CAPACITY);
			size_t front_one_offset = i - 1 + first_offset - (block_index - first_block)*this->BLOCK_CAPACITY;
			new (this->block_lists[block_index].data[new_offset]) T(this->block_lists[front_one_block].data[front_one_offset]);
		}
		/*然后new新的*/
		block_index = first_block + (index + first_offset)/(this->BLOCK_CAPACITY);
		new_offset = index + first_offset - (block_index - first_block)*this->BLOCK_CAPACITY;
		(this->block_lists[block_index].data[new_offset])->~T();
		new (this->block_lists[block_index].data[new_offset]) T(value);
		this->current_size ++;
		return iterator(this,index);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		if(pos.owner != this || pos.index >= this->current_size) throw invalid_iterator();
		size_t index = pos.index;
		for(size_t i = index ; i < current_size - 1 ; i++){
			size_t this_block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			size_t this_new_offset = i + first_offset - (this_block_index - first_block)*this->BLOCK_CAPACITY;

			size_t next_block_index =first_block + (i + 1 + first_offset)/(this->BLOCK_CAPACITY);
			size_t next_new_offset = i + 1 + first_offset - (this_block_index - first_block)*this->BLOCK_CAPACITY;

			(this->block_lists[this_block_index].data[this_new_offset]) ->~T();
			new (this-block_lists[this_block_index].data[this_new_offset]) T(this->block_lists[next_block_index].data[next_new_offset]);
		}
		this->current_size --;
		return iterator(this,index);
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		/*好的，我们来检查尾部插入的情况*/
		/*这个地方不涉及到first block 的移动，也不涉及到first offset的改变，比较好写*/

		/*先判断目前要不要扩容*/
		/*这个地方都是指插入的位置*/
		size_t block_index = first_block + (current_size  + first_offset)/(this->BLOCK_CAPACITY);
		size_t new_offset = current_size  + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*判断位置是否合法?*/
		//if(new_offset == BLOCK_CAPACITY){
		//		new_offset = 0;
		//	block_index ++ ;
		//}   这个检测是多余的
		/*判断block是否存在*/
		if(block_index == deque_capacity){
			/*必须扩容了*/
			this->resize(this->deque_capacity * 2);
			/*重新计算*/
			size_t block_index = first_block + (current_size  + first_offset)/(this->BLOCK_CAPACITY);
			size_t new_offset = current_size  + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

			/*插入*/
			new (this->block_lists[block_index].data[new_offset]) T(value);
			this->current_size++;

		}else{
			/*不用*/
			new (this->block_lists[block_index].data[new_offset]) T(value);
			this->current_size++;
		}
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if(this->current_size == 0) throw container_is_empty();
		/*尾部弹出的情况*/
		size_t index = current_size - 1;
		/*先拿参数*/
		size_t block_index = first_block + (current_size + index)/(this->BLOCK_CAPACITY);
		size_t new_offset = current_size + first_offset - (block_index - first_block) * this->BLOCK_CAPACITY;

		/*调用析构函数*/
		(this->block_lists[block_index].data[new_offset])->~T();

		/*更新参数*/
		this->current_size --;
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		/*push_front 稍微复杂一点点，但是其实大差不差*/
		/*而且还更方便! 我们不用去算参数了*/
		if(this->first_offset == 0){
			first_block--;
			first_offset = this->BLOCK_CAPACITY - 1;
		}else{
			first_offset --;
		}

		if(this->first_block < 0){
			/*出界了！*/
			this->resize(this->deque_capacity * 2);
			
			/*我们已经在resize里面做过，我们把first_block 默认设定为0了,所以可以直接*/
			this->first_block --;
			this->first_offset = this->BLOCK_CAPACITY - 1;
		}
		/*构造*/
		new (this->block_lists[first_block].data[first_offset]) T(value);
		this->current_size ++;
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if(this->current_size == 0) throw container_is_empty();
		else{
			(this->block_lists[first_block].data[first_offset])->~T();
			this->first_offset++;
			if(first_offset == this->BLOCK_CAPACITY){
				this->first_block++;
				this->first_offset = 0;
			}
			this->current_size --;
		}
	}

	void resize(size_t new_block_capacity){
		/*其实这个地方就是构造一个新的blocklist，然后拷贝*/
		block* new_block_lists = new block[new_block_capacity];
		size_t new_first_block = new_block_capacity/2;
		size_t new_first_offset = 0;
		/*开始拷贝*/
		for(int i = 0 ; i < this->current_size ; i++){
			/*这一段，我们寻找原来的值*/
			size_t block_index = first_block + (i + first_offset)/(this->BLOCK_CAPACITY);
			size_t old_new_offset = i + first_offset - (block_index - first_block)  * this->BLOCK_CAPACITY;

			/*拿到值*/
			T val = this->block_lists[block_index].data[old_new_offset];
			/*拿完就删除原来的*/
			(this->block_lists[block_index].data[old_new_offset])->~T();

			/*拷贝到新的*/
			size_t new_block_index = new_first_block + (i + new_first_offset)/(this->BLOCK_CAPACITY); // block capacity不变
			size_t new_new_offset = i + new_first_offset - (new_block_index - new_first_block) * this->BLOCK_CAPACITY;

			new (new_block_lists[new_block_index].data + new_new_offset) T(val);
		}

		delete[] this->block_lists;

		this->block_lists = new_block_lists;
		this->first_block = new_first_block;
		this->first_offset = new_first_offset;
	}
};

}

#endif
