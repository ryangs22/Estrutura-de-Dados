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

/*
 ============================================================================
 PARTE 6: COMPACTAÇÃO DO ARQUIVO COM CABEÇALHO HUFFMAN
 ============================================================================
*/

// Função para calcular quantos bits de lixo teremos no final
int calcularBitsLixo(const char* arquivo_codificado) {
    FILE *arquivo = fopen(arquivo_codificado, "r");
    if (!arquivo) return 0;
    
    int total_bits = 0;
    char bit;
    
    // Contar total de bits no arquivo codificado
    while ((bit = fgetc(arquivo)) != EOF) {
        if (bit == '0' || bit == '1') {
            total_bits++;
        }
    }
    
    fclose(arquivo);
    
    // Calcular bits de lixo: 8 - (total_bits % 8)
    int lixo = (8 - (total_bits % 8)) % 8;
    return lixo;
}

// Função para calcular o tamanho da árvore em pré-ordem
int calcularTamanhoArvore(struct No* raiz) {
    if (raiz == NULL) return 0;
    
    // Cada nó ocupa 1 byte na representação
    return 1 + calcularTamanhoArvore(raiz->esquerdo) + calcularTamanhoArvore(raiz->direito);
}

// Função para escrever a árvore em pré-ordem no arquivo
void escreverArvorePreOrdem(struct No* raiz, FILE* arquivo) {
    if (raiz == NULL) return;
    
    // Escrever o símbolo do nó
    fwrite(&raiz->simbolo, sizeof(unsigned char), 1, arquivo);
    
    // Recursão para subárvores
    escreverArvorePreOrdem(raiz->esquerdo, arquivo);
    escreverArvorePreOrdem(raiz->direito, arquivo);
}

// Função principal de compactação com cabeçalho Huffman
void compactarComCabecalho(const char* arquivo_codificado, struct No* raiz, const char* arquivo_compactado) {
    FILE *entrada = fopen(arquivo_codificado, "r");
    FILE *saida = fopen(arquivo_compactado, "wb");
    
    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos para compactação\n");
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        return;
    }
    
    // CALCULAR VALORES DO CABEÇALHO
    int lixo = calcularBitsLixo(arquivo_codificado);
    int tamanho_arvore = calcularTamanhoArvore(raiz);
    
    printf("=== CABEÇALHO HUFFMAN ===\n");
    printf("Bits de lixo: %d\n", lixo);
    printf("Tamanho da árvore: %d\n", tamanho_arvore);
    
    // CONSTRUIR CABEÇALHO (16 bits = 3 bits lixo + 13 bits tamanho árvore)
    unsigned short cabecalho = 0;
    
    // Colocar bits de lixo nos 3 primeiros bits
    cabecalho |= (lixo & 0x07) << 13;
    
    // Colocar tamanho da árvore nos 13 bits seguintes
    cabecalho |= (tamanho_arvore & 0x1FFF);
    
    // Escrever cabeçalho (2 bytes)
    unsigned char byte1 = (cabecalho >> 8) & 0xFF;
    unsigned char byte2 = cabecalho & 0xFF;
    fwrite(&byte1, sizeof(unsigned char), 1, saida);
    fwrite(&byte2, sizeof(unsigned char), 1, saida);
    
    // ESCREVER ÁRVORE EM PRÉ-ORDEM
    escreverArvorePreOrdem(raiz, saida);
    
    // COMPACTAR DADOS CODIFICADOS
    int j = 7;
    unsigned char mascara, byte = 0;
    char bit;
    int bits_escritos = 0;
    
    while ((bit = fgetc(entrada)) != EOF) {
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
    
    // Escrever último byte se necessário
    if (j != 7) {
        fwrite(&byte, sizeof(unsigned char), 1, saida);
    }
    
    fclose(entrada);
    fclose(saida);
    
    printf("Arquivo compactado salvo como: %s\n", arquivo_compactado);
    printf("Total de bits codificados: %d\n", bits_escritos);
}

// Função para mostrar o cabeçalho do arquivo compactado
void mostrarCabecalhoCompactado(const char* arquivo_compactado) {
    FILE *arquivo = fopen(arquivo_compactado, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo compactado\n");
        return;
    }
    
    printf("\n=== ESTRUTURA DO ARQUIVO COMPACTADO ===\n");
    
    // LER CABEÇALHO
    unsigned char byte1, byte2;
    fread(&byte1, sizeof(unsigned char), 1, arquivo);
    fread(&byte2, sizeof(unsigned char), 1, arquivo);
    
    unsigned short cabecalho = (byte1 << 8) | byte2;
    int lixo = (cabecalho >> 13) & 0x07;
    int tamanho_arvore = cabecalho & 0x1FFF;
    
    printf("Cabeçalho (16 bits): ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (cabecalho >> i) & 1);
        if (i == 13) printf(" ");
    }
    printf("\n");
    
    printf("Bits de lixo: %d (", lixo);
    for (int i = 2; i >= 0; i--) {
        printf("%d", (lixo >> i) & 1);
    }
    printf(")\n");
    
    printf("Tamanho da árvore: %d (", tamanho_arvore);
    for (int i = 12; i >= 0; i--) {
        printf("%d", (tamanho_arvore >> i) & 1);
    }
    printf(")\n");
    
    // LER E MOSTRAR ÁRVORE
    printf("Árvore em pré-ordem (%d bytes): ", tamanho_arvore);
    for (int i = 0; i < tamanho_arvore; i++) {
        unsigned char simbolo;
        fread(&simbolo, sizeof(unsigned char), 1, arquivo);
        
        if (simbolo == '*') {
            printf("*");
        } else if (simbolo >= 32 && simbolo <= 126) {
            printf("%c", simbolo);
        } else {
            printf("[0x%02X]", simbolo);
        }
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

    // PARTE 6: COMPACTAÇÃO COM CABEÇALHO
    printf("\n");
    compactarComCabecalho("texto_codificado.txt", raiz, "compactado.huff");
    mostrarCabecalhoCompactado("compactado.huff");
    
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
