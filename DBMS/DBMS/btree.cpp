#include "btree.h"

// PRINT
void usage() {
	printf("Enter any of the following commands after the prompt > :\n");
	printf("\ti <k>  -- Insert <k> (an integer) as both key and value).\n");
	printf("\td <k>  -- Delete key <k> and its associated value.\n");
	printf(
			"\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n");
	printf("\tt -- Print the B+ tree.\n");
	printf("\tq -- Quit. (Or use Ctl-D.)\n");
}

template<typename T>
void bPlusTree<T>::enqueue(node<T>* new_node) {
	node<T>* c;
	if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	} else {
		c = queue;
		while (c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		new_node->next = NULL;
	}
}

template<typename T>
node<T>* bPlusTree<T>::dequeue() {
	node<T>* n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}

template<typename T>
int bPlusTree<T>::path_to_root(node<T>* child) {
	int length = 0;
	node<T>* c = child;
	while (c != _root) {
		c = c->parent;
		length++;
	}
	return length;
}

template<typename T>
void bPlusTree<T>::print_tree() {
	node<T>* n = NULL;
	int i = 0;
	int rank = 0;
	int new_rank = 0;

	if (_root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	queue = NULL;
	enqueue(_root);
	while (queue != NULL) {
		n = dequeue();
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			new_rank = path_to_root(n);
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		for (i = 0; i < n->num_keys; i++) {
			printf("%d ", n->keys[i]);
		}
		if (!n->is_leaf)
			for (i = 0; i <= n->num_keys; i++)
				enqueue((node<T>*) n->pointers[i]);
		printf("| ");
	}
	printf("\n");
}

// SEARCH

template<typename T>
node<T>* bPlusTree<T>::search(T key) {
	if (_root == NULL) {
		return NULL;
	}
	node<T>* c = _root;
	while (!c->is_leaf) {
		int i = 0;
		for (; i < c->num_keys && key >= c->keys[i]; i++)
			;
		c = (node<T>*) c->pointers[i];
	}
	return c;
}

template<typename T>
record<T>* bPlusTree<T>::find(T key) {
	node<T>* c = search(key);
	if (c == NULL)
		return NULL;
	for (int i = 0; i < c->num_keys; i++) {
		if (c->keys[i] == key)
			return (record<T>*) c->pointers[i];
	}
	return NULL;
}

// INSERTION

template<typename T>
bool bPlusTree<T>::insert_into_leaf(node<T>* leaf, T key, record<T>* pointer) {
	int insert_index = 0;
	while (insert_index < leaf->num_keys && leaf->keys[insert_index] < key)
		insert_index++;

	for (int i = leaf->num_keys; i > insert_index; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}
	leaf->keys[insert_index] = key;
	leaf->pointers[insert_index] = pointer;
	leaf->num_keys++;
	return true;
}

template<typename T>
bool bPlusTree<T>::insert_into_leaf_after_splitting(node<T>* leaf, T key,
		record<T>* pointer) {
	node<T>* new_leaf = new node<T> (true);
	int * temp_keys = new int[_order];
	void ** temp_pointers = new void*[_order];
	int insertion_index = 0;
	while (insertion_index < _order - 1 && leaf->keys[insertion_index] < key)
		insertion_index++;

	for (int i = 0, j = 0; i < leaf->num_keys; i++, j++) {
		if (j == insertion_index)
			j++;
		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}
	temp_keys[insertion_index] = key;
	temp_pointers[insertion_index] = pointer;
	leaf->num_keys = 0;

	for (int i = 0; i < _order / 2; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->num_keys++;
	}
	for (int i = _order / 2, j = 0; i < _order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->num_keys++;
	}
	delete[] temp_pointers;
	delete[] temp_keys;

	new_leaf->pointers[_order - 1] = leaf->pointers[_order - 1];
	leaf->pointers[_order - 1] = new_leaf;
	new_leaf->parent = leaf->parent;

	return insert_into_parent(leaf, new_leaf->keys[0], new_leaf);
}

template<typename T>
bool bPlusTree<T>::insert_into_node(node<T>* n, int left_index, T key,
		node<T>* right) {
	for (int i = n->num_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] = n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
	n->keys[left_index] = key;
	n->num_keys++;
	return true;
}

template<typename T>
bool bPlusTree<T>::insert_into_node_after_splitting(node<T>* old_node,
		int left_index, T key, node<T>* right) {
	int* temp_keys = new int[_order];
	void** temp_pointers = new void*[_order + 1];

	for (int i = 0, j = 0; i < _order; i++, j++) {
		if (j == left_index + 1)
			j++;
		temp_pointers[j] = old_node->pointers[i];
	}

	for (int i = 0, j = 0; i < _order - 1; i++, j++) {
		if (j == left_index)
			j++;
		temp_keys[j] = old_node->keys[i];
	}

	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;

	node<T>* new_node = new node<T> (false), *child;
	old_node->num_keys = 0;
	for (int i = 0; i < _order / 2 - 1; i++) {
		old_node->pointers[i] = temp_pointers[i];
		old_node->keys[i] = temp_keys[i];
		old_node->num_keys++;
	}
	old_node->pointers[_order / 2 - 1] = temp_pointers[_order / 2 - 1];
	int k_prime = temp_keys[_order / 2 - 1];
	for (int i = _order / 2, j = 0; i < _order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[_order / 2] = temp_pointers[_order];
	delete[] temp_pointers;
	delete[] temp_keys;
	new_node->parent = old_node->parent;
	for (int i = 0; i <= new_node->num_keys; i++) {
		child = (node<T>*) (new_node->pointers[i]);
		child->parent = new_node;
	}

	return insert_into_parent(old_node, k_prime, new_node);
}

template<typename T>
bool bPlusTree<T>::insert_into_parent(node<T>* left, T key, node<T>* right) {
	node<T>* parent = left->parent;
	if (parent == NULL) {
		node<T>* root = new node<T> (key, left, right);
		left->parent = root;
		right->parent = root;
		_root = root;
		return true;
	}

	int left_index = 0;
	while (parent->num_keys > left_index && parent->pointers[left_index]
			!= left)
		left_index++;

	if (parent->num_keys < _order - 1)
		return insert_into_node(parent, left_index, key, right);

	return insert_into_node_after_splitting(parent, left_index, key, right);
}

template<typename T>
bool bPlusTree<T>::insert(const T key, const T value) {
	if (find(key) != NULL)
		return false;
	record<T>* pointer = new record<T> (value);

	if (_root == NULL) {
		node<T>* root = new node<T> (true);
		root->keys[0] = key;
		root->pointers[0] = pointer;
		root->pointers[_order - 1] = NULL;
		root->num_keys++;
		_root = root;
		return true;
	}
	node<T>* leaf = search(key);

	if (leaf->num_keys < _order - 1) {
		return insert_into_leaf(leaf, key, pointer);
	}

	return insert_into_leaf_after_splitting(leaf, key, pointer);
}

// DELETION.

template<typename T>
bool bPlusTree<T>::remove(T key) {
	node<T>* key_leaf;
	record<T>* key_record;

	key_record = find(key);
	key_leaf = search(key);
	if (key_record != NULL && key_leaf != NULL && delete_entry(key_leaf, key,
			key_record)) {
		delete key_record;
		return true;
	}
	return false;
}

template<typename T>
bool bPlusTree<T>::delete_entry(node<T>* n, T key, void * pointer) {
	node<T>* neighbor;
	int neighbor_index, k_prime_index;
	T k_prime;
	int capacity;

	n = remove_entry_from_node(n, key, (node<T>*) pointer);
	if (n == _root)
		return adjust_root();

	if (n->num_keys >= _order / 2)
		return true;

	for (int i = 0; i <= n->parent->num_keys; i++) {
		if (n->parent->pointers[i] == n) {
			neighbor_index = i - 1;
			break;
		}
	}
	k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
	k_prime = n->parent->keys[k_prime_index];
	neighbor = neighbor_index == -1 ? (node<T>*) (n->parent->pointers[1])
			: (node<T>*) (n->parent->pointers[neighbor_index]);

	capacity = n->is_leaf ? _order : _order - 1;

	if (neighbor->num_keys + n->num_keys < capacity)
		return coalesce_nodes(n, neighbor, neighbor_index, k_prime);

	else
		return redistribute_nodes(n, neighbor, neighbor_index, k_prime_index,
				k_prime);
}

template<typename T>
node<T>* bPlusTree<T>::remove_entry_from_node(node<T>* n, int key,
		node<T>* pointer) {
	int i, num_pointers;
	i = 0;
	while (n->keys[i] != key)
		i++;
	for (++i; i < n->num_keys; i++)
		n->keys[i - 1] = n->keys[i];

	num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;

	i = 0;
	while (n->pointers[i] != pointer)
		i++;
	for (++i; i < num_pointers; i++)
		n->pointers[i - 1] = n->pointers[i];

	n->num_keys--;
	return n;
}

template<typename T>
bool bPlusTree<T>::adjust_root() {
	if (_root->num_keys > 0)
		return true;

	node<T>* new_root;
	if (!_root->is_leaf) {
		new_root = (node<T>*) (_root->pointers[0]);
		new_root->parent = NULL;
	} else {
		new_root = NULL;
	}
	delete _root;
	_root = new_root;
	return true;
}

template<typename T>
bool bPlusTree<T>::coalesce_nodes(node<T>* n, node<T>* neighbor,
		int neighbor_index, T k_prime) {

	int i, j, neighbor_insertion_index, n_end;
	node<T>* tmp;
	if (neighbor_index == -1) {
		tmp = n;
		n = neighbor;
		neighbor = tmp;
	}
	neighbor_insertion_index = neighbor->num_keys;
	if (!n->is_leaf) {
		neighbor->keys[neighbor_insertion_index] = k_prime;
		neighbor->num_keys++;

		n_end = n->num_keys;

		for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
			n->num_keys--;
		}
		neighbor->pointers[i] = n->pointers[j];
		for (i = 0; i < neighbor->num_keys + 1; i++) {
			tmp = (node<T>*) neighbor->pointers[i];
			tmp->parent = neighbor;
		}
	} else {
		for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
		}
		neighbor->pointers[_order - 1] = n->pointers[_order - 1];
	}

	delete_entry(n->parent, k_prime, n);
	delete n;
	return true;
}

