/* string global */
char globalStr[] = "abc";

/* funcao que soma os valores de um array */
int somaArray(int arr[], int tamanho) {
    int i;
    int soma;
    
    soma = 0;
    for (i = 0; i < tamanho; i = i + 1) {
        soma = soma + arr[i];
    }

    return soma;
}

/* funcao que verifica tamanho de string simples */
int tamanhoString(char *s) {
    int i;
    i = 0;
    while (s[i] != '\0') {
        i = i + 1;
    }
    return i;
}

int main(void) {
    int array[3];
    int r1;
    int r3;

    array[0] = 1;
    array[1] = 2;
    array[2] = 3;

    r1 = somaArray(array, 3);
    r3 = tamanhoString(globalStr);

    if (r1 != 6) return 1;
    if (r3 != 3) return 3;

    return 0;
}
