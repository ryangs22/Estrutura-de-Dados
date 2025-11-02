#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

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

/*
 ============================================================================
 PARTE 2: CONSTRUÇÃO DA ÁRVORE DE HUFFMAN
 ============================================================================


// Função para remover o primeiro nó da lista (CORRIGIDA)
struct No* removerPrimeiroNo(struct No** cabeca) {
    if (*cabeca == NULL) {
        return NULL;
    }
    
    struct No* primeiro = *cabeca;      // Primeiro nó a ser removido
    *cabeca = (*cabeca)->proximo;       // Atualiza cabeça para o próximo
    primeiro->proximo = NULL;           // Isola o nó removido
    
    return primeiro;
}

// Procedimento para construir a árvore de Huffman (CORRIGIDA)
struct No* construirArvoreHuffman(struct No* cabeca) {
    if (cabeca == NULL) {
        printf("Erro: Lista de frequência vazia.\n");
        return NULL;
    }
    
    // Continuar enquanto houver mais de 1 nó na lista
    while (cabeca != NULL && cabeca->proximo != NULL) {
        
        // Passo 1: Remover os dois primeiros nós da lista
        struct No* primeiro = removerPrimeiroNo(&cabeca);
        struct No* segundo = removerPrimeiroNo(&cabeca);
        
        // Verificar se ambos os nós foram removidos com sucesso
        if (primeiro == NULL || segundo == NULL) {
            printf("Erro: Não foi possível remover os dois primeiros nós.\n");
            if (primeiro != NULL) free(primeiro);
            if (segundo != NULL) free(segundo);
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

// Função para imprimir a lista de frequência COM TABELA ASCII COMPLETA
void imprimirListaFrequencia(struct No* cabeca) {
    struct No* atual = cabeca;
    printf("=== TABELA DE FREQUÊNCIAS (ASCII COMPLETO 0-255) ===\n");
    
    while (atual != NULL) {
        // Verificar se é caractere ASCII imprimível
        if (atual->simbolo >= 32 && atual->simbolo <= 126) {
            printf("%3d = %2d = '%c'\n", atual->simbolo, atual->frequencia, atual->simbolo);
        } else {
            // Mostrar em hexadecimal para bytes não-printáveis
            printf("%3d = %2d = 0x%02X\n", atual->simbolo, atual->frequencia, atual->simbolo);
        }
        atual = atual->proximo;
    }
}

/*
 ============================================================================
 PARTE 3: CRIAÇÃO DO DICIONÁRIO DE CÓDIGOS HUFFMAN
 ============================================================================


// Função para calcular altura máxima da árvore
void calcularAlturaArvore(struct No* raiz, int* altura) {
    if (raiz == NULL) {
        *altura = -1;
        return;
    }
    
    int alturaEsquerda, alturaDireita;
    
    // Calcular altura da subárvore esquerda
    calcularAlturaArvore(raiz->esquerdo, &alturaEsquerda);
    
    // Calcular altura da subárvore direita  
    calcularAlturaArvore(raiz->direito, &alturaDireita);
    
    // Determinar a maior altura
    if (alturaEsquerda > alturaDireita) {
        *altura = alturaEsquerda + 1;
    } else {
        *altura = alturaDireita + 1;
    }
}

// Função para alocar a matriz dicionário
void alocarDicionario(char* dicionario[256], int colunas) {
    for (int i = 0; i < 256; i++) {
        // Alocar cada linha com número específico de colunas
        dicionario[i] = (char*)malloc(colunas * sizeof(char));
        
        // Verificar se a alocação foi bem-sucedida
        if (dicionario[i] == NULL) {
            printf("Erro: Falha ao alocar dicionario[%d]\n", i);
            // Liberar alocações anteriores em caso de erro
            for (int j = 0; j < i; j++) {
                free(dicionario[j]);
            }
            exit(1);
        }
        
        // Inicializar com string vazia
        dicionario[i][0] = '\0';
    }
}

// Função recursiva para gerar os códigos Huffman (COM SEGURANÇA)
void gerarDicionarioRecursivo(char* dicionario[256], struct No* no, char concatenacao[], int profundidade, int colunas) {
    if (no == NULL) {
        return;
    }
    
    // Verificar limite de profundidade para evitar estouro
    if (profundidade >= colunas - 1) {
        printf("Aviso: Profundidade máxima atingida para o símbolo 0x%02X\n", no->simbolo);
        return;
    }
    
    // Se é nó folha, salvar o código no dicionário
    if (no->esquerdo == NULL && no->direito == NULL) {
        // Finalizar a string do código
        concatenacao[profundidade] = '\0';
        
        // Copiar o código para a posição correspondente no dicionário
        strcpy(dicionario[no->simbolo], concatenacao);
        return;
    }
    
    // Explorar subárvore esquerda (adiciona '0')
    if (no->esquerdo != NULL) {
        concatenacao[profundidade] = '0';
        gerarDicionarioRecursivo(dicionario, no->esquerdo, concatenacao, profundidade + 1, colunas);
    }
    
    // Explorar subárvore direita (adiciona '1')
    if (no->direito != NULL) {
        concatenacao[profundidade] = '1';
        gerarDicionarioRecursivo(dicionario, no->direito, concatenacao, profundidade + 1, colunas);
    }
}

// Função para gerar o dicionário completo
void gerarDicionario(char* dicionario[256], struct No* raiz, int colunas) {
    // Criar array temporário para construção dos códigos
    char concatenacao[colunas];
    
    // Iniciar a geração recursiva
    gerarDicionarioRecursivo(dicionario, raiz, concatenacao, 0, colunas);
}

// Função para liberar a memória do dicionário
void liberarDicionario(char* dicionario[256]) {
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL) {
            free(dicionario[i]);
            dicionario[i] = NULL;
        }
    }
}

// Função para imprimir o dicionário
void imprimirDicionario(char* dicionario[256]) {
    printf("=== DICIONÁRIO DE CÓDIGOS HUFFMAN ===\n");
    printf("Símbolo | Código\n");
    printf("--------|-------\n");
    
    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL && dicionario[i][0] != '\0') {
            if (i >= 32 && i <= 126) {
                printf("  '%c'   |  %s\n", i, dicionario[i]);
            } else {
                printf("  0x%02X |  %s\n", i, dicionario[i]);
            }
            count++;
        }
    }
    printf("Total de símbolos no dicionário: %d\n", count);
}


/*
 ============================================================================
 PARTE 4: CODIFICAÇÃO DO TEXTO/ARQUIVO
 ============================================================================
*/

