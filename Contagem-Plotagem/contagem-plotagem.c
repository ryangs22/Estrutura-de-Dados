#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct NoLista {
    int valor;
    struct NoLista *prox;
} NoLista;

NoLista* inserirLista(NoLista *inicio, int valor) {
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));
    novo->valor = valor;
    novo->prox = inicio;
    return novo;
}

int buscarLista(NoLista *inicio, int valor, int *comparacoes) {
    *comparacoes = 0;
    NoLista *atual = inicio;
    while (atual != NULL) {
        (*comparacoes)++;
        if (atual->valor == valor)
            return 1;
        atual = atual->prox;
    }
    return 0;
}

void liberarLista(NoLista *inicio) {
    NoLista *atual = inicio;
    NoLista *proximo;
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
}

typedef struct NoArvore {
    int valor;
    struct NoArvore *esq, *dir;
} NoArvore;

NoArvore* novoNo(int valor) {
    NoArvore *no = (NoArvore*) malloc(sizeof(NoArvore));
    no->valor = valor;
    no->esq = no->dir = NULL;
    return no;
}

NoArvore* inserirArvore(NoArvore *raiz, int valor) {
    if (raiz == NULL)
        return novoNo(valor);
    if (valor < raiz->valor)
        raiz->esq = inserirArvore(raiz->esq, valor);
    else if (valor > raiz->valor)
        raiz->dir = inserirArvore(raiz->dir, valor);
    return raiz;
}

int buscarArvore(NoArvore *raiz, int valor, int *comparacoes) {
    *comparacoes = 0;
    NoArvore *atual = raiz;
    while (atual != NULL) {
        (*comparacoes)++;
        if (valor == atual->valor)
            return 1;
        else if (valor < atual->valor)
            atual = atual->esq;
        else
            atual = atual->dir;
    }
    return 0;
}

void liberarArvore(NoArvore *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

int main() {
    NoLista *lista = NULL;
    NoArvore *arvore = NULL;

    int i, n = 1000;
    int numeros[1000];
    srand(time(NULL));

    // Abrir arquivo para salvar os dados
    FILE *arquivo = fopen("dados_comparacoes.csv", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo!\n");
        return 1;
    }
    
    // Cabeçalho do CSV
    fprintf(arquivo, "busca_numero,valor_buscado,comparacoes_lista,comparacoes_arvore\n");

    printf("Inserindo %d numeros aleatorios...\n", n);
    for (i = 0; i < n; i++) {
        int valor = rand() % 5000;
        numeros[i] = valor;
        lista = inserirLista(lista, valor);
        arvore = inserirArvore(arvore, valor);
    }

    printf("Estruturas preenchidas.\n\n");

    for (int j = 0; j < 1000; j++) {
        int valor = rand() % 5000;
        int compLista = 0, compArvore = 0;

        buscarLista(lista, valor, &compLista);
        buscarArvore(arvore, valor, &compArvore);
        
        // Salvar no arquivo CSV
        fprintf(arquivo, "%d,%d,%d,%d\n", j+1, valor, compLista, compArvore);
        
        printf("Busca %d: Valor %d - Lista: %d comparacoes, Arvore: %d comparacoes\n", 
               j+1, valor, compLista, compArvore);
    }

    fclose(arquivo);
    printf("\nDados salvos em 'dados_comparacoes.csv'\n");

    liberarLista(lista);
    liberarArvore(arvore);
    printf("Memoria liberada.\n");

    return 0;
}
