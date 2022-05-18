#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "showtransf.h"

typedef struct transf{
  int sentido;
  char* mensagem;
  size_t tamanho;
  struct transf* next;
}transf;

transf* transferencias=NULL;

void showtransfInclui(int sentido,char* mensagem,size_t tamanho){
  transf* novo;
  novo = (transf*)malloc(sizeof(transf));
  novo->sentido=sentido;
  novo->mensagem=(char*)malloc(tamanho*sizeof(char));
  memcpy(novo->mensagem,mensagem,tamanho);
  novo->tamanho=tamanho;
  novo->next=NULL;
  if(transferencias){
    int count=0;
    transf* apoio;
    apoio=transferencias;
    while(apoio->next){
      apoio=apoio->next;
      count++;
    }
    apoio->next=novo;
    while(count>60){
      apoio=transferencias;
      transferencias=transferencias->next;
      if(apoio->mensagem)
        free(apoio->mensagem);
      free(apoio);
      count--;
    }
  }
  else
    transferencias=novo;
}

int showtransfByte(char* destino,char byte,int posicao){
  char caracter;
  destino[posicao++]='x';
  caracter = byte & 0xF0;
  caracter = caracter >> 4;
  if(caracter<10)
    destino[posicao++]=caracter+48;
  else
    destino[posicao++]=caracter+55;
  caracter = byte & 0x0F;
  if(caracter<10)
    destino[posicao++]=caracter+48;
  else
    destino[posicao++]=caracter+55;
  return posicao;
}

int showtransfByteToCell(char* destino,char byte,int proximo){
      destino[proximo++]='<';
      destino[proximo++]='t';
      destino[proximo++]='d';
      destino[proximo++]='>';
      destino[proximo++]='<';
      destino[proximo++]='p';
      destino[proximo++]='>';
      proximo=showtransfByte(destino,byte,proximo);
      destino[proximo++]='<';
      destino[proximo++]='/';
      destino[proximo++]='p';
      destino[proximo++]='>';
      destino[proximo++]='<';
      destino[proximo++]='/';
      destino[proximo++]='t';
      destino[proximo++]='d';
      destino[proximo++]='>';
      return proximo;
}

int showtransfByte2ToCell(char* destino,char byte0,char byte1,int proximo){
      destino[proximo++]='<';
      destino[proximo++]='t';
      destino[proximo++]='d';
      destino[proximo++]='>';
      destino[proximo++]='<';
      destino[proximo++]='p';
      destino[proximo++]='>';
      proximo=showtransfByte(destino,byte0,proximo);
      proximo=showtransfByte(destino,byte1,proximo);
      destino[proximo++]='<';
      destino[proximo++]='/';
      destino[proximo++]='p';
      destino[proximo++]='>';
      destino[proximo++]='<';
      destino[proximo++]='/';
      destino[proximo++]='t';
      destino[proximo++]='d';
      destino[proximo++]='>';
      return proximo;
}

char* showtransfMostra(size_t* tamanho){
  char* resposta=NULL;
  if(transferencias){
    resposta=(char*)malloc(50*256*sizeof(char));
    sprintf(resposta,"<table>\n<tr><th><p>Sentido</p></th><th><p>Pais</p></th><th><p>Satelite</p></th><th><p>ID</p></th><th><p>Tamanho</p></th><th><p>Comp</p></th><th><p>Mensagem</p></th></tr>\n");
    transf* apoio=transferencias;
    int acrescimo;
    acrescimo=strlen(resposta);
    while(apoio){
      if(apoio->sentido==1)
        sprintf(resposta+acrescimo,"<tr><td><p>S->B</p></td>");
      else if(apoio->sentido==2)
        sprintf(resposta+acrescimo,"<tr><td><p>B->S</p></td>");
      else if(apoio->sentido==3)
        sprintf(resposta+acrescimo,"<tr><td><p>ATAC</p></td>");
      else
        sprintf(resposta+acrescimo,"<tr><td><p>BLOK</p></td>");
      acrescimo=acrescimo+24;
      acrescimo=showtransfByteToCell(resposta,apoio->mensagem[0],acrescimo);
      acrescimo=showtransfByte2ToCell(resposta,apoio->mensagem[1],apoio->mensagem[2],acrescimo);
      acrescimo=showtransfByteToCell(resposta,apoio->mensagem[3],acrescimo);
      acrescimo=showtransfByteToCell(resposta,apoio->mensagem[4],acrescimo);
      acrescimo=showtransfByte2ToCell(resposta,apoio->mensagem[5],apoio->mensagem[6],acrescimo);
      sprintf(resposta+acrescimo,"<td><p>");
      acrescimo=acrescimo+7;
      if(apoio->mensagem[5]!=0){
        sprintf(resposta+acrescimo,"0x%02x",apoio->mensagem[7]);
        acrescimo=acrescimo+4;
        if(apoio->mensagem[3]==0){
          for(int i=8;i<apoio->mensagem[5]+7;i++){
            sprintf(resposta+acrescimo,"0x%02x",apoio->mensagem[i]);
            acrescimo=acrescimo+4;
          }
        }
        else{
          for(int i=8;i<apoio->mensagem[5]+7;i++)
            resposta[acrescimo++]=apoio->mensagem[i];
        }
      }
      sprintf(resposta+acrescimo,"</p></td></tr>\n");
      acrescimo=acrescimo+15;
      apoio=apoio->next;
    }
    sprintf(resposta+acrescimo,"</table>\n");
    acrescimo=acrescimo+9;
    *tamanho=acrescimo;
  }
  else{
    char nada[]="<h2>Nada Ainda</h2>\n";
    *tamanho=sizeof(nada);
    resposta=(char*)malloc((*tamanho)*sizeof(char));
    memcpy(resposta,nada,*tamanho);
  }
  return resposta;
}

void showtransfDeletaTodos(transf* transferido){
  if(transferido->next)
    showtransfDeletaTodos(transferido->next);
  if(transferido->mensagem)
    free(transferido->mensagem);
  free(transferido);
}

void showtransfDestroy(){
  if(transferencias){
    showtransfDeletaTodos(transferencias);
    transferencias=NULL;
  }
}
