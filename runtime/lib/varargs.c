/*
 *	NMH's Simple C Compiler, 2014
 *	_va_start(), _va_arg(), _va_end()
 */

#include <varargs.h>

/* Expect address of last known argument, return vararg pointer */

void **_va_start(void *last) {
	return (void **) last + 1;
}

/*
 * Extract vararg, advance pointer.
 * 
 * Usage: val = (int) _va_arg(&ap);
 * 
 * ap is a void* pointing to the current argument on the stack.
 * &ap is passed to this function, so we receive void**.
 * We need to:
 *   1. Read the value at *ap (the current argument)
 *   2. Advance *ap to point to the next argument
 */
void *_va_arg(void **ap) {
	void *result;
	void **ptr;
	
	ptr = (void **) *ap;  /* ptr points to current arg on stack */
	result = *ptr;        /* get the argument value */
	*ap = (void *)(ptr + 1);  /* advance to next argument */
	return result;
}

void _va_end(void **ap) {}
