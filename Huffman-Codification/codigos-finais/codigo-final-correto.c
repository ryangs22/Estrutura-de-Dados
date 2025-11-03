/**
 * @file huffman_compressor.c
 * @brief Implementação completa do Algoritmo de Huffman para compactação e descompactação
 * @details Este programa implementa o algoritmo de Huffman para compactação e 
 *          descompactação de arquivos de qualquer formato. O arquivo compactado 
 *          segue o padrão .huff com cabeçalho específico contendo:
 *          - 3 bits para indicar bits de lixo
 *          - 13 bits para o tamanho da árvore
 *          - Representação da árvore em pré-ordem
 *          - Dados compactados em formato binário
 * @author [Seu Nome]
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

/*
 ============================================================================
 ESTRUTURAS DE DADOS GENÉRICAS
 ============================================================================
*/

/**
 * @struct ListaNo
 * @brief Nó genérico de lista encadeada
 * 
 * @var ListaNo::item
 * Ponteiro genérico para o item armazenado no nó
 * 
 * @var ListaNo::proximo
 * Ponteiro para o próximo nó da lista
 */
struct ListaNo {
    void* item;
    struct ListaNo* proximo;
};

/**
 * @struct No
 * @brief Representa um nó na árvore de Huffman
 * 
 * @var No::simbolo
 * Byte/caractere representado por este nó (0-255). Para nós internos, usa '*'
 * 
 * @var No::frequencia
 * Frequência de ocorrência do símbolo no arquivo original
 * 
 * @var No::esquerdo
 * Ponteiro para subárvore esquerda (representa bit '0')
 * 
 * @var No::direito
 * Ponteiro para subárvore direita (representa bit '1')
 */
struct No {
    unsigned char simbolo;
    int frequencia;
    struct No* esquerdo;
    struct No* direito;
};

/*
 ============================================================================
 OPERAÇÕES GENÉRICAS DA LISTA ENCADEADA
 ============================================================================
*/

/**
 * @brief Cria um novo nó de lista encadeada
 * 
 * @param item Ponteiro genérico para o item a ser armazenado
 * @return Ponteiro para o novo nó criado
 * 
 * @note Encerra o programa em caso de falha na alocação
 */
struct ListaNo* criarListaNo(void* item) {
    struct ListaNo* novoNo = (struct ListaNo*)malloc(sizeof(struct ListaNo));
    if (novoNo == NULL) {
        printf("Erro na alocação de memória para ListaNo.\n");
        exit(1);
    }
    novoNo->item = item;
    novoNo->proximo = NULL;
    return novoNo;
}

/**
 * @brief Insere um item de forma ordenada na lista encadeada
 * 
 * @param cabeca Ponteiro para o primeiro nó da lista
 * @param item Item a ser inserido
 * @param comparar Função de comparação que retorna <0 se a<b, 0 se a==b, >0 se a>b
 * @return Ponteiro para a nova cabeça da lista (pode mudar se inserido no início)
 * 
 * @details A lista é mantida em ordem crescente de acordo com a função de comparação
 */
struct ListaNo* inserirOrdenado(struct ListaNo* cabeca, void* item, 
                               int (comparar)(void*, void*)) {
    struct ListaNo* novoNo = criarListaNo(item);
    
    if (cabeca == NULL || comparar(item, cabeca->item) < 0) {
        novoNo->proximo = cabeca;
        return novoNo;
    }
    
    struct ListaNo* atual = cabeca;
    while (atual->proximo != NULL && 
           comparar(item, atual->proximo->item) >= 0) {
        atual = atual->proximo;
    }
    
    novoNo->proximo = atual->proximo;
    atual->proximo = novoNo;
    return cabeca;
}

/**
 * @brief Remove e retorna o primeiro item da lista
 * 
 * @param cabeca Ponteiro duplo para a cabeça da lista
 * @return Ponteiro para o item removido, ou NULL se a lista estiver vazia
 * 
 * @note O nó é liberado, mas o item não (responsabilidade do chamador)
 */
void* removerPrimeiroNo(struct ListaNo** cabeca) {
    if (*cabeca == NULL) {
        return NULL;
    }
    
    struct ListaNo* primeiro = *cabeca;
    void* item = primeiro->item;
    *cabeca = primeiro->proximo;
    free(primeiro);
    
    return item;
}

/**
 * @brief Libera toda a memória da lista encadeada
 * 
 * @param cabeca Ponteiro para o primeiro nó da lista
 * 
 * @note Libera apenas os nós da lista, não os itens contidos neles
 */