template<typename T>
bool bPlusTree<T>::redistribute_nodes(node<T>* n, node<T>* neighbor,
		int neighbor_index, int k_prime_index, T k_prime) {
	int i;
	node<T>* tmp;
	if (neighbor_index != -1) {
		if (!n->is_leaf)
			n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
		for (i = n->num_keys; i > 0; i--) {
			n->keys[i] = n->keys[i - 1];
			n->pointers[i] = n->pointers[i - 1];
		}
		if (!n->is_leaf) {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys];
			tmp = (node<T>*) n->pointers[0];
			tmp->parent = n;
			neighbor->pointers[neighbor->num_keys] = NULL;
			n->keys[0] = k_prime;
			n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys
					- 1];
		} else {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			neighbor->pointers[neighbor->num_keys - 1] = NULL;
			n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			n->parent->keys[k_prime_index] = n->keys[0];
		}
	} else {
		if (n->is_leaf) {
			n->keys[n->num_keys] = neighbor->keys[0];
			n->pointers[n->num_keys] = neighbor->pointers[0];
			n->parent->keys[k_prime_index] = neighbor->keys[1];
		} else {
			n->keys[n->num_keys] = k_prime;
			n->pointers[n->num_keys + 1] = neighbor->pointers[0];
			tmp = (node<T>*) n->pointers[n->num_keys + 1];
			tmp->parent = n;
			n->parent->keys[k_prime_index] = neighbor->keys[0];
		}
		for (i = 0; i < neighbor->num_keys - 1; i++) {
			neighbor->keys[i] = neighbor->keys[i + 1];
			neighbor->pointers[i] = neighbor->pointers[i + 1];
		}
		if (!n->is_leaf)
			neighbor->pointers[i] = neighbor->pointers[i + 1];
	}
	n->num_keys++;
	neighbor->num_keys--;
	return true;
}

