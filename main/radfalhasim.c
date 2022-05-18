#include <stdio.h>
#include "radfalhasim.h"
#include <time.h>
#include <stdlib.h>
#include <esp_system.h>
#include <esp_random.h>

#define MOD_BYTE 128

radfalhasim* falhas=NULL; 

int getPerdeu(unsigned int perdas){
	printf("Perdeu\n");
	int retorno=-1;
	if(falhas){
		radfalhasim* falha;
		falha=falhas;
		while(falha){
			if(falha->perdas==perdas){
				retorno = falha->perdeu;
				falha=NULL;
			}
			else
				falha=falha->next;
		}
	}
	return retorno;
}

radfalhasim* radfalhasimInicializa(unsigned int perdas)
{
	if(perdas == 20)
	{
		radfalhasim* retorno;
		retorno = (radfalhasim*) malloc(sizeof(radfalhasim));
		retorno->perdas=perdas;
		retorno->perdeu=0;
		retorno->propPassa=6;
		retorno->propNaoPassa=3;
		retorno->pacotePosicao=0;
		retorno->pacotePassa=1;
		retorno->bytePassa=1;
		retorno->next=NULL;
		return retorno;
	}
	else if(perdas == 40)
	{
		radfalhasim* retorno;
		retorno = (radfalhasim*) malloc(sizeof(radfalhasim));
		retorno->perdas=perdas;
		retorno->propPassa=3;
		retorno->propNaoPassa=2;
		retorno->pacotePosicao=0;
		retorno->pacotePassa=1;
		retorno->bytePassa=1;
		retorno->next=NULL;
		return retorno;
	}
	else
		return NULL;
}

int radfalhasimPacote(radfalhasim* falha,int tam){
	int retorno=1;
	while(tam){
		if(!(esp_random()%MOD_BYTE))
			falha->bytePassa=!(falha->bytePassa);
		if(!(falha->bytePassa))
			retorno=0;
		tam--;
	}
	return retorno;
}

int getRadfalhasim(unsigned int perdas,unsigned int tam){
	if(tam && perdas && (perdas==20 || perdas==40)){
		int retorno=1;
		radfalhasim* falha;
		if(falhas){
			if(perdas == falhas->perdas){
				//printf("Pegando %d\n",perdas);
				falha=falhas;
			}
			else{
				if(falhas->next){
					//printf("Pegando %d\n",perdas);
					falha=falhas->next;
				}
				else{
					//printf("Inicializando %d\n",perdas);
					falhas->next=radfalhasimInicializa(perdas);
					falha=falhas->next;
				}
			}
		}
		else{
			srand(time(NULL));
			falhas=radfalhasimInicializa(perdas);
			falha=falhas;
		}
		if(tam > falha->pacotePosicao){
			if(falha->pacotePosicao){
				tam = tam - falha->pacotePosicao;
				if(!(falha->pacotePassa))
					retorno = radfalhasimPacote(falha,falha->pacotePosicao);
			}
			falha->pacotePosicao = 512 - tam;

			if(falha->pacotePassa){
				falha->pacotePassa = esp_random()%(falha->propPassa);
			}
			else
				falha->pacotePassa = esp_random()%(falha->propNaoPassa);

			if(!(falha->pacotePassa)){
			       falha->perdeu++;
		               if(retorno)
				       retorno = radfalhasimPacote(falha,tam);
			}
		}
		else{
			falha->pacotePosicao = falha->pacotePosicao - tam;
			if(!(falha->pacotePassa))
				retorno = radfalhasimPacote(falha,tam);
		}
		return retorno;
	}
	else
		return 0;
}

