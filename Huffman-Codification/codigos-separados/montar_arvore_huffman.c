#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

/*
 ============================================================================
 PARTE 1: CRIAÇÃO DA LISTA ENCADEADA DE FREQUÊNCIA E ORDENAÇÃO CRESCENTE
 ============================================================================
 

// Estrutura do nó da lista de frequência
struct No {
    unsigned char simbolo;        // Caractere/símbolo (0x00 a 0xFF)
    int frequencia;              // Frequência do símbolo
    struct No* proximo;          // Ponteiro para próximo nó na lista
    struct No* esquerdo;         // Ponteiro esquerdo (para árvore binária)
    struct No* direito;          // Ponteiro direito (para árvore binária)
};

// Função para criar um novo nó da lista de frequência
struct No* criarNo(unsigned char simbolo, int frequencia) {
    // Alocar memória para o novo nó
    struct No* novoNo = (struct No*)malloc(sizeof(struct No));
    if (novoNo == NULL) {
        printf("Erro na alocação de memória.\n");
        exit(1);
    }
    
    // Inicializar os campos do nó
    novoNo->simbolo = simbolo;
    novoNo->frequencia = frequencia;
    novoNo->proximo = NULL;
    novoNo->esquerdo = NULL;
    novoNo->direito = NULL;
    return novoNo;
}

// Procedimento para contar frequências de QUALQUER arquivo binário
void contarFrequenciasArquivo(FILE* arquivo, int frequencias[256]) {
    // Inicializar todo o array com zeros
    for (int i = 0; i < 256; i++) {
        frequencias[i] = 0;
    }
    
    unsigned char byte;
    // Ler cada byte do arquivo até o final (incluindo bytes 0x00)
    while (fread(&byte, 1, 1, arquivo) == 1) {
        frequencias[byte]++;  // Incrementa a frequência do byte lido
    }
}

// Função para inserir nó na lista mantendo a ordenação por frequência
struct No* inserirOrdenado(struct No* cabeca, struct No* novoNo) {
    // Caso 1: Lista vazia ou novo nó deve ser a nova cabeça
    if (cabeca == NULL || novoNo->frequencia < cabeca->frequencia) {
        novoNo->proximo = cabeca;
        return novoNo;
    }
    
    // Caso 2: Percorrer a lista para encontrar posição correta
    struct No* atual = cabeca;
    while (atual->proximo != NULL && atual->proximo->frequencia <= novoNo->frequencia) {
        atual = atual->proximo;
    }
    
    // Inserir o novo nó na posição encontrada
    novoNo->proximo = atual->proximo;
    atual->proximo = novoNo;
    return cabeca;
}

// Função para construir a lista de frequência a partir do array de frequências
struct No* construirListaFrequencia(int frequencias[256]) {
    struct No* cabeca = NULL;
    
    // Percorrer todos os 256 valores possíveis de byte
    for (int i = 0; i < 256; i++) {
        // Verificar se o byte aparece pelo menos uma vez
        if (frequencias[i] > 0) {
            // Criar novo nó para o símbolo com sua frequência
            struct No* novoNo = criarNo((unsigned char)i, frequencias[i]);
            
            // Inserir na lista mantendo ordenação por frequência
            cabeca = inserirOrdenado(cabeca, novoNo);
        }
    }
    return cabeca;
}

*/

/*
 ============================================================================
 PARTE 2: CONSTRUÇÃO DA ÁRVORE DE HUFFMAN
 ============================================================================
*/

// Função para remover o primeiro nó da lista
struct No* removerPrimeiroNo(struct No** cabeca) {
    if (*cabeca == NULL) {
        return NULL;
    }
    
    struct No* primeiro = *cabeca;      // Primeiro nó a ser removido
    *cabeca = (*cabeca)->proximo;          // Atualiza cabeça para o próximo
    primeiro->proximo = NULL;          // Isola o nó removido
    
    return primeiro;
}