template<typename T>
void bPlusTree<T>::destroy_tree_nodes(node<T>* root) {
	int i;
	if (root->is_leaf)
		for (i = 0; i < root->num_keys; i++)
			delete (record<T>*) (root->pointers[i]);
	else
		for (i = 0; i < root->num_keys + 1; i++)
			destroy_tree_nodes((node<T>*) root->pointers[i]);
	delete root;
	_root = NULL;
}

template<typename T>
bPlusTree<T>::~bPlusTree() {
	destroy_tree_nodes(_root);
}

//// MAIN
//
//int main(int argc, char ** argv) {
//	bPlusTree<int>* bTree = new bPlusTree<int> ();
//	int input;
//	char instruction;
//
//	usage();
//	printf("> ");
//	while (scanf("%c", &instruction) != EOF) {
//		switch (instruction) {
//		case 'd':
//			scanf("%d", &input);
//			bTree->remove(input);
//			bTree->print_tree();
//			break;
//		case 'i':
//			scanf("%d", &input);
//			bTree->insert(input, input);
//			bTree->print_tree();
//			break;
//		case 't':
//			bTree->print_tree();
//			break;
//		case 'x':
//			if (!bTree->empty())
//				bTree->destroy_tree_nodes(bTree->root());
//			bTree->print_tree();
//			break;
//		case 'q':
//			while (getchar() != (int) '\n')
//				;
//			return EXIT_SUCCESS;
//		}
//		while (getchar() != (int) '\n')
//			;
//		printf("> ");
//	}
//	delete bTree;
//	printf("\n");
//
//	return EXIT_SUCCESS;
//}