void liberarLista(struct ListaNo* cabeca) {
    while (cabeca != NULL) {
        struct ListaNo* temp = cabeca;
        cabeca = cabeca->proximo;
        free(temp);
    }
}

/**
 * @brief Calcula o número de elementos na lista
 * 
 * @param cabeca Ponteiro para o primeiro nó da lista
 * @return Quantidade de elementos na lista
 */
int tamanhoLista(struct ListaNo* cabeca) {
    int count = 0;
    while (cabeca != NULL) {
        count++;
        cabeca = cabeca->proximo;
    }
    return count;
}

/*
 ============================================================================
 OPERAÇÕES ESPECÍFICAS DO NÓ DE HUFFMAN
 ============================================================================
*/

/**
 * @brief Cria um novo nó da árvore de Huffman
 * 
 * @param simbolo Byte/caractere que o nó representa
 * @param frequencia Frequência de ocorrência do símbolo
 * @return Ponteiro para o novo nó criado
 * 
 * @note Encerra o programa em caso de falha na alocação
 */
struct No* criarNo(unsigned char simbolo, int frequencia) {
    struct No* novoNo = (struct No*)malloc(sizeof(struct No));
    if (novoNo == NULL) {
        printf("Erro na alocação de memória para No.\n");
        exit(1);
    }
    
    novoNo->simbolo = simbolo;
    novoNo->frequencia = frequencia;
    novoNo->esquerdo = NULL;
    novoNo->direito = NULL;
    return novoNo;
}

/**
 * @brief Função de comparação para ordenar nós por frequência
 * 
 * @param a Ponteiro para o primeiro nó
 * @param b Ponteiro para o segundo nó
 * @return Diferença entre as frequências (negativo se a<b, positivo se a>b)
 */
int compararNos(void* a, void* b) {
    struct No* noA = (struct No*)a;
    struct No* noB = (struct No*)b;
    return noA->frequencia - noB->frequencia;
}

/*
 ============================================================================
 PARTE 1: CRIAÇÃO DA LISTA DE FREQUÊNCIAS
 ============================================================================
*/

/**
 * @brief Conta a frequência de cada byte no arquivo
 * 
 * @param arquivo Ponteiro para o arquivo aberto em modo leitura binária
 * @param frequencias Array de 256 posições que será preenchido com as frequências
 * 
 * @details Lê o arquivo byte a byte e incrementa o contador correspondente.
 *          Suporta todos os valores de 0 a 255 (ASCII estendido).
 */
void contarFrequenciasArquivo(FILE* arquivo, int frequencias[256]) {
    for (int i = 0; i < 256; i++) {
        frequencias[i] = 0;
    }
    
    unsigned char byte;
    while (fread(&byte, 1, 1, arquivo) == 1) {
        frequencias[byte]++;
    }
}

/**
 * @brief Constrói uma lista encadeada ordenada de nós com base nas frequências
 * 
 * @param frequencias Array de 256 posições contendo as frequências de cada byte
 * @return Ponteiro para a cabeça da lista ordenada por frequência crescente
 * 
 * @details Cria um nó para cada byte que aparece pelo menos uma vez no arquivo
 *          e os insere em ordem crescente de frequência
 */
struct ListaNo* construirListaFrequencia(int frequencias[256]) {
    struct ListaNo* cabeca = NULL;
    
    for (int i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            struct No* novoNo = criarNo((unsigned char)i, frequencias[i]);
            cabeca = inserirOrdenado(cabeca, novoNo, compararNos);
        }
    }
    return cabeca;
}

/*
 ============================================================================
 PARTE 2: CONSTRUÇÃO DA ÁRVORE DE HUFFMAN
 ============================================================================
*/

/**
 * @brief Constrói a árvore de Huffman a partir da lista de frequências
 * 
 * @param cabeca Lista encadeada ordenada de nós por frequência
 * @return Ponteiro para a raiz da árvore de Huffman construída
 * 
 * @details Algoritmo de Huffman:
 *          1. Remove os dois nós de menor frequência
 *          2. Cria um nó pai com frequência = soma dos filhos
 *          3. Insere o nó pai de volta na lista ordenada
 *          4. Repete até restar apenas um nó (a raiz)
 * 
 * @note A lista original é destruída no processo
 */
struct No* construirArvoreHuffman(struct ListaNo* cabeca) {
    if (cabeca == NULL) {
        printf("Erro: Lista de frequência vazia.\n");
        return NULL;
    }
    
