/* Simple test for C89 types */

short global_short;
unsigned short global_ushort;
long global_long;
unsigned long global_ulong;
unsigned int global_uint;

short add_shorts(short a, short b) {
    return a + b;
}

long add_longs(long a, long b) {
    return a + b;
}

unsigned int add_uints(unsigned int a, unsigned int b) {
    return a + b;
}

int main(void) {
    short local_short;
    unsigned short local_ushort;
    long local_long;
    unsigned long local_ulong;
    
    local_short = 100;
    local_ushort = 200;
    local_long = 100000;
    local_ulong = 200000;
    
    global_short = add_shorts(10, 20);
    global_long = add_longs(1000, 2000);
    global_uint = add_uints(500, 600);
    
    return 0;
}
