/*
 * Test 22: Simulated 2D array using 1D
 * Tests: array indexing with calculations
 */

int matrix[16];  /* 4x4 matrix */

int get(int row, int col) {
	return matrix[row * 4 + col];
}

void set(int row, int col, int val) {
	matrix[row * 4 + col] = val;
}

int main() {
	int i, j, sum;
	
	/* Initialize matrix */
	for (i = 0; i < 4; i = i + 1) {
		for (j = 0; j < 4; j = j + 1) {
			set(i, j, i * 4 + j);
		}
	}
	
	/* Sum diagonal */
	sum = 0;
	for (i = 0; i < 4; i = i + 1) {
		sum = sum + get(i, i);
	}
	
	return sum;  /* 0 + 5 + 10 + 15 = 30 */
}
