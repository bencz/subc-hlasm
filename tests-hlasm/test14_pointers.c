/*
 * Test 14: Pointers
 * Tests: pointer declaration, dereference, address-of
 */

int global_val;

int main() {
	int x;
	int *p;
	
	x = 42;
	p = &x;
	
	*p = 100;
	
	global_val = *p;
	
	return x;  /* 100 */
}
