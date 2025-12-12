/*
 * Test 13: Arrays
 * Tests: array declaration, indexing
 */

int arr[10];

int main() {
	int i, sum;
	int local[5];
	
	/* Initialize global array */
	for (i = 0; i < 10; i = i + 1) {
		arr[i] = i * 2;
	}
	
	/* Initialize local array */
	for (i = 0; i < 5; i = i + 1) {
		local[i] = i + 1;
	}
	
	/* Sum arrays */
	sum = 0;
	for (i = 0; i < 10; i = i + 1) {
		sum = sum + arr[i];
	}
	for (i = 0; i < 5; i = i + 1) {
		sum = sum + local[i];
	}
	
	return sum;  /* 90 + 15 = 105 */
}