// Procedimento para construir a árvore de Huffman (SEM ponteiro para ponteiro)
struct No* construirArvoreHuffman(struct No* cabeca) {
    // Continuar enquanto houver mais de 1 nó na lista
    while (cabeca != NULL && cabeca->proximo != NULL) {
        
        // Passo 1: Remover os dois primeiros nós da lista
        struct No* primeiro = removerPrimeiroNo(&cabeca);
        
        // Atualizar cabeça após remover primeiro nó
        if (primeiro != NULL) {
            cabeca = primeiro->proximo;
        } else {
            cabeca = NULL;
        }
        
        struct No* segundo = removerPrimeiroNo(&cabeca);
        
        // Atualizar cabeça após remover segundo nó
        if (segundo != NULL) {
            cabeca = segundo->proximo;
        } else {
            cabeca = NULL;
        }
        
        // Verificar se ambos os nós foram removidos com sucesso
        if (primeiro == NULL || segundo == NULL) {
            printf("Erro: Não foi possível remover os dois primeiros nós.\n");
            break;
        }
        
        // Passo 2: Criar novo nó interno com '*' (nó da árvore)
        struct No* novoNo = criarNo('*', primeiro->frequencia + segundo->frequencia);
        
        // Passo 3: Configurar os ponteiros esquerdo e direito
        novoNo->esquerdo = primeiro;
        novoNo->direito = segundo;
        
        // Passo 4: Inserir o novo nó de volta na lista ordenadamente
        cabeca = inserirOrdenado(cabeca, novoNo);
    }
    
    // Retornar a nova cabeça (que será a raiz da árvore)
    return cabeca;
}

// Procedimento para imprimir a árvore em PRÉ-ORDEM (com caractere de escape '\')
void imprimirArvorePreOrdem(struct No* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    // Se for nó interno (*) ou caractere especial, usar escape
    if (raiz->simbolo == '*' || raiz->simbolo == '\\') {
        printf("\\%c", raiz->simbolo);
    }
    // Se for caractere imprimível normal
    else if (raiz->simbolo >= 32 && raiz->simbolo <= 126) {
        printf("%c", raiz->simbolo);
    }
    // Se for caractere não imprimível, mostrar com escape hexadecimal
    else {
        printf("\\x%02X", raiz->simbolo);
    }
    
    // Recursão para esquerdo e direito
    imprimirArvorePreOrdem(raiz->esquerdo);
    imprimirArvorePreOrdem(raiz->direito);
}

// Procedimento para liberar a árvore inteira (pós-ordem)
void liberarArvore(struct No* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    // Liberar subárvores primeiro
    liberarArvore(raiz->esquerdo);
    liberarArvore(raiz->direito);
    
    // Liberar o nó atual
    free(raiz);
}

// Função para imprimir a lista de frequência
void imprimirListaFrequencia(struct No* cabeca) {
    struct No* atual = cabeca;
    printf("Lista de Frequência (válida para qualquer formato):\n");
    
    while (atual != NULL) {
        // Verificar se é caractere ASCII imprimível
        if (atual->simbolo >= 32 && atual->simbolo <= 126) {
            printf("  '%c'  = %d\n", atual->simbolo, atual->frequencia);
        } else {
            // Mostrar em hexadecimal para bytes não-printáveis
            printf("  0x%02X = %d\n", atual->simbolo, atual->frequencia);
        }
        atual = atual->proximo;
    }
}

// Função principal
int main() {
    setlocale(LC_ALL, "Portuguese");
    
    // Abrir arquivo em modo binário para ler QUALQUER formato
    char* nomeArquivo = "arquivo.bin"; // Pode ser .jpg, .mp3, .exe, etc.
    FILE* arquivo = fopen(nomeArquivo, "rb"); // "rb" = read binary
    
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo: %s\n", nomeArquivo);
        return 1;
    }
    
    int frequencias[256];
    
    // PARTE 1.1: Contar frequências de todos os bytes do arquivo
    contarFrequenciasArquivo(arquivo, frequencias);
    fclose(arquivo);
    
    // PARTE 1.2: Construir lista ordenada de frequências
    struct No* lista = construirListaFrequencia(frequencias);
    
    printf("=== LISTA DE FREQUÊNCIA ===\n");
    imprimirListaFrequencia(lista);
    
    // PARTE 2: Construir árvore de Huffman (SEM ponteiro para ponteiro)
    printf("\n=== CONSTRUINDO ÁRVORE DE HUFFMAN ===\n");
    struct No* raiz = construirArvoreHuffman(lista);
    
    printf("\n=== ÁRVORE EM PRÉ-ORDEM ===\n");
    imprimirArvorePreOrdem(raiz);
    printf("\n");
    
    // Liberar memória
    liberarArvore(raiz);
    
    return 0;
}
