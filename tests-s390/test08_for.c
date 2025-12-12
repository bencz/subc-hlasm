/*
 * Test 08: For loop
 * Tests: for statement
 */

int main() {
	int i, sum;
	
	sum = 0;
	
	for (i = 1; i <= 10; i = i + 1) {
		sum = sum + i;
	}
	
	return sum;  /* 55 */
}
