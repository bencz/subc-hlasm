/*
 * Test 11: Function calls
 * Tests: function definition, parameters, return
 */

int add(int a, int b) {
	return a + b;
}

int multiply(int a, int b) {
	return a * b;
}

int main() {
	int x, y, z;
	
	x = add(10, 20);
	y = multiply(5, 6);
	z = add(x, y);
	
	return z;  /* 60 */
}
