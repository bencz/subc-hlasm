/*
 * Test 24: Bubble sort
 * Tests: nested loops, array manipulation, swapping
 */

int arr[10];

void swap(int *a, int *b) {
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void bubble_sort(int n) {
	int i, j;
	for (i = 0; i < n - 1; i = i + 1) {
		for (j = 0; j < n - i - 1; j = j + 1) {
			if (arr[j] > arr[j + 1]) {
				swap(&arr[j], &arr[j + 1]);
			}
		}
	}
}

int main() {
	arr[0] = 64;
	arr[1] = 34;
	arr[2] = 25;
	arr[3] = 12;
	arr[4] = 22;
	arr[5] = 11;
	arr[6] = 90;
	arr[7] = 5;
	arr[8] = 77;
	arr[9] = 1;
	
	bubble_sort(10);
	
	/* Return first (smallest) element */
	return arr[0];  /* 1 */
}
