#include <stdio.h>
#include <stdlib.h>
#include "frame.h"

#define TAM 200100 //Tamanho de troca_pixel
#define num_frames 6571 // Até que frame do vídeo será incluído (maximo é 6571)

int indice = 0;
int pula_frame = 1;
char arquivo[50] = "D:/Arduino2_2_1/BadApple/LCD_Test/pixels.h";
//char arquivo[50] = "pixels.h";
short int quantidade[num_frames]; // Quantos pixels mudaram?
short int troca_pixel[TAM];
short int matA[FRAME_ROWS][FRAME_COLS], matB[FRAME_ROWS][FRAME_COLS];

void imprime_matriz(short int mat[FRAME_ROWS][FRAME_COLS])
{
    for(int i = 0; i < FRAME_ROWS; i++)
    {
        for(int j = 0; j < FRAME_COLS; j++)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

void imprime_vetor(short int vet[], int size)
{
    int rep = 0;
    for(int i = 0; i < size; i++)
    {
        printf("%d ", vet[i]);
        rep++;
        if(rep >= 30)
        {
            rep = 0;
            printf("\n");
        }
    }
}

void soma_elementos(short int vet[], int size)
{
    long int soma = 0;
    for(int i = 0; i < size; i++)
    {
        soma += vet[i];
    }
    printf("Soma dos elementos ate a posicao %d: %d\n",(size-1),soma);
}

void min(short int vet[], int size)
{
    short int menor = 0;
    for(int i = 0; i < size; i++)
    {
        if(vet[i] < menor)
        {
            menor = vet[i];
        }
    }
    printf("Menor valor: %d",menor);
}

void passar_conteudo(short int vetMat[FRAME_ROWS][FRAME_COLS], const uint8_t vetFrames[FRAME_ROWS][FRAME_COLS])
{
    for(int i = 0; i < FRAME_ROWS; i++)
    {
        for(int j = 0; j < FRAME_COLS; j++)
        {
            vetMat[i][j] = vetFrames[i][j];
        }
    }
}

void comparar_matrizes(short int vetA[FRAME_ROWS][FRAME_COLS], short int vetB[FRAME_ROWS][FRAME_COLS], int frame_atual)
{
    short int posicao = -10, posicao_anterior = -10; // Precisa iniciar com valor negativo para não conflitar com posições calculadas
    bool meu_teste = 0;
    bool neg = 0;
    for(int i = 0; i < FRAME_ROWS; i++)
    {
        for(int j = 0; j < FRAME_COLS; j++)
        {
            if(vetA[i][j]^vetB[i][j])
            {
                if(indice >= TAM)
                {
                    printf("Limite de memoria atingido");
                    exit(1);
                }
               posicao = 40 * i + j;
               if(posicao == 1076)
               {
                meu_teste = 1;
               }
               if(posicao == (posicao_anterior + 1))
               {
                    neg = 1; //Verifica se havia números negativos antes
                    troca_pixel[indice]--;
                    quantidade[frame_atual]++;
                    posicao_anterior = posicao;
               }
               else
               {
                    if(neg)
                    {
                        neg = 0;
                        indice++; // Garante que a próxima posição positiva não sobrescreverá o valor negativo
                    }
                    troca_pixel[indice] = posicao;
                    indice++;
                    posicao_anterior = posicao;
                    quantidade[frame_atual]++;              
               }
               
            }
        }
    }
    if(neg) // Caso frame termine em valor negativo
    {
        neg = 0;
        indice++;
    }
}

void grava_arquivo()
{
    int cnt = 0;
    int sub_quant = num_frames;
    int bloco = 200100;
    FILE *arq = fopen(arquivo,"w");
    fprintf(arq,"#include <stdint.h>\n\n");
    fprintf(arq, "const uint8_t inc = %d;\n\n", pula_frame + 1);
    fprintf(arq,"const int16_t quantidade[%d] = {%d", sub_quant, quantidade[0]);
    for(int i = 1; i < sub_quant; i++)
    {
        fprintf(arq, ",%d",quantidade[i]);
        cnt++;
        if(cnt >= 50)
        {
            cnt = 0;
            fprintf(arq, "\n");
        }
    }
    fprintf(arq,"};\n\n");
    cnt = 0;
    fprintf(arq,"const int16_t vet0[%d] = {%d",bloco,troca_pixel[0]);
    for(int i = 1; i < bloco; i++)
    {
        fprintf(arq,",%d",troca_pixel[i]);
        cnt++;
        if(cnt >= 50)
        {
            cnt = 0;
            fprintf(arq, "\n");
        }
    }
    fprintf(arq,"};\n\n");
    fclose(arq);
}

int main(){
    int avanco = pula_frame + 1;
    for(int k = 0; (k+avanco) < (num_frames+1); k = k + avanco) // maior valor de frame válido é 6571
    {
        passar_conteudo(matA, frames[k]);
        passar_conteudo(matB, frames[k + avanco]);
        comparar_matrizes(matA,matB,k);
    }
    //imprime_vetor(troca_pixel, 500);
    //printf("O total de elementos e: %d", indice);
    grava_arquivo();
    return 0;
}