#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char const *argv[])
{
    int8_t nums[4] = {111,2,3,4};
    
    FILE* arq;
    arq = fopen("numeros.bin", "rb");

    FILE *arqHamming;
    arqHamming = fopen("numerosHammin.bin", "wb");
    //fwrite(nums, sizeof(int8_t), 4, arq);
    char byte;

    int bits[8], ham[16];

    for (int loop = 0; loop < 4; loop++)
    {

    
    fread(&byte, sizeof(char), 1, arq);

    for (int i = 0; i < 8; i++)
    {
        bits[i] = (byte >> i) & 1;
        //printf("%d", bits[i]);
    }

//if(!(log2(i)%1))
    ham[3] = bits[0];
    ham[5] = bits[1];
    ham[6] = bits[2];
    ham[7] = bits[3];
    ham[9] = bits[4];
    ham[10] = bits[5];
    ham[11] = bits[6];
    ham[12] = bits[7];

    ham[13] = 0;
    ham[14] = 0;
    ham[15] = 0;

    for (int i = 1; i <= 8; i = i<<1)
    {
        ham[i] = 0;  // <--- For com multiplas atribuições, complexidade de leitura e entendido do codigo
        for (int j = 3; j < 13; j++)
            if(i&j)
                ham[i] = ham[i] ^ ham[j];
    }
    
    ham[0] = 0;
    for (int i = 1; i < 13; i++)
        ham[0] = ham[0] ^ ham[i];

    u_int8_t teste1 = 0, teste2 = 0;
    for(int i = 7; i>=0; i--)
        teste1 = (teste1<<1) | ham[i];

    printf("%x ", teste1);
    
    for(int i = 15; i>=8; i--)
        teste2 = (teste2<<1) | ham[i];

    printf("%x ", teste2);

    for(int i = 15; i>=0; i--)
        printf("%d ", ham[i]);


    fwrite(&teste2, sizeof(u_int8_t), 1, arqHamming);
    fwrite(&teste1, sizeof(u_int8_t), 1, arqHamming);
    }

    fclose(arq);
    fclose(arqHamming);
    return 0;


}
