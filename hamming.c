#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char const *argv[])
{
    FILE *arq;
    arq = fopen("numeros.bin", "rb");

    FILE *arqHamming;
    arqHamming = fopen("numerosHammin.bin", "wb");
    // fwrite(nums, sizeof(int8_t), 4, arq);
    char byte;

    int bits[8], ham[16];

    for (int loop = 0; loop < 4; loop++)
    {
        memset(ham, 0, sizeof(int) * 16);
        fread(&byte, sizeof(char), 1, arq);

        for (int i = 0; i < 8; i++)
        {
            bits[i] = (byte >> i) & 1;
        }

        int j = 0;
        for (int i = 0; i < 16; i++)
        {

        /* 
        i & ( i - 1 )
        bitwise de i seu antecessor
        
        2 & 2 - 1
        0010 & 0010 - 0001
        0010 & 0001
        0000

        3 & 3 - 1
        11 & 11 - 01 
        11 & 10

         10 => true



        8 & 8 - 1
        1000 & 0111
        0000
        0
         
        

        1000 & 0111 => 0000
        
         */    
            if (i & (i - 1))
            {
                ham[i] = bits[j++];
            }
        }


        for (int i = 1; i <= 8; i = i << 1)
        {
            ham[i] = 0; // <--- For com multiplas atribuições, complexidade de leitura e entendido do codigo
            for (int j = 3; j < 13; j++)
                if (i & j)
                    ham[i] = ham[i] ^ ham[j];
        }

        ham[0] = 0;
        for (int i = 1; i < 13; i++)
            ham[0] = ham[0] ^ ham[i];

        u_int8_t teste1 = 0, teste2 = 0;
        for (int i = 7; i >= 0; i--)
            teste1 = (teste1 << 1) | ham[i];

        printf("%x ", teste1);

        for (int i = 15; i >= 8; i--)
            teste2 = (teste2 << 1) | ham[i];

        printf("%x ", teste2);

        fwrite(&teste2, sizeof(u_int8_t), 1, arqHamming);
        fwrite(&teste1, sizeof(u_int8_t), 1, arqHamming);
    }

    fclose(arq);
    fclose(arqHamming);
    return 0;
}
