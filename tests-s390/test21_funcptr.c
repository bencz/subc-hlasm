/*
 * Test 21: Function pointers
 * Tests: pointers to functions
 */

int add(int a, int b) {
	return a + b;
}

int sub(int a, int b) {
	return a - b;
}

int apply(int (*fn)(), int x, int y) {
	return fn(x, y);
}

int main() {
	int r1, r2;
	
	r1 = apply(add, 10, 5);  /* 15 */
	r2 = apply(sub, 10, 5);  /* 5 */
	
	return r1 + r2;  /* 20 */
}
