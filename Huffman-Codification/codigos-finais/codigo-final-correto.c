#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

/*
 ============================================================================
 PROJETO: COMPACTADOR DE ARQUIVOS - ALGORITMO DE HUFFMAN
 ============================================================================
 Este programa implementa o algoritmo de Huffman para compactação e 
 descompactação de arquivos de qualquer formato. O arquivo compactado 
 segue o padrão .huff com cabeçalho específico.
 ============================================================================
*/

/*
 ============================================================================
 ESTRUTURAS DE DADOS GENÉRICAS
 ============================================================================
*/

/**
 * ListaNo - Nó genérico de lista encadeada
 */
struct ListaNo {
    void* item;
    struct ListaNo* proximo;
};

/**
 * No - Representa um nó na árvore de Huffman
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

void liberarLista(struct ListaNo* cabeca) {
    while (cabeca != NULL) {
        struct ListaNo* temp = cabeca;
        cabeca = cabeca->proximo;
        free(temp);
    }
}

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

void contarFrequenciasArquivo(FILE* arquivo, int frequencias[256]) {
    for (int i = 0; i < 256; i++) {
        frequencias[i] = 0;
    }
    
    unsigned char byte;
    while (fread(&byte, 1, 1, arquivo) == 1) {
        frequencias[byte]++;
    }
}

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

void liberarArvore(struct No* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    liberarArvore(raiz->esquerdo);
    liberarArvore(raiz->direito);
    
    free(raiz);
}

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

int calcularAlturaArvore(struct No* raiz) {
    if (raiz == NULL) {
        return -1;
    }
    
    int alturaEsquerda = calcularAlturaArvore(raiz->esquerdo);
    int alturaDireita = calcularAlturaArvore(raiz->direito);
    
    return (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita) + 1;
}

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

void gerarDicionario(char* dicionario[256], struct No* raiz, int colunas) {
    char concatenacao[colunas];
    gerarDicionarioRecursivo(dicionario, raiz, concatenacao, 0, colunas);
}

void liberarDicionario(char* dicionario[256]) {
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] != NULL) {
            free(dicionario[i]);
            dicionario[i] = NULL;
        }
    }
}

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

int calcularTamanhoArvore(struct No* raiz) {
    if (raiz == NULL) return 0;
    
    if (raiz->esquerdo == NULL && raiz->direito == NULL) {
        return 2;
    }
    else {
        return 1 + calcularTamanhoArvore(raiz->esquerdo) + calcularTamanhoArvore(raiz->direito);
    }
}

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
    
    unsigned short cabecalho = 0;
    cabecalho |= (lixo & 0x07) << 13;
    cabecalho |= (tamanho_arvore & 0x1FFF);
    
    unsigned char byte1 = (cabecalho >> 8) & 0xFF;
    unsigned char byte2 = cabecalho & 0xFF;
    fwrite(&byte1, sizeof(unsigned char), 1, saida);
    fwrite(&byte2, sizeof(unsigned char), 1, saida);
    
    escreverArvorePreOrdem(raiz, saida);
    
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
 PARTE 8: FUNÇÃO PRINCIPAL MODIFICADA
 ============================================================================
*/

void mostrarMenu() {
    printf("\n=== COMPACTADOR HUFFMAN ===\n");
    printf("1. Compactar arquivo\n");
    printf("2. Descompactar arquivo .huff\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
}

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
