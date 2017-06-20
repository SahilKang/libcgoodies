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
#include <stack.h>

struct Stack {
	size_t sp; /* stack pointer */
	void (*dtor)(void*); /* optional destructor */

	void **buf;
	size_t buf_size;
};

Stack* make_stack(size_t size, void (*dtor)(void*))
{
	Stack *stack = malloc(sizeof(Stack));
	if (stack == NULL) return NULL;

	stack->sp = 0;
	stack->dtor = dtor;
	stack->buf_size = size;

	if (size != 0) {
		stack->buf = malloc(size * sizeof(void*));
		if (stack->buf == NULL) {
			free(stack);
			return NULL;
		}
	}

	return stack;
}

void stack_free(Stack *stack)
{
	if (stack->buf != NULL) {
		if (stack->dtor != NULL) { /* call dtor on elements */
			size_t i;
			for (i=0; i<stack->sp; ++i) {
				(*(stack->dtor))(stack->buf[i]);
			}
		}

		free(stack->buf);
	}

	free(stack);
}

size_t stack_size(Stack *stack)
{
	return stack->sp;
}

int stack_push(Stack *stack, void *elem)
{
	if (stack->buf_size == 0) {
		stack->buf_size = 1;
		stack->buf = malloc(sizeof(void*));
		if (stack->buf == NULL) return 0;
	} else if (stack->sp == stack->buf_size) { /* expand stack */
		void **buf2;
		buf2 = realloc(stack->buf, 2*stack->buf_size * sizeof(void*));
		if (buf2 == NULL) {
			return 0;
		} else {
			stack->buf_size *= 2;
			stack->buf = buf2;
		}
	}

	stack->buf[stack->sp++] = elem;
	return 1;
}

void* stack_pop(Stack *stack)
{
	if (stack->sp == 0) {
		return NULL;
	} else {
		return stack->buf[--stack->sp];
	}
}
