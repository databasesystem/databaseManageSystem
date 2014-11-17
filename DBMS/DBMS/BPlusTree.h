#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H
#include <stdio.h>
#include <stdlib.h>

#define MIN_ORDER 3
#define MAX_ORDER 20

typedef struct record{
	int value;
};

typedef struct node{
	void ** pointers;
	int * keys;
	node * parent;
	bool is_leaf;
	int num_keys;
	node * next;
};

class BPlusTree {
public:
	int order;
	bool verbose_output;
	node * queue;

	BPlusTree();
	void license_notice( void );
	void print_license( int licence_part );
	void usage_1( void );
	void usage_2( void );
	void usage_3( void );
	void enqueue( node * new_node );
	node * dequeue( void );
	int height( node * root );
	int path_to_root( node * root, node * child );
	void print_leaves( node * root );
	void print_tree( node * root );
	void find_and_print(node * root, int key, bool verbose);
	void find_and_print_range(node * root, int range1, int range2, bool verbose);
	int find_range( node * root, int key_start, int key_end, bool verbose,
			int returned_keys[], void * returned_pointers[]);
	node * find_leaf( node * root, int key, bool verbose );
	record * find( node * root, int key, bool verbose );
	int cut( int length );

	// Insertion.

	record * make_record(int value);
	node * make_node( void );
	node * make_leaf( void );
	int get_left_index(node * parent, node * left);
	node * insert_into_leaf( node * leaf, int key, record * pointer );
	node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer);
	node * insert_into_node(node * root, node * parent,
			int left_index, int key, node * right);
	node * insert_into_node_after_splitting(node * root, node * parent, int left_index,
			int key, node * right);
	node * insert_into_parent(node * root, node * left, int key, node * right);
	node * insert_into_new_root(node * left, int key, node * right);
	node * start_new_tree(int key, record * pointer);
	node * insert_node( node * root, int key, int value );

	// Deletion.

	int get_neighbor_index( node * n );
	node * adjust_root(node * root);
	node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime);
	node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index,
			int k_prime_index, int k_prime);
	node * delete_entry( node * root, node * n, int key, void * pointer );
	node * delete_node( node * root, int key );
	//add
	void destroy_tree_nodes(node * root);
	node * remove_entry_from_node(node * n, int key, node * pointer);
	node * delete_nodes(node * root, int key);
	node * destroy_tree(node * root);
};

#endif