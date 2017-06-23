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

		struct rb_node *buf =realloc(tree_set->buf,
			new_size * sizeof(struct rb_node));
		if (buf == NULL) return 0; /* TODO signal error here */

		tree_set->buf = buf;
		tree_set->buf_size = new_size;

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
