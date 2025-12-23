/* Test file for typed function pointers */

/* Simple function pointer types */
struct test_vtable {
    void (*fn_void)(void);
    void (*fn_int)(int n);
    void (*fn_str)(char *s);
    void (*fn_two)(int a, int b);
    int  (*fn_ret)(void);
    char *(*fn_retstr)(char *s);
};

/* Test functions to be called via pointers */
void test_void(void) {
}

void test_int(int n) {
}

void test_str(char *s) {
}

void test_two(int a, int b) {
}

int test_ret(void) {
    return 42;
}

char *test_retstr(char *s) {
    return s;
}

/* Global vtable instance */
struct test_vtable vtable;

/* Test calling functions via vtable */
int test_calls(void) {
    int result;
    
    /* Initialize vtable */
    vtable.fn_void = test_void;
    vtable.fn_int = test_int;
    vtable.fn_str = test_str;
    vtable.fn_two = test_two;
    vtable.fn_ret = test_ret;
    vtable.fn_retstr = test_retstr;
    
    /* Call via vtable */
    vtable.fn_void();
    vtable.fn_int(10);
    vtable.fn_str("hello");
    vtable.fn_two(1, 2);
    result = vtable.fn_ret();
    /* Note: SubC treats all fn ptrs as returning int, so we cast */
    vtable.fn_retstr("world");
    
    return result;
}

int main(void) {
    return test_calls() - 42;  /* Should return 0 if correct */
}
