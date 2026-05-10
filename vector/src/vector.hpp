#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
	
public:
	/* 定义基本参数 */
	int curr_size;
	int curr_capacity;
	T* data;	
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the
	// iterator points to.
	// STL algorithms and containers may use these type_traits (e.g. the following
	// typedef) to work properly. In particular, without the following code,
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/* 参数和构造,ptr指向位置,owner 表示父亲 */
		T* ptr;
		const vector<T>* owner;
		iterator(){
			ptr = nullptr;
			owner = nullptr;
		}
		iterator(T* place , const vector<T>* parent){
			owner = parent;
			ptr = place;
		}

		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const
		{
			//TODO
			iterator new_itr;
			new_itr.owner = this->owner;
			new_itr.ptr = this->ptr + n;
			return new_itr;
		}
		iterator operator-(const int &n) const
		{
			//TODO
			iterator new_itr;
			new_ptr.owner = this->owner;
			new_ptr.ptr = this->ptr - n;
			return new_itr;
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			//TODO
			if(this->owner != rhs.owner) throw invalid_iterator();
			else{
				return (int) (this->ptr - rhs.ptr);
			}
		}
		iterator& operator+=(const int &n)
		{
			//TODO
			this->ptr = this->ptr + n;
			return *this;
		}
		iterator& operator-=(const int &n)
		{
			//TODO
			this->ptr = this->ptr - n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator old_one;
			old_one.owner = this->owner;
			old_one.ptr = this->ptr;
			this->ptr += 1 ;
			return old_one;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			this->ptr += 1;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator old_one;
			old_one.owner = this->owner;
			old_one.ptr = this->ptr;
			this->ptr -= 1 ;
			return oldone;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			this->ptr -= 1;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return *this->ptr;
		}

		T* operator->() const{
			return this->ptr;
		}

		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
			return rhs.owner == this->owner  && this->ptr == rhs.ptr;
		}
		bool operator==(const const_iterator &rhs) const {
			return rhs.owner == this->owner  && this->ptr == rhs.ptr;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !this->operator==(rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !this->operator==(rhs);
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		/*TODO*/
	
	public:
		const T* ptr;
		const vector<T>* owner;
		/* 构造函数和前面大差不差，这个地方额外的增加一个从普通iterator派生 */
		const_iterator(){
			this->ptr = nullptr;
			this->owner = nullptr;
		}
		const_iterator(T* place , const vector<T>* parent){
			this->owner = parent;
			this->ptr = place;
		}
		const_iterator(iterator it){
			this->owner = it->owner;
			this->ptr = it->ptr;
		}

		const_iterator operator+(const int &n) const{
			const_iterator new_itr;
			new_itr.owner = this->owner;
			new_itr.ptr = this->ptr + n;
			return new_itr;
		}

		const_iterator operator-(const int &n) const{
			const_iterator new_itr;
			new_itr.owner = this->owner;
			new_itr.ptr = this->ptr - n;
			return new_itr;
		}

		int operator-(const const_iterator &rhs) const{
			if(this->owner != rhs.owner) throw invalid_iterator();
			else{
				return (int) (this->ptr - rhs.ptr);
			}
		}

		const_iterator& operator+=(cosnt int &n){
			this->ptr += n;
			return *this;
		}

		const_iterator& operator-=(const int &n){
			this->ptr -= n;
			return *this;
		}

		const_iterator operator++(int){
			const_iterator old_one;
			old_one.owner = this->owner;
			old_one.ptr = this->ptr;
			this->ptr ++;
			return old_one;
		}

		const_iterator& operator++(){
			this->ptr ++;
			return *this;
		}

		const_iterator operator--(int){
			const_iterator old_one;
			old_one.owner = this->owner;
			old_one.ptr = this->ptr;
			this->ptr --;
			return old_one;
		} 
		
		const_iterator& operator--(){
			this->ptr --;
			return *this;
		}

		const T& operator*() const{
			return *this->ptr;
		}

		const T* operator->() const{
			return this->ptr;
		}

		bool operator==(const iterator &rhs) const{
			return rhs.owner == this->owner && this->ptr == rhs.ptr;
		}
		
		bool operator==(const const_iterator &rhs) const{
			return rhs.owner == this->owner && this->ptr == rhs.ptr;
		}

		bool operator!=(const iterator &rhs) const{
			return !this->operator==(rhs);
		}

		bool operator!=(const const_iterator &rhs) const{
			return !this->operator==(rhs);
		}
	};
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector() {
		/*初始化容量*/
		curr_size = 0;
		curr_capacity = 8;
		/* 使用operator new 方法*/
		data = static_cast<T*>(operator new(curr_capacity * sizeof(T)));
	}
	vector(const vector &other) {
		/* 初始化容量 */
		this->curr_size = other.curr_size;
		this->curr_capacity = other.curr_capacity;
		/* 深拷贝 */
		this->data = static_cast<T*>(operator new(curr_capacity * sizeof(T)));

		for(int i = 0 ; i < curr_size ; i++){
			new (this->data + i) T(other.data[i]);
		}
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		for(int i = 0 ; i < curr_size ; i++){
			/* 删除不能用delete,我们这边已经决定用显式析构函数 */
			(data + i)->~T();
		}
		operator delete(data);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		/* 边界条件判断，防止出错 */
		if(this == &other){
			return *this;
		}
		/* 删除老数据 */
		for(int i = 0 ; i < curr_size ; i++){
			(data + i)->~T();
		}
		operator delete(data);
		
		/* 同复制构造函数 */
		this->curr_size = other.curr_size;
		this->curr_capacity = other.curr_capacity;
		/* 深拷贝 */
		this->data = static_cast<T*>(operator new(curr_capacity* sizeof(T)));

		for(int i = 0 ; i < curr_size ; i++){
			new (this->data + i) T(other.data[i]);
		}
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
		/* 查错 */
		if(pos < 0 || pos >= curr_size){
			throw index_out_of_bound();
		}else{
			return data[pos];
		}
	}
	const T & at(const size_t &pos) const {
		/* 查错 */
		if(pos < 0 || pos >= curr_size){
			throw index_out_of_bound();
		}else{
			return data[pos];
		}
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		/* 这个地方有写boundary检测的需求，我们就加上 */
		/* 查错 */
		if(pos < 0 || pos >= curr_size){
			throw index_out_of_bound();
		}else{
			return data[pos];
		}
	}
	const T & operator[](const size_t &pos) const {
		/* 查错 */
		if(pos < 0 || pos >= curr_size){
			throw index_out_of_bound();
		}else{
			return data[pos];
		}
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		/*边界条件检查*/
		if(curr_size == 0 ) throw container_is_empty();
		else{
			return this->data[0];
		}
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		if( curr_size == 0) throw container_is_empty();
		else{
			return this->data[curr_size-1];
		}
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {}
	const_iterator begin() const {}
	const_iterator cbegin() const {}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {}
	const_iterator end() const {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
		return curr_size == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return (size_t) curr_size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		for(int i = 0 ; i < curr_size ; i++){
			/* 显式调用析构函数 */
			(data + i)->~T();
		}
		curr_size = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		/* 发现已经满了 */
		if(curr_size == curr_capacity){
			int new_capacity = std::max(1,2*curr_capacity);
			resize(new_capacity);
		}
		/* 扩容完成 */
		new (data+curr_size)T(value);
		curr_size++;
		return;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if(curr_size == 0) throw container_is_empty();
		else{
			/* 显式析构 */
			(data+curr_size-1)->~T();
			curr_size--;
			/*如果小于一半，我们就缩容*/
			if(curr_size < curr_capacity/2 && curr_capacity > 16){
				resize(curr_capacity / 2);
			}
		}
	}

	void resize(int new_capacity){
		/* 新的数组 */
		T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));

		/* 把老数组拷贝到新的数组 */
		for(int i = 0 ; i < curr_size ; i++){
			new(new_data + i) T(this->data[i]);
			(data + i)->~T();
		}
		/* 指针改变指向 */
		operator delete(data);
		data = new_data;
		curr_capacity = new_capacity;
		return;
	}
};


}

#endif
