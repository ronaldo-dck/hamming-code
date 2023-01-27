# hamming-code
Github: @satorike @marlonfpereira
Para escrita hamming, insira o nome do arquivo, o modo '-w' seguido pelo tamanho da palavra a ser protegida: 8, 16, 32 ou 64;
ex. ./hamming arquivo.txt -w 26

Para leitura, insira apenas o nome do arquivo com .whamX (sendo X o tamanho da palavra hamming anteriormente gerada) e o modo '-r';
ex. ./hamming arquivo.txt.wham24 -r