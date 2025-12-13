/*
 * Test 02: Variables and assignments
 * Tests: global/local variables, int/char types
 */

int global_int;
char global_char;

int main() {
	int local_int;
	char local_char;
	
	global_int = 42;
	global_char = 'A';
	local_int = 100;
	local_char = 'Z';
	
	return global_int + local_int;
}