    while (tamanhoLista(cabeca) > 1) {
        
        struct No* primeiro = (struct No*)removerPrimeiroNo(&cabeca);
        struct No* segundo = (struct No*)removerPrimeiroNo(&cabeca);
        
        if (primeiro == NULL || segundo == NULL) {
            printf("Erro: Não foi possível remover os dois primeiros nós.\n");
            if (primeiro != NULL) free(primeiro);
            if (segundo != NULL) free(segundo);
            break;
        }
        
        struct No* novoNo = criarNo('*', primeiro->frequencia + segundo->frequencia);
        novoNo->esquerdo = primeiro;
        novoNo->direito = segundo;
        
        cabeca = inserirOrdenado(cabeca, novoNo, compararNos);
    }
    
    struct No* raiz = (struct No*)removerPrimeiroNo(&cabeca);
    return raiz;
}

/**
 * @brief Imprime a árvore de Huffman em pré-ordem
 * 
 * @param raiz Ponteiro para a raiz da árvore
 * 
 * @details Formato de impressão:
 *          - Nós internos: '*'
 *          - Folhas com caracteres imprimíveis: o próprio caractere
 *          - Folhas com caracteres não-imprimíveis: \xHH (formato hexadecimal)
 *          - Caracteres especiais: \* e \\ (com escape)
 */
void imprimirArvorePreOrdem(struct No* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    if (raiz->simbolo == '*' || raiz->simbolo == '\\') {
        printf("\\%c", raiz->simbolo);
    }
    else if (raiz->simbolo >= 32 && raiz->simbolo <= 126) {
        printf("%c", raiz->simbolo);
    }
    else {
        printf("\\x%02X", raiz->simbolo);
    }
    
    imprimirArvorePreOrdem(raiz->esquerdo);
    imprimirArvorePreOrdem(raiz->direito);
}

/**
 * @brief Libera recursivamente toda a memória da árvore
 * 
 * @param raiz Ponteiro para a raiz da árvore
 * 
 * @details Usa travessia pós-ordem para garantir que os filhos
 *          sejam liberados antes do pai
 */
void liberarArvore(struct No* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    liberarArvore(raiz->esquerdo);
    liberarArvore(raiz->direito);
    
    free(raiz);
}

/**
 * @brief Imprime a tabela de frequências de forma formatada
 * 
 * @param cabeca Ponteiro para a lista de nós com frequências
 * 
 * @details Exibe três colunas: código ASCII, frequência e representação visual
 */
void imprimirListaFrequencia(struct ListaNo* cabeca) {
    struct ListaNo* atual = cabeca;
    printf("=== TABELA DE FREQUÊNCIAS (ASCII COMPLETO 0-255) ===\n");
    
    while (atual != NULL) {
        struct No* no = (struct No*)atual->item;
        
        if (no->simbolo >= 32 && no->simbolo <= 126) {
            printf("%3d = %2d = '%c'\n", no->simbolo, no->frequencia, no->simbolo);
        } else {
            printf("%3d = %2d = 0x%02X\n", no->simbolo, no->frequencia, no->simbolo);
        }
        atual = atual->proximo;
    }
}

/*
 ============================================================================
 PARTE 3: CRIAÇÃO DO DICIONÁRIO DE CÓDIGOS HUFFMAN
 ============================================================================
*/

/**
 * @brief Calcula a altura da árvore de Huffman
 * 
 * @param raiz Ponteiro para a raiz da árvore
 * @return Altura da árvore (-1 se vazia, 0 se apenas raiz, etc.)
 * 
 * @details A altura determina o tamanho máximo dos códigos de Huffman
 */
int calcularAlturaArvore(struct No* raiz) {
    if (raiz == NULL) {
        return -1;
    }
    
    int alturaEsquerda = calcularAlturaArvore(raiz->esquerdo);
    int alturaDireita = calcularAlturaArvore(raiz->direito);
    
    return (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita) + 1;
}

/**
 * @brief Aloca memória para o dicionário de códigos
 * 
 * @param dicionario Array de 256 ponteiros para strings
 * @param colunas Tamanho máximo de cada string (baseado na altura da árvore)
 * 
 * @note Encerra o programa em caso de falha na alocação
 */
void alocarDicionario(char* dicionario[256], int colunas) {
    for (int i = 0; i < 256; i++) {
        dicionario[i] = (char*)malloc(colunas * sizeof(char));
        
        if (dicionario[i] == NULL) {
            printf("Erro: Falha ao alocar dicionario[%d]\n", i);
            for (int j = 0; j < i; j++) {
                free(dicionario[j]);
            }
            exit(1);
        }
        
        dicionario[i][0] = '\0';
    }
}

