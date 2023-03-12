#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "mythreads.h"
int max;
int loops;
int *buffer1,*buffer2;
pthread_mutex_t trava = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t trava2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cons;
pthread_cond_t amb1;
pthread_cond_t amb2;
pthread_cond_t prod;
int consome1  = 0;
int produz1 = 0;
int consome2  = 0;
int produz2= 0;
int ocupbuf1 = 0;
int ocupbuf2 = 0;
int consbuf1 = 0;
int consbuf2 = 0;
#define MAX (10)
int consumidores = 1;
int produtores = 1;
int nambos = 1;

//Aluno: Kennedy Coelho Nolasco 
//Matrícula:21751912

void produz(int valor,int buf) {
   if(buf==1){
      buffer1[produz1] = valor;
      produz1 = (produz1+1) % max;
   }else{
      buffer2[produz2] = valor;
      produz2 = (produz2+1) % max;
   }
}
int consome(int buf) {
   int tmp; 
   if(buf==1){
      tmp = buffer1[consome1];
      consome1 = (consome1+1) %max;
   }else{
      tmp = buffer2[consome2];
      consome2 = (consome2+1) %max;
   }
   return tmp;
}
void *produtor(void *arg) {
   int i;
   for (i = 0; i < loops; i++) {
      Pthread_mutex_lock(&trava);
      while(ocupbuf1>=max)
	      Pthread_cond_wait(&prod,&trava);
      produz(i,1);
      ocupbuf1++;
      Pthread_cond_signal(&amb1);
      printf("Produtor %d produziu %d\n", *(int*)(&arg),i);
      Pthread_mutex_unlock(&trava);
   }
   printf("Produtor %d finalizado\n",*(int*)(&arg));
   return NULL;
}

void *consumidor(void *arg) {
   int tmp = 0;
   int i=0;
   while(1) {
      Pthread_mutex_lock(&trava2);
      while(ocupbuf2==0)
	      Pthread_cond_wait(&cons,&trava2);
      tmp = consome(2);
      ocupbuf2--;
      consbuf2++;
      printf("Consumidor %lld consumiu %d\n", (long long int) arg, tmp);
      Pthread_cond_signal(&amb2);
      Pthread_mutex_unlock(&trava2);
   }
   return NULL;
}

void *ambos(void *arg) {
   int tmp = 0;
   int i=0;
   while(1){
      Pthread_mutex_lock(&trava);
      while(ocupbuf1==0)
          Pthread_cond_wait(&amb1,&trava);
      tmp = consome(1);
      ocupbuf1--;
      consbuf1++;
      printf("Ambos %lld consumiu %d\n", (long long int)arg, tmp);
      Pthread_cond_signal(&prod);
      Pthread_mutex_unlock(&trava);
      Pthread_mutex_lock(&trava2);
      while(ocupbuf2>=max)
          Pthread_cond_wait(&amb2,&trava2);
      produz(tmp,2);
      ocupbuf2++;
      Pthread_cond_signal(&cons);
      Pthread_mutex_unlock(&trava2);
   }
   return NULL;
}

int main(int argc, char *argv[]) {
   if (argc != 6) {
      fprintf(stderr, "uso: %s <tambuffer> <loops> <produtores> <consumidores> <ambos>\n", argv[0]);
      exit(1);
   }
   max   = atoi(argv[1]);
   loops = atoi(argv[2]);
   produtores = atoi(argv[3]);
   consumidores = atoi(argv[4]);
   nambos = atoi(argv[5]);
   assert(consumidores <= MAX);
   buffer1 = (int *) malloc(max * sizeof(int));
   buffer2 = (int *) malloc(max * sizeof(int));
   int i;
   for (i = 0; i < max; i++) {
      buffer1[i] = 0;
      buffer2[i] = 0;
   }
   Pthread_cond_init(&cons, NULL);
   Pthread_cond_init(&amb1, NULL);
   Pthread_cond_init(&amb2, NULL);
   Pthread_cond_init(&prod, NULL);
   pthread_t pid[MAX], cid[MAX],aid[MAX];
   for (i = 0; i < produtores; i++) {
      pthread_create(&pid[i], NULL, produtor, (void*)(long long int)i);
   }
   for (i = 0; i < consumidores; i++) {
      pthread_create(&cid[i], NULL, consumidor, (void *) (long long int) i); 
   }
   for (i = 0; i < nambos; i++) {
      pthread_create(&aid[i], NULL, ambos, (void *) (long long int) i);
   }
   while((consbuf2<produtores*loops) || (consbuf1<produtores*loops)){
   }
   for(i=0;i<produtores;i++){
        pthread_cancel(pid[i]); 
   }
   for(i=0;i<consumidores;i++){
	pthread_cancel(cid[i]); 
   }
   for(i=0;i<nambos;i++){
	pthread_cancel(aid[i]); 
   }
   return 0;
}


