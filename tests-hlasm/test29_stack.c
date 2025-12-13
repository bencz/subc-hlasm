/*
 * Test 29: Stack implementation
 * Tests: array-based stack, push/pop operations
 */

int stack[20];
int sp;

void stack_init() {
	sp = 0;
}

int stack_empty() {
	return sp == 0;
}

int stack_full() {
	return sp >= 20;
}

void push(int val) {
	if (!stack_full()) {
		stack[sp] = val;
		sp = sp + 1;
	}
}

int pop() {
	if (!stack_empty()) {
		sp = sp - 1;
		return stack[sp];
	}
	return -1;
}

int peek() {
	if (!stack_empty()) {
		return stack[sp - 1];
	}
	return -1;
}

int main() {
	int sum;
	
	stack_init();
	
	push(10);
	push(20);
	push(30);
	
	sum = 0;
	while (!stack_empty()) {
		sum = sum + pop();
	}
	
	return sum;  /* 60 */
}
