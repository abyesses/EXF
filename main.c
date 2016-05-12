//
//  main.c
//  PA_EF
//
//  Created by Abraham Esses on 5/12/16.
//  Copyright © 2016 Abraham Esses. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CASILLAS 8
#define NUM_PROC 2
typedef struct{
    int id;
    int entrada;
    int salida;
}proc;
int matrix[NUM_CASILLAS][NUM_CASILLAS];
proc * procStruct;
int matrizSol[NUM_CASILLAS*10][2];
void signalHandler(int);
void generaMatriz();
void matrizSolinit();
void addTomatrizSol(int,int);
void imprimeMatriz();
void * buscaCaminos(void *);
void asignaCuadrante();
void recorreCuadrante(int j);
void signalHandler(int p){
    printf("He recibido la señal SIGUSR1\n");
    imprimeMatriz();
}
void matrizSolinit(){
    for (int i = 0; i < NUM_CASILLAS*10; ++i) {
        for (int j = 0; j<2; j++) {
            matrizSol[i][j]=-1;
        }
    }
}
void addTomatrizSol(int k ,int d){
    int indice = -1;
    for (int i = 0; i < NUM_CASILLAS*10; ++i) {
            if (matrizSol[i][0] == -1 && matrizSol[i][1] == -1) {
                indice = i;
            }
        
    }
    matrizSol[indice][0] = k;
    matrizSol[indice][1] = d;
}
void imprimeMatriz(){
    printf("Imprimiendo matriz solución\n");
    for (int i = 0; i < NUM_CASILLAS*10; i++) {
        if (matrizSol[i][0] != -1 && matrizSol[i][1] != -1 ) {
            printf("[%d][%d]\n",matrizSol[i][0],matrizSol[i][1]);
        }
        
    }
}
void generaMatriz(){
    for (int i = 0; i < NUM_CASILLAS; ++i) {
        for (int j = 0; j < NUM_CASILLAS; ++j) {
            matrix[i][j] = (rand() % (1+1-0))+0;
        }
    }
}
void * buscaCaminos(void * p){
    proc *id = (proc *)p;
    printf("Mi ID es:%d \n",id->id);
    printf("Mi entrada es: [%d][%d]\n",id->entrada,id->entrada);
    printf("Mi salida es: [%d][%d]\n",id->salida,id->salida);
    matrix[id->entrada][id->entrada] = 0;
    matrix[id->salida][id->salida] = 0;
    for (int k = id->entrada; k <= id->salida; k++) {
        for (int d = id->entrada; d <= id->salida; d++) {
            //Comprobando diagonal
            if (k == d &&  matrix[k][d] == 0) {
                //printf("Thread[%d]: [%d][%d]\n",id->id,k,d);
                addTomatrizSol(k, d);
            }
            //Caminando a la derecha
            else if (matrix[k][d] == 0){
                //printf("Thread[%d]: [%d][%d]\n",id->id,k,d);
                addTomatrizSol(k, d);
            }
            //Caminando hacia abajo
            else if (matrix[d][k] == 0){
                //printf("Thread[%d]: [%d][%d]\n",id->id,k,d);
                addTomatrizSol(k, d);
            }
            else if (matrix[k][d] == 1){
                 printf("Obstaculo encontrado en [%d][%d]\n",k,d);
            }
        }
    }
    pthread_exit(NULL);
}
int main(int argc, const char * argv[]) {
    signal(SIGUSR1, signalHandler);
    signal(SIGINT,SIG_IGN);
    procStruct = (proc *)malloc(sizeof(proc)*NUM_PROC);
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t)*NUM_PROC);
    generaMatriz();
    matrizSolinit();
    proc * temp;
    for (int i = 0; i < NUM_PROC ; ++i) {
        temp = &procStruct[i];
        temp->id = i+1;
        if (temp->id == 1) {
            temp->entrada = ((NUM_CASILLAS/NUM_PROC)*temp->id - (NUM_CASILLAS/NUM_PROC));
        }
        else
            temp->entrada = ((NUM_CASILLAS/NUM_PROC)*temp->id - (NUM_CASILLAS/NUM_PROC))+1;
        temp->salida = (NUM_CASILLAS/NUM_PROC)*temp->id;
        
        pthread_create(&threads[i], NULL, buscaCaminos, (void *)temp);
    }
    for (int j = 0; j < NUM_PROC ; ++j) {
        pthread_join(threads[j], NULL);
    }
    imprimeMatriz();
    return 0;
}
