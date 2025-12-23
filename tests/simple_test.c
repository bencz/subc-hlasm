/* Simple test to debug frame layout */
int simple(int a, int b) {
    int x;
    int y;
    x = a + 1;
    y = b + 2;
    return x + y;
}

int main(void) {
    return simple(10, 20);
}
