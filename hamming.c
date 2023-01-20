#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void byte2bits(int numBytes, char *byte, int *bits)
{
    for (int i = 0, j = numBytes - 1; j >= 0; i++)
    {
        bits[i] = (byte[j] >> i % 8) & 1;
        if (!((i % 8) - 7))
            j--;
    }
}

void writeFile(int outTimes, int *ham, FILE *arqHamming)
{
    for (int row = 0, i = (8 * outTimes) - 8; row < outTimes; row++)
    {
        u_int8_t output = 0;
        for (int j = 7; j >= 0; j--)
            output = (output << 1) | ham[j + i];
        i -= 8;
        fwrite(&output, sizeof(u_int8_t), 1, arqHamming);
    }
}
void writeHam(char* binInput, int SIZE_IN)
{
    int SIZE_HAM = 1 + SIZE_IN + (int)ceil(log2(SIZE_IN + 1));
    int SIZE_PAD = 8 - (SIZE_HAM % 8);
    FILE *arq, *arqHamming;
    arq = fopen(binInput, "rb");
    sprintf(binInput, "%s.wham%d", binInput, (SIZE_HAM + SIZE_PAD));
    arqHamming = fopen(binInput, "wb");

    int bits[SIZE_IN], ham[(SIZE_HAM + SIZE_PAD)], numBytes = SIZE_IN / 8;
    char byte[numBytes];

    while (fread(byte, sizeof(char), numBytes, arq))
    {
        byte2bits(numBytes, byte, bits);

        memset(ham, 0, sizeof(int) * (SIZE_HAM + SIZE_PAD));
        for (int i = 0, j = 0; i < SIZE_HAM; i++)
            if (i & (i - 1))
                ham[i] = bits[j++];

        for (int i = 1; i <= SIZE_IN; i = i << 1)
        {
            ham[i] = 0;
            for (int j = 3; j < SIZE_HAM; j++)
                if (i & j)
                    ham[i] = ham[i] ^ ham[j];
        }

        for (int i = 1; i < SIZE_HAM; i++)
            ham[0] = ham[0] ^ ham[i];

        
        unsigned short outTimes = (SIZE_HAM + SIZE_PAD) / 8;
        writeFile(outTimes, ham, arqHamming);
    }

    fclose(arq);
    fclose(arqHamming);
}

void readHam(char* binInput)
{
    FILE *arqHamming, *arq;
    char *ponto, out[100] = "out_";
    strcat(out, binInput);
    ponto = strrchr(out, '.');
    memset(ponto, '\0', 1);
    arqHamming = fopen(binInput, "rb");
    arq = fopen(out, "wb");

    int SIZE_IN = atoi(&ponto[5]);
    unsigned numBytes = SIZE_IN / 8;
    int bits[SIZE_IN];
    char byte[numBytes];

    while (fread(byte, sizeof(char), numBytes, arqHamming))
    {
        byte2bits(numBytes, byte, bits);
        /* for (int i = SIZE_IN-1; i >= 0; i--)
            printf("%d ", bits[i]);
        printf("\n"); */

        int ham[SIZE_IN];
        for (int i = 0; i < SIZE_IN; i++)
            ham[i] = bits[i];

        u_int8_t sindrome = 0;
        for (int i = 1; i < SIZE_IN; i = i << 1)
        {
            ham[i] = 0;
            for (int j = 3; j < SIZE_IN; j++)
                if (i & j)
                    ham[i] = ham[i] ^ ham[j];

            printf("%d %d\n", ham[i], bits[i]);
            sindrome = (sindrome << 1) | (ham[i] ^ bits[i]);
        }
        printf("%d\n", sindrome);

        if(sindrome && sindrome <= SIZE_IN)
        {
            ham[sindrome] = abs(ham[sindrome] - 1);
            printf("Alterando bit no indice %d\n", sindrome);
        }

        ham[0] = 0;
        for (int i = 1; i < SIZE_IN; i++)
            ham[0] = ham[0] ^ ham[i];
        if(ham[0])
            printf("Palavra Rejeitada\n");
        else
        {
            printf("Palavra Aceita!\n");
            writeFile(numBytes, ham, arq);
        }
    }
    fclose(arqHamming);
    fclose(arq);
    }

int main(int argc, char const *argv[4])
{
    char binInput[30], mode[3];
    strcpy(binInput, argv[1]);
    strcpy(mode, argv[2]);

    if(mode[1] == 'w')
    {
        int SIZE_IN = atoi(argv[3]);
        writeHam(binInput, SIZE_IN);
    }
    else if(mode[1] == 'r')
    {
        readHam(binInput);
    }
    else
        printf("Modo invalido\n");
    return 0;
}
