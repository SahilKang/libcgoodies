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

#ifndef TREE_SET_HEADER
#define TREE_SET_HEADER

typedef struct TreeSet TreeSet;

TreeSet* make_tree_set(size_t size, int (*cmp)(void*, void*));
void free_tree_set(TreeSet *tree_set);

int tree_set_insert(TreeSet *tree_set, void *elem);
int tree_set_remove(TreeSet *tree_set, void *elem);

void* tree_set_min(TreeSet *tree_set);
void* tree_set_max(TreeSet *tree_set);

size_t tree_set_size(TreeSet *tree_set);

int tree_set_contains(TreeSet *tree_set);

#endif
