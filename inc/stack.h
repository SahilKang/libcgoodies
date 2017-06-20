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

#ifndef STACK_HEADER
#define STACK_HEADER

/* an ever-expanding stack */

#include <stddef.h>

typedef struct Stack Stack;

Stack* make_stack(size_t size, void (*dtor)(void*));
void stack_free(Stack *stack);

size_t stack_size(Stack *stack);
int stack_push(Stack *stack, void *elem);
void* stack_pop(Stack *stack);

#endif
