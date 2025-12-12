/*
 * Test 23: Simulated linked list using arrays
 * Tests: complex pointer/array manipulation
 */

int data[10];
int next[10];
int head;
int free_idx;

void init() {
	int i;
	head = -1;
	free_idx = 0;
	for (i = 0; i < 10; i = i + 1) {
		next[i] = -1;
	}
}

int alloc_node() {
	int idx;
	if (free_idx >= 10) return -1;
	idx = free_idx;
	free_idx = free_idx + 1;
	return idx;
}

void push(int val) {
	int idx;
	idx = alloc_node();
	if (idx >= 0) {
		data[idx] = val;
		next[idx] = head;
		head = idx;
	}
}

int sum_list() {
	int sum, curr;
	sum = 0;
	curr = head;
	while (curr >= 0) {
		sum = sum + data[curr];
		curr = next[curr];
	}
	return sum;
}

int main() {
	init();
	push(10);
	push(20);
	push(30);
	push(40);
	return sum_list();  /* 100 */
}
