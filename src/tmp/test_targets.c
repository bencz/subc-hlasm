int soma(int a, int b) {
    return a + b;
}

int main(void) {
    int x;
    int y;
    int z;
    
    x = 10;
    y = 20;
    z = soma(x, y);
    
    if (z != 30) return 1;
    return 0;
}
