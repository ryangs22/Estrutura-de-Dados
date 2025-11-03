#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// estrutura de um nó para lista encadeada simples
typedef struct NoLista {
    int valor;                // armazena o valor inteiro do nó
    struct NoLista *prox;     // ponteiro para o próximo nó da lista
} NoLista;

// insere um novo elemento no início da lista encadeada
// retorna o ponteiro para o novo início da lista
NoLista* inserirLista(NoLista *inicio, int valor) {
    // aloca memória para o novo nó
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));
    novo->valor = valor;      // atribui o valor ao novo nó
    novo->prox = inicio;      // o novo nó aponta para o antigo início
    return novo;              // retorna o novo início da lista
}

// busca um valor na lista encadeada de forma sequencial
// retorna 1 se encontrou, 0 caso contrário
// também conta quantas comparações foram necessárias
int buscarLista(NoLista *inicio, int valor, int *comparacoes) {
    *comparacoes = 0;         // inicializa o contador de comparações
    NoLista *atual = inicio;  // ponteiro para percorrer a lista
    
    // percorre toda a lista até o final
    while (atual != NULL) {
        (*comparacoes)++;     // incrementa o contador a cada comparação
        if (atual->valor == valor)
            return 1;         // valor encontrado
        atual = atual->prox;  // avança para o próximo nó
    }
    return 0;                 // valor não encontrado
}

// libera toda a memória alocada para a lista encadeada
void liberarLista(NoLista *inicio) {
    NoLista *atual = inicio;
    NoLista *proximo;
    
    // percorre a lista liberando cada nó
    while (atual != NULL) {
        proximo = atual->prox;  // guarda o próximo antes de liberar
        free(atual);            // libera o nó atual
        atual = proximo;        // avança para o próximo
    }
}

// estrutura de um nó para árvore binária de busca
typedef struct NoArvore {
    int valor;                      // armazena o valor inteiro do nó
    struct NoArvore *esq, *dir;     // ponteiros para subárvores esquerda e direita
} NoArvore;

// cria e retorna um novo nó de árvore
NoArvore* novoNo(int valor) {
    // aloca memória para o novo nó
    NoArvore *no = (NoArvore*) malloc(sizeof(NoArvore));
    no->valor = valor;        // atribui o valor
    no->esq = no->dir = NULL; // inicializa os filhos como nulos
    return no;
}

// insere um valor na árvore binária de busca de forma recursiva
// valores menores vão para a esquerda, maiores para a direita
NoArvore* inserirArvore(NoArvore *raiz, int valor) {
    // caso base: posição vazia encontrada
    if (raiz == NULL)
        return novoNo(valor);
    
    // valor menor que a raiz: insere na subárvore esquerda
    if (valor < raiz->valor)
        raiz->esq = inserirArvore(raiz->esq, valor);
    // valor maior que a raiz: insere na subárvore direita
    else if (valor > raiz->valor)
        raiz->dir = inserirArvore(raiz->dir, valor);
    // obs: valores iguais são ignorados (não inseridos novamente)
    
    return raiz;
}

// busca um valor na árvore binária de forma iterativa
// retorna 1 se encontrou, 0 caso contrário
// também conta quantas comparações foram necessárias
int buscarArvore(NoArvore *raiz, int valor, int *comparacoes) {
    *comparacoes = 0;         // inicializa o contador de comparações
    NoArvore *atual = raiz;   // ponteiro para percorrer a árvore
    
    // percorre a árvore até encontrar ou chegar em nó nulo
    while (atual != NULL) {
        (*comparacoes)++;     // incrementa o contador a cada comparação
        
        if (valor == atual->valor)
            return 1;         // valor encontrado
        else if (valor < atual->valor)
            atual = atual->esq;  // busca na subárvore esquerda
        else
            atual = atual->dir;  // busca na subárvore direita
    }
    return 0;                 // valor não encontrado
}

// libera toda a memória alocada para a árvore de forma recursiva (pós-ordem)
void liberarArvore(NoArvore *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarArvore(raiz->esq);  // libera subárvore esquerda
    liberarArvore(raiz->dir);  // libera subárvore direita
    free(raiz);                // libera o nó atual
}

int main() {
    NoLista *lista = NULL;     // inicializa lista vazia
    NoArvore *arvore = NULL;   // inicializa árvore vazia
    int i, n = 1000;           // n é a quantidade de elementos a inserir
    int numeros[1000];         // array para armazenar os números gerados
    
    srand(time(NULL));         // inicializa gerador de números aleatórios
    
    // abre arquivo csv para salvar os resultados das comparações
    FILE *arquivo = fopen("dados_comparacoes.csv", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo!\n");
        return 1;
    }
    
    // escreve o cabeçalho do arquivo csv
    fprintf(arquivo, "busca_numero,valor_buscado,comparacoes_lista,comparacoes_arvore\n");
    
    printf("Inserindo %d numeros aleatorios...\n", n);
    
    // insere 1000 números aleatórios nas duas estruturas
    for (i = 0; i < n; i++) {
        int valor = rand() % 5000;   // gera número aleatório entre 0 e 4999
        numeros[i] = valor;          // armazena no array
        lista = inserirLista(lista, valor);      // insere na lista
        arvore = inserirArvore(arvore, valor);   // insere na árvore
    }
    
    printf("Estruturas preenchidas.\n\n");
    
    // realiza 1000 buscas e compara o desempenho das estruturas
    for (int j = 0; j < 1000; j++) {
        int valor = rand() % 5000;   // gera valor aleatório para buscar
        int compLista = 0, compArvore = 0;  // contadores de comparações
        
        // realiza a busca em ambas as estruturas
        buscarLista(lista, valor, &compLista);
        buscarArvore(arvore, valor, &compArvore);
        
        // salva os resultados no arquivo csv
        fprintf(arquivo, "%d,%d,%d,%d\n", j+1, valor, compLista, compArvore);
        
        // exibe os resultados na tela
        printf("Busca %d: Valor %d - Lista: %d comparacoes, Arvore: %d comparacoes\n", 
               j+1, valor, compLista, compArvore);
    }
    
    fclose(arquivo);  // fecha o arquivo
    printf("\nDados salvos em 'dados_comparacoes.csv'\n");
    
    // libera toda a memória alocada
    liberarLista(lista);
    liberarArvore(arvore);
    
    printf("Memoria liberada.\n");
    return 0;
}