// Função para codificar um arquivo usando o dicionário
void codificarArquivo(FILE* arquivo_entrada, char* dicionario[256], const char* nome_saida) {
    // Voltar ao início do arquivo
    fseek(arquivo_entrada, 0, SEEK_SET);
    
    // Abrir arquivo de saída
    FILE* arquivo_saida = fopen(nome_saida, "w");
    if (arquivo_saida == NULL) {
        printf("Erro ao criar arquivo de saída: %s\n", nome_saida);
        return;
    }
    
    unsigned char byte;
    long total_bits = 0;
    
    // Ler cada byte do arquivo e codificar
    while (fread(&byte, 1, 1, arquivo_entrada) == 1) {
        char* codigo = dicionario[byte];
        if (codigo != NULL && codigo[0] != '\0') {
            fprintf(arquivo_saida, "%s", codigo);
            total_bits += strlen(codigo);
        }
    }
    
    fclose(arquivo_saida);
}

// Função para mostrar o texto codificado na tela
void mostrarTextoCodificado(FILE* arquivo, char* dicionario[256]) {
    // Voltar ao início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    printf("=== TEXTO CODIFICADO ===\n");
    
    unsigned char byte;
    while (fread(&byte, 1, 1, arquivo) == 1) {
        char* codigo = dicionario[byte];
        if (codigo != NULL && codigo[0] != '\0') {
            printf("%s", codigo);
        }
    }
    printf("\n");
}

/*
 ============================================================================
 PARTE 5: DECODIFICAÇÃO DO TEXTO/ARQUIVO
 ============================================================================
*/

// Função para decodificar um arquivo codificado usando a árvore de Huffman
void decodificarArquivo(const char* arquivo_codificado, struct No* raiz, const char* nome_saida) {
    FILE* arquivo_entrada = fopen(arquivo_codificado, "r");
    if (arquivo_entrada == NULL) {
        printf("Erro ao abrir arquivo codificado: %s\n", arquivo_codificado);
        return;
    }
    
    FILE* arquivo_saida = fopen(nome_saida, "wb");
    if (arquivo_saida == NULL) {
        printf("Erro ao criar arquivo de saída: %s\n", nome_saida);
        fclose(arquivo_entrada);
        return;
    }
    
    struct No* atual = raiz;
    char bit;
    int caracteres_decodificados = 0;
    
    // Ler cada bit do arquivo codificado
    while ((bit = fgetc(arquivo_entrada)) != EOF) {
        if (bit == '0') {
            atual = atual->esquerdo;
        } else if (bit == '1') {
            atual = atual->direito;
        } else {
            // Ignorar caracteres que não são 0 ou 1
            continue;
        }
        
        // Se chegamos a uma folha, escrever o símbolo
        if (atual->esquerdo == NULL && atual->direito == NULL) {
            fwrite(&atual->simbolo, 1, 1, arquivo_saida);
            caracteres_decodificados++;
            atual = raiz; // Voltar para a raiz para o próximo símbolo
        }
    }
    
    fclose(arquivo_entrada);
    fclose(arquivo_saida);
}

