#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define bias 8
#define bitsInByte 8

void printaString(int *bits, int size)
{
    for (int i = size - 1; i >= 0; i--)
        printf("%d ", bits[i]);
    printf("\n");
}

void geraBitsControle(int *bits, int size_in, int size_ham)
{
    for (int i = 1; i <= size_in; i = i << 1)
    {
        bits[i] = 0;
        for (int j = 3; j < size_ham; j++)
            if (i & j)
                bits[i] = bits[i] ^ bits[j];
    }
}

void geraBitG(int *ham, int size_ham)
{
    ham[0] = 0;
    for (int i = 1; i < size_ham; i++)
        ham[0] = ham[0] ^ ham[i];
}

void byte2bits(int numBytes, char *byte, int *bits)
{
    for (int i = 0, j = numBytes - 1; j >= 0; i++)
    {
        bits[i] = (byte[j] >> i % bitsInByte) & 1;
        if (!((i % bitsInByte) - 7))
            j--;
    }
}

void outputFile(int outNumBytes, int *ham, FILE *arqHamming)
{
    for (int word = 0, indexByte = (bitsInByte * outNumBytes) - bitsInByte; word < outNumBytes; word++)
    {
        u_int8_t output = 0;
        for (int indexBit = bitsInByte - 1; indexBit >= 0; indexBit--)
            output = (output << 1) | ham[indexBit + indexByte];
        indexByte -= bitsInByte;
        fwrite(&output, sizeof(u_int8_t), 1, arqHamming);
    }
}

void writeHam(char *binInput, int SIZE_IN)
{
    //numero de bits de entrada + numero de bits de controle necessarios + bit G
    int SIZE_HAM = SIZE_IN + (int)ceil(log2(SIZE_IN + 1)) + 1;
    // se o byte nao estiver completo o padding sera o tamanho necessario para que ele seja, se nao ele eh zero
    int SIZE_PAD = (SIZE_HAM % bitsInByte) ? (bitsInByte - (SIZE_HAM % bitsInByte)) : 0;

    FILE *arq, *arqHamming;
    arq = fopen(binInput, "rb");
    sprintf(binInput, "%s.wham%d", binInput, (SIZE_HAM + SIZE_PAD));
    arqHamming = fopen(binInput, "wb");

    int bits[SIZE_IN], ham[(SIZE_HAM + SIZE_PAD)], numBytes = SIZE_IN / bitsInByte;
    char byte[numBytes];

    while (fread(byte, sizeof(char), numBytes, arq))
    {
        byte2bits(numBytes, byte, bits);
        printf("Bits de entrada: ");
        printaString(bits, SIZE_IN);

        memset(ham, 0, sizeof(int) * (SIZE_HAM + SIZE_PAD));
        //preenche a palavra hamming com os bits de entrada nas posicoes nao destinadas ao controle
        for (int i = 0, j = 0; i < SIZE_HAM; i++)
            if (i & (i - 1))
                ham[i] = bits[j++];

        printf("Palavra Hamming sem controle: ");
        printaString(ham, SIZE_HAM);

        geraBitsControle(ham, SIZE_IN, SIZE_HAM);
        geraBitG(ham, SIZE_HAM);

        printf("Palavra Hamming calculada:    ");
        printaString(ham, SIZE_HAM);

        unsigned short outNumBytes = (SIZE_HAM + SIZE_PAD) / bitsInByte;
        outputFile(outNumBytes, ham, arqHamming);
    }

    fclose(arq);
    fclose(arqHamming);
}

void extractHam(int *ham, int *bits, int SIZE_HAM)
{
    memset(bits, 0, sizeof(int) * SIZE_HAM - bias);
    for (int i = 0, j = 0; i < SIZE_HAM; i++)
        if (i & (i - 1))
            bits[j++] = ham[i];
}

void readHam(char *binInput)
{
    FILE *arqHamming, *arq;

    //concatena ao fim da string out o nome do arquivo, removendo o .wham de seu final
    char *ponto, out[100] = "out_";
    strcat(out, binInput);
    ponto = strrchr(out, '.');
    memset(ponto, '\0', 1);

    arqHamming = fopen(binInput, "rb");
    arq = fopen(out, "wb");

    //pega o valor presente na extensao do arquivo
    int SIZE_IN = atoi(&ponto[5]);
    int bits[SIZE_IN];

    unsigned numBytes = SIZE_IN / bitsInByte;
    char byte[numBytes];

    while (fread(byte, sizeof(char), numBytes, arqHamming))
    {
        byte2bits(numBytes, byte, bits);

        int ham[SIZE_IN];

        for (int i = 0; i < SIZE_IN; i++)
            ham[i] = bits[i];

        printf("Palavra Hamming de entrada:  ");
        printaString(bits, SIZE_IN);

        printf("Palavra Hamming Recalculada: ");
        geraBitsControle(ham, SIZE_IN, SIZE_IN);
        printaString(ham, SIZE_IN);

        u_int8_t sindrome = 0;
        printf("Bit de controle recalculado | Bit de controle lido\n");
        //realiza uma xor entre os bits de controle e os atribui a palavra sindrome
        for (int i = 1; i < SIZE_IN; i = i << 1)
        {
            printf("%d | %d\n", ham[i], bits[i]);
            sindrome = (sindrome << 1) | (ham[i] ^ bits[i]);
        }
        printf("\nPalavra sindrome: ");
        printf("0x%x\n", sindrome);

        //se a palabra sindrome nao for 0 e existir dentro dos bits de entrada, eh feita a correcao
        if (sindrome && sindrome <= SIZE_IN)
        {
            bits[sindrome] = abs(bits[sindrome] - 1);
            ham[sindrome] = abs(ham[sindrome] - 1);
            printf("Alterando bit no indice %d\n", sindrome);
        }

        geraBitG(ham, SIZE_IN);

        if (ham[0] ^ bits[0])
            printf("Houve uma diferenca no bit G. PALAVRA REJEITADA.\n");
        else
            printf("Palavra Aceita!\n");

        int bitsOut[SIZE_IN - bias];
        extractHam(bits, bitsOut, SIZE_IN);

        printf("Bits de saida: ");
        printaString(bitsOut, SIZE_IN - bias);

        outputFile(numBytes - 1, bitsOut, arq);
    }
    fclose(arqHamming);
    fclose(arq);
}

int main(int argc, char const *argv[4])
{
    char binInput[30], mode[3];
    strcpy(binInput, argv[1]);
    strcpy(mode, argv[2]);

    if (mode[1] == 'w')
    {
        int SIZE_IN = atoi(argv[3]);
        writeHam(binInput, SIZE_IN);
    }
    else if (mode[1] == 'r')
        readHam(binInput);
    else
        printf("Modo invalido\n");
    return 0;
}