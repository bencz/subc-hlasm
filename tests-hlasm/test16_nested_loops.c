/*
 * Test 16: Nested loops
 * Tests: nested for loops
 */

int main() {
	int i, j, sum;
	
	sum = 0;
	
	for (i = 0; i < 5; i = i + 1) {
		for (j = 0; j < 5; j = j + 1) {
			sum = sum + 1;
		}
	}
	
	return sum;  /* 25 */
}