/**
 * @brief Gera recursivamente os códigos de Huffman para cada símbolo
 * 
 * @param dicionario Array que será preenchido com os códigos
 * @param no Nó atual sendo processado
 * @param concatenacao String auxiliar para construir o código atual
 * @param profundidade Profundidade atual na árvore
 * @param colunas Tamanho máximo do código
 * 
 * @details Travessia em pré-ordem:
 *          - Adiciona '0' ao descer pela esquerda
 *          - Adiciona '1' ao descer pela direita
 *          - Quando atinge uma folha, armazena o código completo
 */
void gerarDicionarioRecursivo(char* dicionario[256], struct No* no, 
                             char concatenacao[], int profundidade, int colunas) {
    if (no == NULL) {
        return;
    }
    
    if (profundidade >= colunas - 1) {
        printf("Aviso: Profundidade máxima atingida para o símbolo 0x%02X\n", no->simbolo);
        return;
    }
    
    if (no->esquerdo == NULL && no->direito == NULL) {
        concatenacao[profundidade] = '\0';
        strcpy(dicionario[no->simbolo], concatenacao);
        return;
    }
    
    if (no->esquerdo != NULL) {
        concatenacao[profundidade] = '0';
        gerarDicionarioRecursivo(dicionario, no->esquerdo, concatenacao, profundidade + 1, colunas);
    }
    
    if (no->direito != NULL) {
        concatenacao[profundidade] = '1';
        gerarDicionarioRecursivo(dicionario, no->direito, concatenacao, profundidade + 1, colunas);
    }
}

/**
 * @brief Gera o dicionário completo de códigos de Huffman
 * 
 * @param dicionario Array de 256 strings (já alocado)
 * @param raiz Raiz da árvore de Huffman
 * @param colunas Tamanho máximo de cada código
 * 
 * @details Wrapper para a função recursiva que inicializa a string auxiliar
 */
void gerarDicionario(char* dicionario[256], struct No* raiz, int colunas) {
    char concatenacao[colunas];
    gerarDicionarioRecursivo(dicionario, raiz, concatenacao, 0, colunas);
}

/**
 * @brief Libera a memória alocada para o dicionário
 * 
 * @param dicionario Array de 256 ponteiros para strings
 */
void liberarDicionario(char* dicionario[256]) {
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL) {
            free(dicionario[i]);
            dicionario[i] = NULL;
        }
    }
}

/**
 * @brief Imprime o dicionário de códigos de forma formatada
 * 
 * @param dicionario Array com os códigos de Huffman
 * 
 * @details Exibe apenas os símbolos que aparecem no arquivo original
 */
