#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


typedef struct Node {
	char *key;
	void *value;
	struct Node *parent;
	struct Node *left;
	struct Node *right;
	pthread_mutex_t lock;
} Node;


typedef struct Tree {
	int count;
	struct Node *root;
	pthread_mutex_t lock;
} Tree;


void free_node(Node *node)
{
	pthread_mutex_destroy(&node->lock);
	free(node->key);
	free(node);
}


Node *create_node(char *key, void *value)
{
	int s;
	Node *node;
	size_t key_len;

	if (key == NULL || value == NULL) {
		return NULL;
	}

	key_len = strlen(key);

	node = calloc(1, sizeof(Node));
	if (node == NULL) {
		return NULL;
	}

	node->key = calloc(1, key_len + 1);
	if (node->key == NULL) {
		free(node);
		return NULL;
	}

	strncpy(node->key, key, key_len);

	node->value = value;

	s = pthread_mutex_init(&node->lock, NULL);
	if (s != 0) {
		free(node->key);
		free(node);
		return NULL;
	}

	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return node;
}


Boolean lookup(Tree *tree, char *key, void **value)
{
	int cmp;
	Node *node;
	Node *prev;

	node = tree->root;

	while (node != NULL) {
		pthread_mutex_lock(&node->lock);

		prev = node;

		cmp = strcmp(key, node->key);
		if (cmp < 0) {
			node = node->left;
		} else if (cmp > 0) {
			node = node->right;
		} else {
			*value = node->value;
			pthread_mutex_unlock(&node->lock);
			return TRUE;
		}

		pthread_mutex_unlock(&prev->lock);
	}

	return FALSE;
}


Node *find_last(Tree *tree, char *key)
{
	int cmp;
	Node *node;
	Node *prev;

	node = tree->root;

	while (node != NULL) {
		pthread_mutex_lock(&node->lock);

		prev = node;

		cmp = strcmp(key, node->key);
		if (cmp < 0) {
			node = node->left;
		} else if (node > 0) {
			node = node->right;
		} else {
			pthread_mutex_unlock(&node->lock);
			return node;
		}

		pthread_mutex_unlock(&prev->lock);
	}

	return prev;
}


Node *add_child(Tree *tree, char *key, void *value)
{
	int cmp;
	Node *new;
	Node *last;

	new = create_node(key, value);

	if (new == NULL) {
		return NULL;
	}

	pthread_mutex_lock(&new->lock);

	// empty tree
	if (tree->root == NULL) {
		pthread_mutex_lock(&tree->lock);
		tree->root = new;
		pthread_mutex_unlock(&tree->lock);
		pthread_mutex_unlock(&new->lock);
		return tree->root;
	}

	last = find_last(tree, key);

	pthread_mutex_lock(&last->lock);

	cmp = strcmp(key, last->key);
	if (cmp < 0) {
		last->left = new;
	} else if (cmp > 0) {
		last->right = new;
	} else {
		// dupicate key
		pthread_mutex_unlock(&new->lock);
		free_node(new);
		pthread_mutex_unlock(&last->lock);
		return NULL;
	}

	pthread_mutex_unlock(&last->lock);

	new->parent = last;

	pthread_mutex_lock(&tree->lock);
	tree->count++;
	pthread_mutex_unlock(&tree->lock);

	pthread_mutex_unlock(&new->lock);

	return new;
}


void splice(Tree *tree, Node *node)
{
	Node *orphan;

	if (node->left != NULL) {
		orphan = node->left;
	} else {
		orphan = node->right;
	}

	if (node == tree->root) {
		pthread_mutex_lock(&tree->lock);
		tree->root = orphan;
		pthread_mutex_unlock(&tree->lock);
	} else {
		pthread_mutex_lock(&node->parent->lock);
		if (node->parent->left == node) {
			node->parent->left = orphan;
		} else {
			node->parent->right = orphan;
		}

		pthread_mutex_unlock(&node->parent->lock);
	}

	if (orphan != NULL) {
		pthread_mutex_lock(&orphan->lock);
		orphan->parent = node->parent;
		pthread_mutex_unlock(&orphan->lock);
	}

	pthread_mutex_unlock(&node->lock);
	free_node(node);

	pthread_mutex_lock(&tree->lock);
	tree->count--;
	pthread_mutex_unlock(&tree->lock);
}


