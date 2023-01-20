#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SIZE_IN 8

int main(int argc, char const *argv[])
{
    int SIZE_HAM = 1 + SIZE_IN + (int)ceil(log2(SIZE_IN+1));
    int SIZE_PAD = 8 - (SIZE_HAM % 8);

    FILE *arq, *arqHamming;
    arq = fopen("numeros.bin", "rb");
    arqHamming = fopen("numerosHammin.bin", "wb");

    unsigned numBytes = SIZE_IN / 8;
    int bits[SIZE_IN], ham[(SIZE_HAM+SIZE_PAD)];
    char byte[numBytes];

    while (fread(byte, sizeof(char), numBytes, arq))
    {
        memset(ham, 0, sizeof(int) * (SIZE_HAM+SIZE_PAD));
        for (int i = 0, j = numBytes-1; j >= 0; i++)
        {
            bits[i] = (byte[j] >> i%8) & 1;
            if (!((i % 8) - 7))
                j--;
        }

        int j = 0;
        for (int i = 0; i < SIZE_HAM; i++)
            if (i & (i - 1))
                ham[i] = bits[j++];

        for (int i = 1; i <= SIZE_IN; i = i << 1)
        {
            ham[i] = 0;
            for (int j = 3; j < SIZE_HAM; j++)
                if (i & j)
                    ham[i] = ham[i] ^ ham[j];
        }

        ham[0] = 0;
        for (int i = 1; i < SIZE_HAM; i++)
            ham[0] = ham[0] ^ ham[i];

        unsigned short outTimes = (SIZE_HAM+SIZE_PAD)/8;
        for (int row = 0, i = SIZE_HAM + SIZE_PAD - 8; row < outTimes; row++)
        {
            u_int8_t output = 0;
            for (int j = 7; j >= 0; j--)
            {
                printf("%d %d\n", j+i, ham[j+i]);
                output = (output << 1) | ham[j + i];
            }
            i -= 8;
            fwrite(&output, sizeof(u_int8_t), 1, arqHamming);
        }
    }

    fclose(arq);
    fclose(arqHamming);
    return 0;
}
