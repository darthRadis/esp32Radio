#ifndef __RADFALHASIM__
#define __RADFALHASIM__

typedef struct radfalhasim{
	int pacotePosicao;
	int pacotePassa;
	int bytePassa;
	unsigned int perdas;
	int perdeu;
	int propPassa;
	int propNaoPassa;
	struct radfalhasim* next;
}radfalhasim;

int getPerdeu(unsigned int perdas);
int getRadfalhasim(unsigned int perdas,unsigned int tam);
#endif