void imprimirDicionario(char* dicionario[256]) {
    printf("=== DICIONÁRIO DE CÓDIGOS HUFFMAN ===\n");
    printf("Símbolo | Código\n");
    printf("--------|-------\n");
    
    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL && dicionario[i][0] != '\0') {
            if (i >= 32 && i <= 126) {
                printf("  '%c'    |  %s\n", i, dicionario[i]);
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
 PARTE 4: CODIFICAÇÃO
 ============================================================================
*/

/**
 * @brief Codifica o arquivo usando o dicionário de Huffman
 * 
 * @param arquivo_entrada Arquivo original a ser codificado
 * @param dicionario Dicionário com os códigos de Huffman
 * @param nome_saida Nome do arquivo de saída (códigos em ASCII '0' e '1')
 * 
 * @details Lê o arquivo byte a byte e escreve os códigos correspondentes
 *          como caracteres ASCII '0' e '1' (formato intermediário)
 */
void codificarArquivo(FILE* arquivo_entrada, char* dicionario[256], const char* nome_saida) {
    fseek(arquivo_entrada, 0, SEEK_SET);
    
    FILE* arquivo_saida = fopen(nome_saida, "wb");
    if (arquivo_saida == NULL) {
        printf("Erro ao criar arquivo de saída: %s\n", nome_saida);
        return;
    }
    
    unsigned char byte;
    long total_bits = 0;
    
    while (fread(&byte, 1, 1, arquivo_entrada) == 1) {
        char* codigo = dicionario[byte];
        if (codigo != NULL && codigo[0] != '\0') {
            for (int i = 0; codigo[i] != '\0'; i++) {
                fputc(codigo[i], arquivo_saida);
                total_bits++;
            }
        }
    }
    
    fclose(arquivo_saida);
}

/**
 * @brief Exibe o texto codificado na tela
 * 
 * @param arquivo Arquivo original
 * @param dicionario Dicionário com os códigos de Huffman
 * 
 * @details Útil para depuração - mostra a sequência de bits em formato ASCII
 */
void mostrarTextoCodificado(FILE* arquivo, char* dicionario[256]) {
    fseek(arquivo, 0, SEEK_SET);
    
    printf("=== TEXTO CODIFICADO (ASCII 0s/1s) ===\n");
    
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

/**
 * @brief Decodifica um arquivo codificado usando a árvore de Huffman
 * 
 * @param arquivo_codificado Nome do arquivo com códigos ASCII ('0' e '1')
 * @param raiz Raiz da árvore de Huffman
 * @param nome_saida Nome do arquivo de saída decodificado
 * 
 * @details Percorre a árvore bit a bit:
 *          - '0' vai para a esquerda
 *          - '1' vai para a direita
 *          - Ao atingir uma folha, escreve o símbolo e volta à raiz
 */
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
    
    while ((bit = fgetc(arquivo_entrada)) != EOF) {
        if (bit == '0') {
            atual = atual->esquerdo;
        } else if (bit == '1') {
            atual = atual->direito;
        } else {
            continue;
        }
        
        if (atual->esquerdo == NULL && atual->direito == NULL) {
            fwrite(&atual->simbolo, 1, 1, arquivo_saida);
            caracteres_decodificados++;
            atual = raiz;
        }
    }
    
    fclose(arquivo_entrada);
    fclose(arquivo_saida);
}

/**
 * @brief Decodifica e exibe o conteúdo na tela
 * 
 * @param arquivo_codificado Nome do arquivo codificado
 * @param raiz Raiz da árvore de Huffman
 * 
 * @details Usa um arquivo temporário para armazenar o resultado
 *          e depois o exibe na saída padrão
 */
void mostrarTextoDecodificado(const char* arquivo_codificado, struct No* raiz) {
    decodificarArquivo(arquivo_codificado, raiz, "temp_decodificado.txt");
    
    FILE* temp = fopen("temp_decodificado.txt", "rb");
    if (temp == NULL) {
        printf("Erro ao abrir arquivo temporário\n");
        return;
    }
    
    printf("=== TEXTO DECODIFICADO ===\n");
    
    unsigned char buffer[1000];
    size_t bytes_lidos;
    
    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), temp)) > 0) {
        fwrite(buffer, 1, bytes_lidos, stdout);
    }
    
    printf("\n");
    fclose(temp);
    
    remove("temp_decodificado.txt");
}

/*
 ============================================================================
 PARTE 6: COMPACTAÇÃO - GERA O ARQUIVO .huff FINAL
 ============================================================================
*/

/**
 * @brief Calcula quantos bits de lixo serão necessários
 * 
 * @param arquivo_codificado Nome do arquivo com códigos ASCII
 * @return Número de bits de lixo (0-7)
 * 
 * @details Como cada byte tem 8 bits, precisamos calcular
 *          quantos bits vazios sobrarão no último byte
 */
int calcularBitsLixo(const char* arquivo_codificado) {
    FILE *arquivo = fopen(arquivo_codificado, "rb");
    if (!arquivo) return 0;
    
    int total_bits = 0;
    unsigned char bit;
    
    while (fread(&bit, 1, 1, arquivo) == 1) {
        if (bit == '0' || bit == '1') {
            total_bits++;
        }
    }
    
    fclose(arquivo);
    
    int lixo = (8 - (total_bits % 8)) % 8;
    return lixo;
}

/**
 * @brief Calcula o tamanho da representação da árvore em bytes
 * 
 * @param raiz Raiz da árvore de Huffman
 * @return Número de bytes necessários para representar a árvore
 * 
 * @details Cada nó folha ocupa 2 bytes (\simbolo), cada nó interno ocupa 1 byte (*)
 */
int calcularTamanhoArvore(struct No* raiz) {
    if (raiz == NULL) return 0;
    
    if (raiz->esquerdo == NULL && raiz->direito == NULL) {
        return 2;
    }
    else {
        return 1 + calcularTamanhoArvore(raiz->esquerdo) + calcularTamanhoArvore(raiz->direito);
    }
}

/**
 * @brief Escreve a árvore de Huffman em pré-ordem no arquivo
 * 
 * @param raiz Raiz da árvore
 * @param arquivo Arquivo de saída binário
 * 
 * @details Formato:
 *          - Nós internos: um byte '*'
 *          - Nós folha: dois bytes '\\' seguido do símbolo
 */
