/*
 * Test 20: Complex program - Fibonacci
 * Tests: combination of features
 */

int fib[20];

int fibonacci(int n) {
	int i;
	
	fib[0] = 0;
	fib[1] = 1;
	
	for (i = 2; i <= n; i = i + 1) {
		fib[i] = fib[i-1] + fib[i-2];
	}
	
	return fib[n];
}

int main() {
	return fibonacci(10);  /* 55 */
}