int delete_node(Tree *tree, char *key)
{
	int cmp;
	Node *node;
	Node *prev;
	Node *smallest;

	node = tree->root;
	if (node == NULL) {
		return -1;
	}

	while (node != NULL) {
		pthread_mutex_lock(&node->lock);

		prev = node;

		cmp = strcmp(key, node->key);
		if (cmp < 0) {
			node = node->left;
		} else if (cmp > 0) {
			node = node->right;
		} else {
			// node found. keep lock on it
			break;
		}

		pthread_mutex_unlock(&prev->lock);
	}

	// node not found
	if (node == NULL) {
		return -1;
	}

	if (node->left == NULL || node->right == NULL) {
		splice(tree, node);
	} else {
		// replace the value of the deleted node with the
		// node of the smallest value in the subtree

		smallest = node->right;
		while (smallest->left != NULL) {
			smallest = smallest->left;
		}

		if (node->parent->left == node) {
			node->parent->left = smallest;
		} else {
			node->parent->right = smallest;
		}

		smallest->parent = node->parent;

		free_node(node);
	}

	return 0;
}


void initialize_tree(Tree *tree)
{
	tree->count = 0;
	tree->root = NULL;
	pthread_mutex_init(&tree->lock, NULL);
}


int main(int argc, char *argv[])
{
	int value = 0;
	Tree tree;

	initialize_tree(&tree);

	add_child(&tree, "1", (void *)1);
	add_child(&tree, "2", (void *)2);
	add_child(&tree, "3", (void *)3);
	add_child(&tree, "4", (void *)4);
	add_child(&tree, "5", (void *)5);
	add_child(&tree, "6", (void *)6);
	add_child(&tree, "7", (void *)7);

	if (lookup(&tree, "1", (void **)&value)) {
		printf("1: %d\n", value);
	} else {
		printf("'1' not found\n");
	}

	if (lookup(&tree, "2", (void **)&value)) {
		printf("2: %d\n", value);
	} else {
		printf("'2' not found\n");
	}

	if (lookup(&tree, "3", (void **)&value)) {
		printf("3: %d\n", value);
	} else {
		printf("'3' not found\n");
	}

	if (lookup(&tree, "4", (void **)&value)) {
		printf("4: %d\n", value);
	} else {
		printf("'4' not found\n");
	}

	if (lookup(&tree, "5", (void **)&value)) {
		printf("5: %d\n", value);
	} else {
		printf("'5' not found\n");
	}

	if (lookup(&tree, "6", (void **)&value)) {
		printf("6: %d\n", value);
	} else {
		printf("'6' not found\n");
	}

	if (lookup(&tree, "7", (void **)&value)) {
		printf("7: %d\n", value);
	} else {
		printf("'7' not found\n");
	}

	delete_node(&tree, "1");
	delete_node(&tree, "2");
	delete_node(&tree, "3");
	delete_node(&tree, "4");
	delete_node(&tree, "5");
	delete_node(&tree, "6");
	delete_node(&tree, "7");

	if (lookup(&tree, "1", (void **)&value)) {
		printf("1: %d\n", value);
	} else {
		printf("'1' not found\n");
	}

	if (lookup(&tree, "2", (void **)&value)) {
		printf("2: %d\n", value);
	} else {
		printf("'2' not found\n");
	}

	if (lookup(&tree, "3", (void **)&value)) {
		printf("3: %d\n", value);
	} else {
		printf("'3' not found\n");
	}

	if (lookup(&tree, "4", (void **)&value)) {
		printf("4: %d\n", value);
	} else {
		printf("'4' not found\n");
	}

	if (lookup(&tree, "5", (void **)&value)) {
		printf("5: %d\n", value);
	} else {
		printf("'5' not found\n");
	}

	if (lookup(&tree, "6", (void **)&value)) {
		printf("6: %d\n", value);
	} else {
		printf("'6' not found\n");
	}

	if (lookup(&tree, "7", (void **)&value)) {
		printf("7: %d\n", value);
	} else {
		printf("'7' not found\n");
	}

	exit(EXIT_SUCCESS);
}