void escreverArvorePreOrdem(struct No* raiz, FILE* arquivo) {
    if (raiz == NULL) return;
    
    if (raiz->esquerdo == NULL && raiz->direito == NULL) {
        unsigned char escape = '\\';
        fwrite(&escape, sizeof(unsigned char), 1, arquivo);
        fwrite(&raiz->simbolo, sizeof(unsigned char), 1, arquivo);
    }
    else {
        unsigned char simbolo_interno = '*';
        fwrite(&simbolo_interno, sizeof(unsigned char), 1, arquivo);
    }
    
    escreverArvorePreOrdem(raiz->esquerdo, arquivo);
    escreverArvorePreOrdem(raiz->direito, arquivo);
}

/**
 * @brief Compacta o arquivo codificado gerando o arquivo .huff final
 * 
 * @param arquivo_codificado Nome do arquivo com códigos ASCII
 * @param raiz Raiz da árvore de Huffman
 * @param arquivo_compactado Nome do arquivo .huff de saída
 * 
 * @details Formato do arquivo .huff:
 *          - Byte 1-2: Cabeçalho (3 bits lixo + 13 bits tamanho árvore)
 *          - Bytes seguintes: Árvore em pré-ordem
 *          - Resto: Dados compactados em formato binário (8 bits por byte)
 * 
 * @note Converte ASCII '0'/'1' para bits reais, empacotando 8 bits por byte
 */
void compactarComCabecalho(const char* arquivo_codificado, struct No* raiz, const char* arquivo_compactado) {
    FILE *entrada = fopen(arquivo_codificado, "rb");
    FILE *saida = fopen(arquivo_compactado, "wb");
    
    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos para compactação\n");
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        return;
    }
    
    int lixo = calcularBitsLixo(arquivo_codificado);
    int tamanho_arvore = calcularTamanhoArvore(raiz);
    
    printf("\n=== CABEÇALHO HUFFMAN ===\n");
    printf("Bits de lixo: %d\n", lixo);
    printf("Tamanho da árvore: %d\n", tamanho_arvore);
    
    // Monta cabeçalho: 3 bits lixo + 13 bits tamanho
    unsigned short cabecalho = 0;
    cabecalho |= (lixo & 0x07) << 13;
    cabecalho |= (tamanho_arvore & 0x1FFF);
    
    unsigned char byte1 = (cabecalho >> 8) & 0xFF;
    unsigned char byte2 = cabecalho & 0xFF;
    fwrite(&byte1, sizeof(unsigned char), 1, saida);
    fwrite(&byte2, sizeof(unsigned char), 1, saida);
    
    escreverArvorePreOrdem(raiz, saida);
    
    // Converte ASCII '0'/'1' para bits binários
    int j = 7;
    unsigned char mascara, byte = 0;
    unsigned char bit;
    int bits_escritos = 0;
    
    while (fread(&bit, 1, 1, entrada) == 1) {
        if (bit == '0' || bit == '1') {
            mascara = 1;
            
            if (bit == '1') {
                mascara = mascara << j;
                byte = byte | mascara;
            } 
            
            j--;
            bits_escritos++;
            
            if (j < 0) {
                fwrite(&byte, sizeof(unsigned char), 1, saida);
                byte = 0;
                j = 7;
            }
        }
    }
    
    if (j != 7) {
        fwrite(&byte, sizeof(unsigned char), 1, saida);
    }
    
    fclose(entrada);
    fclose(saida);
    
    printf("Arquivo compactado salvo como: %s\n", arquivo_compactado);
    printf("Total de bits codificados: %d\n", bits_escritos);
}

/*
 ============================================================================
 PARTE 7: DESCOMPACTAÇÃO
 ============================================================================
*/

/**
 * @brief Reconstrói a árvore de Huffman a partir de sua representação em pré-ordem
 * 
 * @param arquivo Arquivo binário aberto contendo a árvore serializada
 * @return Ponteiro para a raiz da árvore reconstruída
 * 
 * @details Lê byte a byte do arquivo:
 *          - Se encontrar '*': cria nó interno e reconstrói subárvores recursivamente
 *          - Se encontrar '\\': lê o próximo byte como símbolo de uma folha
 *          - Caso contrário: trata como folha direta
 */
struct No* reconstruirArvorePreOrdem(FILE* arquivo) {
    unsigned char simbolo_lido;
    
    if (fread(&simbolo_lido, sizeof(unsigned char), 1, arquivo) != 1) {
        return NULL;
    }

