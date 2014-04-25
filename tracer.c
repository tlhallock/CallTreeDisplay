/*
 * tracer.c
 *
 *  Created on: Apr 11, 2014
 *      Author: thallock
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

static bool very_very_verbose = true;

struct _call_stack
{
	pthread_t tid;
	size_t stack_capacity;
	int stack_depth;
	void **stack;
	struct _call_stack *next;
};

typedef struct _call_stack call_stack;

static call_stack *all_call_stacks = NULL;

static call_stack *get_call_stack_for_current_thread()
{
	pthread_t tid = pthread_self();
	call_stack *stack = all_call_stacks;

	while (stack != NULL && stack->tid != tid)
	{
		stack = stack->next;
	}

	if (stack == NULL)
	{
		stack = (call_stack *) malloc (sizeof (*stack));
		stack->tid = tid;
		stack->stack_capacity = 0;
		stack->stack_depth = 0;
		stack->stack = NULL;

		// should be synchronized
		{
			stack->next = all_call_stacks;
			all_call_stacks = stack;
		}
	}

	return stack;
}

static void add_to_stack(void *fn, call_stack *cstack)
{
	if (cstack->stack_depth + 1 > cstack->stack_capacity)
	{
		if (cstack->stack_capacity == 0)
		{
			cstack->stack_capacity = 128;
			cstack->stack = (void **) malloc(sizeof (*cstack->stack) * cstack->stack_capacity);
		}
		else
		{
			cstack->stack_capacity = 3 * cstack->stack_capacity / 2;
			cstack->stack = (void **) realloc(cstack->stack, sizeof (*cstack->stack) * cstack->stack_capacity);
		}
	}

	if (very_very_verbose)
	{
		printf("tid %ld entering %p\n", cstack->tid, fn);
	}
	cstack->stack[cstack->stack_depth++] = fn;
}

static void remove_from_stack(void *fn, call_stack *cstack)
{
	if (cstack->stack_depth == 0)
	{
		if (very_very_verbose)
		{
			puts("no function to return to: exception must have been thrown");
		}
		return;
	}

	if (cstack->stack[cstack->stack_depth-1] != fn)
	{
		if (very_very_verbose)
		{
			puts("Exception must have been thrown.");
		}
		cstack->stack_depth = 0;
		return;
	}

	if (very_very_verbose)
	{
		printf("tid %ld leaving %p\n", cstack->tid, fn);
	}
	cstack->stack_depth--;
}

void set_very_very_verbose(bool on)
{
//	very_very_verbose = on;
}

void print_stack_trace(FILE *out)
{
	int i;
	call_stack *stack = get_call_stack_for_current_thread();
	pthread_t tid = pthread_self();

	fprintf(out, "pid[%ld] bt: start\n", tid);
	for(i=0; i<stack->stack_depth; i++)
	{
		fprintf(out, "pid[%ld] bt:[010%p]\n", tid, stack->stack[i]);
	}
	fprintf(out, "pid[%ld] bt: end\n", tid);
}

void __cyg_profile_func_enter(void *fn, void *from)
{
	add_to_stack(fn, get_call_stack_for_current_thread());
}

void __cyg_profile_func_exit(void *fn, void *from)
{
	remove_from_stack(fn, get_call_stack_for_current_thread());
}
