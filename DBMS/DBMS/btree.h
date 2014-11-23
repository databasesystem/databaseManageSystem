#ifndef BTREE_H_
#define BTREE_H_
#include <stdio.h>
#include <stdlib.h>

template<typename T> struct record {
	T value;
	record(T value) :
		value(value) {
	}
	;
};

template<typename T> struct node {
	bool is_leaf;
	int num_keys;
	node * parent;
	node * next;
	T * keys;
	void ** pointers;
	node(bool is_leaf, int order = 4) :
		is_leaf(is_leaf), num_keys(0), parent(NULL), next(NULL) {
		keys = new T[order - 1];
		pointers = new void*[order];
	}
	node(T key, void* left, void* right, int order = 4) :
		is_leaf(false), num_keys(1), parent(NULL), next(NULL) {
		keys = new T[order - 1];
		keys[0] = key;
		pointers = new void*[order];
		pointers[0] = left;
		pointers[1] = right;
	}
	~node() {
		delete[] keys;
		delete[] pointers;
	}
};

template<typename T> class bPlusTree {
protected:
	int _order;
	node<T>* _root;
	node<T>* queue;
public:
	bPlusTree(int order = 4) :
		_order(order) {
		_root = new node<T> (true);
	}
	~bPlusTree();
	node<T>* & root() {
		return _root;
	}
	bool empty() const {
		return !_root;
	}
	node<T>* search(T e);
	bool insert(T, T);
	bool remove(T);
	void destroy_tree_nodes(node<T>*);
	void enqueue(node<T>*);
	node<T> * dequeue();
	int path_to_root(node<T>*);
	void print_tree();
private:
	//insertion
	bool insert_into_leaf(node<T>*, T, record<T>*);
	bool insert_into_leaf_after_splitting(node<T>*, T, record<T>*);
	bool insert_into_node(node<T>*, int, T key, node<T>*);
	bool insert_into_node_after_splitting(node<T>*, int, T, node<T>*);
	bool insert_into_parent(node<T>*, T, node<T>*);
	//deletion
	int get_neighbor_index(node<T>* n);
	node<T>* remove_entry_from_node(node<T>*, int, node<T>*);
	bool adjust_root();
	bool coalesce_nodes(node<T>*, node<T>*, int, T);
	bool redistribute_nodes(node<T>*, node<T>*, int, int, T);
	bool delete_entry(node<T>*, T, void *);
	record<T>* find(T key);
};
#endif /* BTREE_H_ */
