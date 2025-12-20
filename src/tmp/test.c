/* string global */
char globalStr[] = "abc";

/* funcao que soma os valores de um array */
int somaArray(int arr[], int tamanho) {
    int i;
    int soma = 0;

    for (i = 0; i < tamanho; i++) {
        soma += arr[i];
    }

    return soma;
}

/* funcao que soma todos os elementos de uma matriz 2x2 */
int somaMatriz(int m[2][2]) {
    int i;
    int j;
    int soma = 0;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            soma += m[i][j];
        }
    }

    return soma;
}

/* funcao que verifica tamanho de string simples */
int tamanhoString(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

int main(void) {
    /* string local */
    char localStr[] = "abcd";

    int array[3];
    int matriz[2][2];

    int r1;
    int r2;
    int r3;
    int r4;

    array[0] = 1;
    array[1] = 2;
    array[2] = 3;

    matriz[0][0] = 1;
    matriz[0][1] = 2;
    matriz[1][0] = 3;
    matriz[1][1] = 4;

    r1 = somaArray(array, 3);
    r2 = somaMatriz(matriz);
    r3 = tamanhoString(globalStr);
    r4 = tamanhoString(localStr);

    if (r1 != 6) return 1;
    if (r2 != 10) return 2;
    if (r3 != 3) return 3;
    if (r4 != 4) return 4;

    return 0;
}
