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

#ifndef RED_BLACK_TREE_HEADER
#define RED_BLACK_TREE_HEADER

/* red-black tree containing only unique values */

struct rb_node {
	void *key;
	enum { RED, BLACK } color;

	struct rb_node *parent;
	struct rb_node *left;
	struct rb_node *right;
};

struct rb_node* rb_tree_search(
	struct rb_node *root,
	void *key,
	int (*cmp)(void*, void*));

struct rb_node* rb_tree_min(struct rb_node *root);
struct rb_node* rb_tree_max(struct rb_node *root);

struct rb_node* rb_tree_prev(struct rb_node *root);
struct rb_node* rb_tree_next(struct rb_node *root);

int rb_tree_insert(
	struct rb_node **root,
	struct rb_node *node,
	int (*cmp)(void*, void*));

void rb_tree_delete(struct rb_node **root, struct rb_node *node);

#endif