    if (simbolo_lido == '*') {
        struct No* novoNo = criarNo('*', 0);
        novoNo->esquerdo = reconstruirArvorePreOrdem(arquivo);
        novoNo->direito = reconstruirArvorePreOrdem(arquivo);
        return novoNo;
    }
    else if (simbolo_lido == '\\') {
        if (fread(&simbolo_lido, sizeof(unsigned char), 1, arquivo) != 1) {
            return NULL;
        }
        return criarNo(simbolo_lido, 0);
    }
    
    return criarNo(simbolo_lido, 0);
}

/**
 * @brief Lê e decodifica o cabeçalho do arquivo compactado
 * 
 * @param arquivo Arquivo .huff aberto
 * @param lixo Ponteiro para armazenar o número de bits de lixo
 * @param tamanho_arvore Ponteiro para armazenar o tamanho da árvore
 * 
 * @details O cabeçalho tem 2 bytes (16 bits):
 *          - 3 bits mais significativos: bits de lixo (0-7)
 *          - 13 bits seguintes: tamanho da árvore em bytes (0-8191)
 */
void lerCabecalhoCompactado(FILE* arquivo, int* lixo, int* tamanho_arvore) {
    unsigned char byte1, byte2;
    
    size_t lido1 = fread(&byte1, 1, 1, arquivo);
    size_t lido2 = fread(&byte2, 1, 1, arquivo);
    
    if (lido1 != 1 || lido2 != 1) {
        printf("Erro: Não foi possível ler o cabeçalho do arquivo compactado\n");
        *lixo = 0;
        *tamanho_arvore = 0;
        return;
    }
    
    unsigned short cabecalho = (byte1 << 8) | byte2;
    
    *lixo = (cabecalho >> 13) & 0x07;
    *tamanho_arvore = cabecalho & 0x1FFF;
}

/**
 * @brief Descompacta um arquivo .huff gerando o arquivo original
 * 
 * @param arquivo_compactado Nome do arquivo .huff de entrada
 * @param arquivo_saida Nome do arquivo descompactado de saída
 * 
 * @details Processo de descompactação:
 *          1. Lê o cabeçalho (2 bytes)
 *          2. Reconstrói a árvore de Huffman
 *          3. Lê os dados compactados bit a bit
 *          4. Percorre a árvore para decodificar cada símbolo
 *          5. Ignora os bits de lixo no final
 * 
 * @note Os bits de lixo ficam no último byte e devem ser descartados
 */
void descompactarArquivoGeral(const char* arquivo_compactado, const char* arquivo_saida) {
    FILE *entrada = fopen(arquivo_compactado, "rb");
    if (!entrada) {
        printf("Erro ao abrir arquivo compactado: %s\n", arquivo_compactado);
        return;
    }
    FILE *saida = fopen(arquivo_saida, "wb");
    if (!saida) {
        printf("Erro ao criar arquivo de saída: %s\n", arquivo_saida);
        fclose(entrada);
        return;
    }

    int lixo, tamanho_arvore;
    lerCabecalhoCompactado(entrada, &lixo, &tamanho_arvore);
    
    printf("\n=== LENDO CABEÇALHO PARA DESCOMPACTAÇÃO ===\n");
    printf("Bits de lixo: %d\n", lixo);
    printf("Tamanho da árvore: %d\n", tamanho_arvore);
    
    if (tamanho_arvore == 0) {
        printf("Erro: Cabeçalho inválido ou arquivo corrompido\n");
        fclose(entrada);
        fclose(saida);
        return;
    }

    struct No* raiz_lida = reconstruirArvorePreOrdem(entrada);
    
    long inicio_dados = 2 + tamanho_arvore;
    
    fseek(entrada, 0, SEEK_END);
    long tamanho_total = ftell(entrada);
    fseek(entrada, inicio_dados, SEEK_SET);
    
    long bytes_dados_compactados = tamanho_total - inicio_dados;
    long total_bits_validos = (bytes_dados_compactados * 8) - lixo;
    
    printf("Bytes de dados compactados: %ld\n", bytes_dados_compactados);
    printf("Total de bits válidos (sem lixo): %ld\n", total_bits_validos);
    
    struct No* atual = raiz_lida;
    unsigned char byte_lido;
    long bits_processados = 0;
    int simbolos_decodificados = 0;
    
    while (fread(&byte_lido, 1, 1, entrada) == 1) {
        for (int i = 7; i >= 0; i--) {
            if (bits_processados >= total_bits_validos) {
                break;
            }

            int bit = (byte_lido >> i) & 1;

            if (bit == 0) {
                atual = atual->esquerdo;
            } else {
                atual = atual->direito;
            }
            
            if (atual != NULL && atual->esquerdo == NULL && atual->direito == NULL) {
                fwrite(&atual->simbolo, 1, 1, saida);
                atual = raiz_lida;
                simbolos_decodificados++;
            }
            
            bits_processados++;
        }
        
        if (bits_processados >= total_bits_validos) {
            break;
        }
    }
    
    printf("Decodificação concluída. Bits processados: %ld\n", bits_processados);
    printf("Símbolos decodificados: %d\n", simbolos_decodificados);
    
    liberarArvore(raiz_lida);
    fclose(entrada);
    fclose(saida);
}

