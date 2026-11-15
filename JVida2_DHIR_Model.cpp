// JVida2_DHIR_Model.cpp
// 05/11/2026
/*Daniel Luis de Amorim Mariano Santos
Henrique Campos Rodrigues
Isabella de Souza Fleury
Rafaella Castro Zandona Alves de Lima */

#include <stdio.h>
#include <stdlib.h>
#include "JVida2_DHIR_Model.h"

#define ARQUIVO "geracoes.txt"

char Matriz[60][60];
int dim;
int mostrarMortos;

static int proxGeracao = 0; //quantas geracoes ja foram carregadas

int dimValida(int d) {
    if (d >= 10 && d <= 60) {
        return 1;
    }
    return 0;
}

void limparMapa() {
    int l, c;
    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            Matriz[l][c] = '.';
        }
    }
}

int coordValida(int l, int c) {
    if (l >= 0 && l <= dim - 1 && c >= 0 && c <= dim - 1) {
        return 1;
    }
    return 0;
}

int celulaViva(int l, int c) {
    if (Matriz[l][c] == 'O') {
        return 1;
    }
    return 0;
}

void incluirCelula(int l, int c) {
    Matriz[l][c] = 'O';
}

void excluirCelula(int l, int c) {
    Matriz[l][c] = '.';
}

void marcarVizinhasMortas() {
    int l, c, i, j;
    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            if (celulaViva(l, c)) {
                for (i = l - 1; i <= l + 1; i++) {
                    for (j = c - 1; j <= c + 1; j++) {
                        if (coordValida(i, j) && Matriz[i][j] == '.') {
                            Matriz[i][j] = '+'; //marcar as vizinhas mortas com + 
                        }
                    }
                }
            }
        }
    }
}

void desmarcarVizinhasMortas() {
	int i, j;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
        	if (Matriz[i][j] == '+'){
            Matriz[i][j] = '.';
        	}
        }
    }
}

// retorna 1 se salvou, 0 se deu erro
int salvarGeracao() {
    FILE *f;
    int l, c, qtd = 0;

    f = fopen(ARQUIVO, "a"); // "a" = acrescenta no fim
    if (f == NULL) {
        return 0;
    }

    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            if (celulaViva(l, c)) {
                qtd++;
            }
        }
    }

    fprintf(f, "%d %d\n", dim, qtd); // cabecalho: dimensao e quantidade
    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            if (celulaViva(l, c)) {
                fprintf(f, "%d %d\n", l, c);
            }
        }
    }

    fclose(f);
    return 1;
}

// retorna 1 se carregou, 0 se acabaram as geracoes, -1 se nao achou o arquivo
int carregarGeracao() {
    FILE *f;
    int g, i, d, qtd, l, c;

    f = fopen(ARQUIVO, "r");
    if (f == NULL) {
        return -1;
    }

    // pula as geracoes que ja foram carregadas
    for (g = 0; g < proxGeracao; g++) {
        if (fscanf(f, "%d %d", &d, &qtd) != 2) {
            fclose(f);
            proxGeracao = 0; //volta ao comeco na proxima vez
            return 0;
        }
        for (i = 0; i < qtd; i++) {
            fscanf(f, "%d %d", &l, &c);
        }
    }

    // le a geracao da vez
    if (fscanf(f, "%d %d", &d, &qtd) != 2 || !dimValida(d)) {
        fclose(f);
        proxGeracao = 0;
        return 0;
    }

    dim = d;
    limparMapa();
    for (i = 0; i < qtd; i++) {
        if (fscanf(f, "%d %d", &l, &c) == 2 && coordValida(l, c)) {
            incluirCelula(l, c);
        }
    }

    fclose(f);
    proxGeracao++;
    return 1;
}

//conta os vizinhos vivos de uma casa (no maximo 8)
int contarVizinhosVivos(int l, int c) {
    int i, j, n = 0;
    for (i = l - 1; i <= l + 1; i++) {
        for (j = c - 1; j <= c + 1; j++) {
            if (!(i == l && j == c) && coordValida(i, j) && celulaViva(i, j)) {
                n++;
            }
        }
    }
    return n;
}

//quantidade de celulas vivas
int contarVivas() {
    int l, c, n = 0;
    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            if (celulaViva(l, c)) {
                n++;
            }
        }
    }
    return n;
}

//quantidade de vizinhas-mortas (casa nao viva com pelo menos 1 vizinho vivo)
int contarVizinhasMortas() {
    int l, c, n = 0;
    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            if (!celulaViva(l, c) && contarVizinhosVivos(l, c) > 0) {
                n++;
            }
        }
    }
    return n;
}

/* criar um No novo e coloca no inicio da lista */
static No *inserirInicio(No *ini, int l, int c) {
	No *novo = (No *) malloc(sizeof(No)); //pede o espaco da memoria do tamanho do No
	if (novo == NULL){
		return ini; //sem memoria = nao insere
	}
	novo -> l = l;
	novo -> c = c;
	novo -> prox = ini;
	return novo;
}

void liberarLista(No *ini){
	No *p;
	while (ini != NULL){
		p = ini;
		ini = ini -> prox;
		free(p);
	}
}

No *CriarListaVivas(){
	No *ini = NULL;
	int l, c;
	for (l = dim - 1; l >= 0; l--){ //percorre de tras para frente 
		for (c =dim - 1; c >= 0; c--){
			if (celulaViva(l, c)){
				ini = inserirInicio(ini, l, c);
			}
		}
	}
	return ini;
}

No *CriarListaVizinhasMortas(){
	No *ini = NULL;
	int l, c;
	for (l = dim - 1; l >= 0; l--){
		for (c = dim -1; c >= 0; c--){
			if (!celulaViva(l, c) && contarVizinhosVivos(l, c) > 0){
				ini = inserirInicio(ini, l, c);
			}
			
		}
	}
	return ini;
}

//gera a proxima geracao usando a lista ligada simples 
void proximaGeracao() {
	No *vivasProx = NULL;
	No *p;
    int l, c, viz;

    for (l = 0; l < dim; l++) {
        for (c = 0; c < dim; c++) {
            viz = contarVizinhosVivos(l, c);
            if (celulaViva(l, c)) {
                if (viz == 2 || viz == 3) {
                    vivasProx = inserirInicio(vivasProx, l, c); //sobrevive
            	}
        	} else if (viz == 3){
        		vivasProx = inserirInicio(vivasProx, l, c); //nasce
				}
		}
    }

    limparMapa();
    for (p = vivasProx; p != NULL; p = p->prox) {
        incluirCelula(p->l, p->c);
    }
    liberarLista(vivasProx);
	}

int qtdValida(int qtd){
	if (qtd >= 1){
		return 1;
	}
	return 0;
}

int velValida(int vel){
	if (vel >= 0){
		return 1;
	}
	return 0;
}
