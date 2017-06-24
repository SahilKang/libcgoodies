/******************************************************************************
 * Copyright (C) 2017 Sahil Kang <sahil.kang@asilaycomputing.com>
 *
 * This file is part of libcgoodies.
 *
 * libcgoodies is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcgoodies is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libcgoodies.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <stdlib.h>
#include <tree_set.h>
#include <red_black_tree.h>
#include <stack.h>

struct TreeSet {
	struct rb_node *root; /* red-black tree which backs this set */
	int (*cmp)(void*, void*); /* comparison function */
	void (*dtor)(void*); /* optional destructor */

	struct rb_node *buf; /* node buffer */
	size_t buf_size;
	Stack *unused_nodes; /* contains empty nodes to be used again */
};

TreeSet* make_tree_set(
	size_t size,
	int (*cmp)(void*, void*),
	void (*dtor)(void*))
{
	size_t i;

	TreeSet* tree_set = malloc(sizeof(TreeSet));
	if (tree_set == NULL) return NULL;

	tree_set->root = NULL;
	tree_set->cmp = cmp;
	tree_set->dtor = dtor;
	tree_set->buf_size = size;

	tree_set->unused_nodes = make_stack(size, NULL);
	if (tree_set->unused_nodes == NULL) {
		free(tree_set);
		return NULL;
	}

	if (size != 0) {
		tree_set->buf = malloc(size * sizeof(struct rb_node));
		if (tree_set->buf == NULL) {
			free(tree_set->unused_nodes);
			free(tree_set);
			return NULL;
		}
	}

	for (i=0; i<size; ++i) {
		stack_push(tree_set->unused_nodes, &tree_set->buf[i]);
	}

	return tree_set;
}

void free_tree_set(TreeSet *tree_set)
{
	if (tree_set->buf != NULL) {
		if (tree_set->dtor != NULL) {
			size_t i; /* call dtor on elements */
			for (i=0; i<tree_set->buf_size; ++i) {
				(*(tree_set->dtor))(tree_set->buf[i].key);
			}
		}

		free(tree_set->buf);
	}

	stack_free(tree_set->unused_nodes);
	free(tree_set);
}

int tree_set_insert(TreeSet *tree_set, void *elem)
{
	struct rb_node *node = stack_pop(tree_set->unused_nodes);

	if (node == NULL) { /* need to realloc more memory */
		size_t i;
		size_t const old_size = tree_set->buf_size;
		size_t const new_size = 2 * old_size;
		struct rb_node * const old_ptr = tree_set->buf;

		struct rb_node *buf =realloc(tree_set->buf,
			new_size * sizeof(struct rb_node));
		if (buf == NULL) return 0; /* TODO signal error here */

		tree_set->buf = buf;
		tree_set->buf_size = new_size;

		/* realloc moved the array so we have to update
		 * the parent, left, and right pointers of the
		 * currently used nodes.
		 */
		if (tree_set->buf != old_ptr) {
			ptrdiff_t diff;
			struct rb_node n;

			for (i=0; i<old_size; ++i) {
				n = tree_set->buf[i];

				if (n.parent != NULL) {
					diff = n.parent - old_ptr;
					n.parent = &tree_set->buf[diff];
				}

				if (n.left != NULL) {
					diff = n.left - old_ptr;
					n.left = &tree_set->buf[diff];
				}

				if (n.right != NULL) {
					diff = n.right - old_ptr;
					n.right = &tree_set->buf[diff];
				}
			}
		}

		for (i=old_size; i<new_size-1; ++i) {
			if(!stack_push(tree_set->unused_nodes, &buf[i])) {
				return 0; /* TODO signal error here */
			}
		}

		node = &tree_set->buf[new_size-1];
	}

	node->key = elem;
	return rb_tree_insert(&(tree_set->root), node, tree_set->cmp);
}

int tree_set_remove(TreeSet *tree_set, void *elem)
{
	struct rb_node *node =
		rb_tree_search(tree_set->root, elem, tree_set->cmp);

	if (node == NULL) return 0; /* elem does not exist in tree_set */

	rb_tree_delete(&tree_set->root, node);
	stack_push(tree_set->unused_nodes, node); /* TODO handle error */
	return 1;
}

void* tree_set_min(TreeSet *tree_set)
{
	struct rb_node * const node = rb_tree_min(tree_set->root);
	return node == NULL ? NULL : node->key;
}