/*
 ============================================================================
 PARTE 8: INTERFACE DE USUÁRIO E FUNÇÕES PRINCIPAIS
 ============================================================================
*/

/**
 * @brief Exibe o menu principal do programa
 */
void mostrarMenu() {
    printf("\n=== COMPACTADOR HUFFMAN ===\n");
    printf("1. Compactar arquivo\n");
    printf("2. Descompactar arquivo .huff\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
}

/**
 * @brief Fluxo completo de compactação de um arquivo
 * 
 * @details Processo:
 *          1. Solicita nome do arquivo ao usuário
 *          2. Conta frequências de cada byte
 *          3. Constrói lista ordenada de frequências
 *          4. Gera árvore de Huffman
 *          5. Cria dicionário de códigos
 *          6. Codifica o arquivo em formato temporário ASCII
 *          7. Compacta para formato binário .huff
 *          8. Remove arquivo temporário
 * 
 * @note O arquivo .huff terá o mesmo nome do original com extensão .huff
 */
void compactarArquivo() {
    char nome_arquivo[256];
    printf("Digite o nome do arquivo para compactar: ");
    scanf("%255s", nome_arquivo);
    
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro: Não foi possível abrir o arquivo '%s'\n", nome_arquivo);
        return;
    }
    
    printf("Compactando arquivo: %s\n", nome_arquivo);
    
    int frequencias[256];
    contarFrequenciasArquivo(arquivo, frequencias);
    
    struct ListaNo* lista = construirListaFrequencia(frequencias);
    if (lista == NULL) {
        printf("Erro: Arquivo vazio ou não contém dados válidos\n");
        fclose(arquivo);
        return;
    }
    
    struct No* raiz = construirArvoreHuffman(lista);
    if (raiz == NULL) {
        printf("ERRO: Árvore não foi construída!\n");
        liberarLista(lista);
        fclose(arquivo);
        return;
    }
    
    int altura = calcularAlturaArvore(raiz);
    int colunas = altura + 2;
    
    char* dicionario[256] = {NULL};
    alocarDicionario(dicionario, colunas);
    gerarDicionario(dicionario, raiz, colunas);
    
    const char* nome_codificado_temp = "temp_codificado.temp";
    codificarArquivo(arquivo, dicionario, nome_codificado_temp);
    
    char nome_saida[512];
    snprintf(nome_saida, sizeof(nome_saida), "%s.huff", nome_arquivo);
    compactarComCabecalho(nome_codificado_temp, raiz, nome_saida);
    
    liberarDicionario(dicionario);
    liberarArvore(raiz);
    liberarLista(lista);
    fclose(arquivo);
    
    remove(nome_codificado_temp);
    
    printf("Compactação concluída! Arquivo salvo como: %s\n", nome_saida);
}

/**
 * @brief Fluxo completo de descompactação de um arquivo .huff
 * 
 * @details Processo:
 *          1. Solicita nome do arquivo .huff
 *          2. Solicita nome do arquivo de saída
 *          3. Chama função de descompactação
 */
void descompactarArquivo() {
    char nome_arquivo[256];
    printf("Digite o nome do arquivo .huff para descompactar: ");
    scanf("%255s", nome_arquivo);
    
    char nome_saida[256];
    printf("Digite o nome para o arquivo descompactado: ");
    scanf("%255s", nome_saida);
    
    descompactarArquivoGeral(nome_arquivo, nome_saida);
    printf("Descompactação concluída! Arquivo salvo como: %s\n", nome_saida);
}

/**
 * @brief Função principal do programa
 * 
 * @return 0 em caso de sucesso
 * 
 * @details Exibe menu interativo permitindo ao usuário:
 *          - Compactar arquivos (gera .huff)
 *          - Descompactar arquivos .huff
 *          - Sair do programa
 */
int main() {
    setlocale(LC_ALL, "Portuguese");
    
    int opcao;
    
    do {
        mostrarMenu();
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1:
                compactarArquivo();
                break;
            case 2:
                descompactarArquivo();
                break;
            case 3:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 3);
    
    return 0;
}