// Função para mostrar o texto decodificado na tela (VERSÃO ROBUSTA)
void mostrarTextoDecodificado(const char* arquivo_codificado, struct No* raiz) {
    // Primeiro decodifica para um arquivo temporário
    decodificarArquivo(arquivo_codificado, raiz, "temp_decodificado.txt");
    
    // Depois lê e imprime o arquivo temporário
    FILE* temp = fopen("temp_decodificado.txt", "rb");
    if (temp == NULL) {
        printf("Erro ao abrir arquivo temporário\n");
        return;
    }
    
    printf("=== TEXTO DECODIFICADO ===\n");
    
    unsigned char buffer[1000];
    size_t bytes_lidos;
    
    // Ler e imprimir em chunks
    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), temp)) > 0) {
        fwrite(buffer, 1, bytes_lidos, stdout);
    }
    
    printf("\n");
    fclose(temp);
    
    // Remover arquivo temporário
    remove("temp_decodificado.txt");
}


// Função para ler e mostrar o conteúdo de um arquivo no terminal
void mostrarConteudoArquivo(const char* nome_arquivo, const char* titulo) {
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo: %s\n", nome_arquivo);
        return;
    }
    
    printf("\n=== %s ===\n", titulo);
    
    unsigned char byte;
    while (fread(&byte, 1, 1, arquivo) == 1) {
        printf("%c", byte);
    }
    printf("\n");
    
    fclose(arquivo);
}

// Função principal atualizada
int main() {
    setlocale(LC_ALL, "Portuguese");
    
    // TESTE COM STRING QUE INCLUI CARACTERES ESPECIAIS
    const char* texto_teste = "Vamos aprender programação";
    
    // Criar arquivo temporário para teste
    FILE* arquivo_teste = fopen("teste.txt", "wb");
    if (arquivo_teste != NULL) {
        fwrite(texto_teste, sizeof(char), strlen(texto_teste), arquivo_teste);
        fclose(arquivo_teste);
    }
    
    // Abrir arquivo em modo binário
    char* nomeArquivo = "teste.txt";
    FILE* arquivo = fopen(nomeArquivo, "rb");
    
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo: %s\n", nomeArquivo);
        return 1;
    }
    
    int frequencias[256];
    
    // PARTE 1.1: Contar frequências de todos os bytes do arquivo
    contarFrequenciasArquivo(arquivo, frequencias);
    
    // PARTE 1.2: Construir lista ordenada de frequências
    struct No* lista = construirListaFrequencia(frequencias);
    
    if (lista == NULL) {
        printf("Lista vazia - nenhum símbolo encontrado!\n");
        fclose(arquivo);
        return 1;
    }
    
    // PRINT 1: Lista de frequência
    printf("\n");
    imprimirListaFrequencia(lista);
    
    // PARTE 2: Construir árvore de Huffman
    struct No* raiz = construirArvoreHuffman(lista);
    
    if (raiz == NULL) {
        printf("ERRO: Árvore não foi construída!\n");
        liberarArvore(lista);
        fclose(arquivo);
        return 1;
    }
    
    // PRINT 2: Árvore em pré-ordem
    printf("\n=== ÁRVORE EM PRÉ-ORDEM ===\n");
    imprimirArvorePreOrdem(raiz);
    printf("\n");
    
    // PARTE 3: Criar dicionário de códigos Huffman
    int altura;
    calcularAlturaArvore(raiz, &altura);
    int colunas = altura + 2;
    
    char* dicionario[256] = {NULL};
    alocarDicionario(dicionario, colunas);
    gerarDicionario(dicionario, raiz, colunas);
    
    // PRINT 3: Dicionário de códigos
    printf("\n");
    imprimirDicionario(dicionario);
    
    // PARTE 4: Codificação
    // PRINT 4: Texto codificado
    printf("\n");
    mostrarTextoCodificado(arquivo, dicionario);
    
    // Codificar e salvar em arquivo
    codificarArquivo(arquivo, dicionario, "texto_codificado.txt");
    
    // Fechar arquivo de entrada
    fclose(arquivo);
    
    // PARTE 5: Decodificação
    // PRINT 5: Texto decodificado
    printf("\n");
    mostrarTextoDecodificado("texto_codificado.txt", raiz);
    
    // Decodificar e salvar em arquivo
    decodificarArquivo("texto_codificado.txt", raiz, "texto_decodificado.txt");
    
    // Liberar memória
    liberarDicionario(dicionario);
    liberarArvore(raiz);
    
    // Remover arquivo temporário
    remove("teste.txt");
    
    return 0;
}
