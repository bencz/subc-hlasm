/*
 * Test 25: Binary search
 * Tests: divide and conquer algorithm
 */

int arr[10];

int binary_search(int key, int low, int high) {
	int mid;
	
	if (low > high)
		return -1;
	
	mid = (low + high) / 2;
	
	if (arr[mid] == key)
		return mid;
	
	if (arr[mid] > key)
		return binary_search(key, low, mid - 1);
	
	return binary_search(key, mid + 1, high);
}

int main() {
	/* Sorted array */
	arr[0] = 2;
	arr[1] = 5;
	arr[2] = 8;
	arr[3] = 12;
	arr[4] = 16;
	arr[5] = 23;
	arr[6] = 38;
	arr[7] = 56;
	arr[8] = 72;
	arr[9] = 91;
	
	return binary_search(23, 0, 9);  /* 5 */
}
