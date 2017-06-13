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

#include <stddef.h>
#include <red_black_tree.h>

static void insert_fixup(struct rb_node **root, struct rb_node *x);
static void delete_fixup(struct rb_node **root, struct rb_node *x);

static void left_rotate(struct rb_node **root, struct rb_node *x);
static void right_rotate(struct rb_node **root, struct rb_node *y);

static void transplant(
	struct rb_node **root,
	struct rb_node *old,
	struct rb_node *new);

struct rb_node* rb_tree_search(
	struct rb_node *root,
	void *key,
	int (*cmp)(void*, void*))
{
	int val;

	while (root != NULL) {
		val = (*cmp)(key, root->key);

		if (val < 0) root = root->left;
		else if (val > 0) root = root->right;
		else break;
	}

	return root;
}

struct rb_node* rb_tree_min(struct rb_node *root)
{
	if (root == NULL) return NULL;

	while (root->left != NULL) root = root->left;
	return root;
}

struct rb_node* rb_tree_max(struct rb_node *root)
{
	if (root == NULL) return NULL;

	while (root->right != NULL) root = root->right;
	return root;
}

struct rb_node* tree_prev(struct rb_node *root)
{
	if (root == NULL) return NULL;

	if (root->left != NULL) {
		return rb_tree_max(root->left);
	} else {
		struct rb_node *trailing = root->parent;

		while (trailing != NULL && root == trailing->left) {
			root = trailing;
			trailing = trailing->parent;
		}

		return trailing;
	}
}

struct rb_node* tree_next(struct rb_node *root)
{
	if (root == NULL) return NULL;

	if (root->right != NULL) {
		return rb_tree_min(root->right);
	} else {
		struct rb_node *trailing = root->parent;

		while (trailing != NULL && root == trailing->right) {
			root = trailing;
			trailing = trailing->parent;
		}

		return trailing;
	}
}

int rb_tree_insert(
	struct rb_node **root,
	struct rb_node *node,
	int (*cmp)(void*, void*))
{
	int val;

	struct rb_node *trailing = NULL;
	struct rb_node *cur = *root;

	while (cur != NULL) {
		trailing = cur;
		val = (*cmp)(node->key, cur->key);

		if (val < 0) {
			cur = cur->left;
		} else if (val > 0) {
			cur = cur->right;
		} else {
			return 0; /* node already exists in tree, no insert */
		}
	}

	/*
	 * at this point, the node does not exist in the tree (val != 0),
	 * and val equals (*cmp)(node->key, trailing->key)
	 */

	 if (trailing == NULL) {
	 	*root = node; /* tree was empty */
	 } else if (val < 0) {
	 	trailing->left = node;
	 } else {
	 	trailing->right = node;
	 }

	 node->parent = trailing;
	 node->left = NULL;
	 node->right = NULL;
	 node->color = RED;

	 insert_fixup(root, node);

	 return 1;
}

static void insert_fixup(struct rb_node **root, struct rb_node *x)
{
	struct rb_node *uncle = NULL;

	while (x != *root && x->parent->color == RED) {
		if (x->parent == x->parent->parent->left) {
			uncle = x->parent->parent->right;

			if (uncle != NULL && uncle->color == RED) { /*case 1*/
				x->parent->color = BLACK;
				uncle->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right) { /* case 2 */
					x = x->parent;
					left_rotate(root, x);
				}

				/* case 3 */
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				right_rotate(root, x->parent->parent);
			}
		} else { /* uncle must be the left child */
			uncle = x->parent->parent->left;

			if (uncle != NULL && uncle->color == RED) { /*case 1*/
				x->parent->color = BLACK;
				uncle->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left) { /* case 2 */
					x = x->parent;
					right_rotate(root, x);
				}

				/* case 3 */
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				left_rotate(root, x->parent->parent);
			}
		}
	}

	(*root)->color = BLACK;
}

static void left_rotate(struct rb_node **root, struct rb_node *x)
{
	struct rb_node *y = x->right;

	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}

	y->parent = x->parent;
	if (x->parent == NULL) {
		*root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;
}

static void right_rotate(struct rb_node **root, struct rb_node *y)
{
	struct rb_node *x = y->left;

	y->left = x->right;
	if (x->right != NULL) {
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == NULL) {
		*root = x;
	} else if (y == y->parent->left) {
		y->parent->left = x;
	} else {
		y->parent->right = x;
	}

	x->right = y;
	y->parent = x;
}

static void transplant(
	struct rb_node **root,
	struct rb_node *old,
	struct rb_node *new)
{
	if (old->parent == NULL) {
		*root = new;
	} else if (old == old->parent->left) {
		old->parent->left = new;
	} else {
		old->parent->right = new;
	}

	if (new != NULL) {
		new->parent = old->parent;
	}
}

void rb_tree_delete(struct rb_node **root, struct rb_node *node)
{
	struct rb_node *z = NULL;
	struct rb_node *y = node;
	int original_color = y->color;

	if (node->left == NULL) {
		z = node->right;
		transplant(root, node, node->right);
	} else if (node->right == NULL) {
		z = node->left;
		transplant(root, node, node->left);
	} else {
		y = rb_tree_min(node->right);
		original_color = y->color;

		z = y->right;
		if (y->parent == node) {
			z->parent = y;
		} else {
			transplant(root, y, y->right);
			y->right = node->right;
			y->right->parent = y;
		}

		transplant(root, node, y);
		y->left = node->left;
		y->left->parent = y;
		y->color = node->color;
	}

	if (z != NULL && original_color == BLACK) {
		delete_fixup(root, z);
	}
}

static void delete_fixup(struct rb_node **root, struct rb_node *x)
{
	struct rb_node *sibling = NULL;

	while (x != *root && x->color == BLACK) {
		if (x == x->parent->left) {
			sibling = x->parent->right;

			if (sibling->color == RED) { /* case 1 */
				sibling->color = BLACK;
				x->parent->color = RED;
				left_rotate(root, x->parent);
				sibling = x->parent->right;
			}

			if (sibling->left->color == BLACK
				&& sibling->right->color == BLACK) { /*case 2*/
				sibling->color = RED;
				x = x->parent;
			} else {
				if (sibling->right->color == BLACK) {/*case 3*/
					sibling->left->color = BLACK;
					sibling->color = RED;
					right_rotate(root, sibling);
					sibling = x->parent->right;
				}

				/* case 4 */
				sibling->color = x->parent->color;
				x->parent->color = BLACK;
				sibling->right->color = BLACK;
				left_rotate(root, x->parent);
				x = *root;
			}
		} else { /* sibling must be the left child */
			sibling = x->parent->left;

			if (sibling->color == RED) { /* case 1 */
				sibling->color = BLACK;
				x->parent->color = RED;
				right_rotate(root, x->parent);
				sibling = x->parent->left;
			}

			if (sibling->left->color == BLACK
				&& sibling->right->color == BLACK) { /*case 2*/
				sibling->color = RED;
				x = x->parent;
			} else {
				if (sibling->left->color == BLACK) {/*case 3*/
					sibling->right->color = BLACK;
					sibling->color = RED;
					left_rotate(root, sibling);
					sibling = x->parent->left;
				}

				/* case 4 */
				sibling->color = x->parent->color;
				x->parent->color = BLACK;
				sibling->left->color = BLACK;
				right_rotate(root, x->parent);
				x = *root;
			}
		}
	}

	x->color = BLACK;
}
