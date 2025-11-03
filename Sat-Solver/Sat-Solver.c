/**
 * @file sat_solver.c
 * @brief Implementação de um SAT Solver usando busca em árvore binária
 * @details Este programa resolve o problema de satisfatibilidade booleana (SAT)
 *          lendo fórmulas em formato CNF (Conjunctive Normal Form) e realizando
 *          busca exaustiva através de uma árvore de decisão.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct CNF
 * @brief Estrutura para representar uma fórmula CNF
 * 
 * @var CNF::clausulas
 * Matriz para armazenar cada cláusula. Ex: (x1 ∨ ¬x2)
 * 
 * @var CNF::tamanho
 * Array contendo o número de literais por cláusula
 * 
 * @var CNF::n_clausulas
 * Quantidade total de cláusulas na fórmula
 * 
 * @var CNF::n_variaveis
 * Quantidade total de variáveis na fórmula
 */
typedef struct 
{
    int **clausulas;
    int *tamanho;
    int n_clausulas;
    int n_variaveis;
} CNF;

/**
 * @struct arvorebinaria
 * @brief Estrutura para representar um nó da árvore de decisão
 * 
 * @var arvorebinaria::valor
 * Valor da variável neste nó (0 ou 1)
 * 
 * @var arvorebinaria::nivel
 * Nível da árvore (corresponde ao índice da variável)
 * 
 * @var arvorebinaria::direita
 * Ponteiro para subárvore direita (representa atribuição True)
 * 
 * @var arvorebinaria::esquerda
 * Ponteiro para subárvore esquerda (representa atribuição False)
 */
typedef struct arvorebinaria {
    int valor;
    int nivel;
    struct arvorebinaria *direita;
    struct arvorebinaria *esquerda;
} Arvorebinaria;

/**
 * @brief Cria um novo nó da árvore binária
 * 
 * @param variavel Índice da variável representada por este nó
 * @param valor Valor atribuído à variável (0 para False, 1 para True)
 * @return Ponteiro para o novo nó criado
 * 
 * @note A função aloca memória dinamicamente que deve ser liberada posteriormente
 */
Arvorebinaria* criarNo(int variavel, int valor) {
    Arvorebinaria *novo = (Arvorebinaria*)malloc(sizeof(Arvorebinaria));
    novo->nivel = variavel;
    novo->valor = valor;
    novo->direita = NULL;
    novo->esquerda = NULL;
    return novo;
}

/**
 * @brief Lê uma fórmula CNF de um arquivo no formato DIMACS
 * 
 * @param nome_arquivo Nome do arquivo a ser lido
 * @return Ponteiro para estrutura CNF preenchida, ou NULL em caso de erro
 * 
 * @details O arquivo deve seguir o formato DIMACS:
 *          - Linhas começando com 'c' são comentários
 *          - Linha 'p cnf [n_vars] [n_clauses]' define o problema
 *          - Cada linha subsequente representa uma cláusula (terminada com 0)
 *          - Números positivos representam literais, negativos representam negações
 * 
 * @note A memória alocada deve ser liberada manualmente
 */
CNF* LerCNF(const char *nome_arquivo){
    FILE *fp = fopen(nome_arquivo, "r");
    
    if (!fp)
    {
        printf ("Ocorreu um erro ao abrir o arquivo %s\n",nome_arquivo);
        return NULL;
    }
    
    CNF *f = (CNF*) malloc(sizeof(CNF));
    if (!f)
    {
        printf("Erro ao alocar memória para CNF \n");
        fclose(fp);
        return NULL;
    }
    
    f->n_variaveis = 0;
    f->n_clausulas = 0;
    f->clausulas = NULL;
    f->tamanho = NULL;

    // Lê cabeçalho do arquivo
    char linha[256];
    while (fgets(linha,sizeof(linha),fp))
    {
        if (linha[0] == 'c' || linha [0] == '\n')
        {
            continue;
        }
        if (linha [0] == 'p')
        {
            sscanf (linha,"p cnf %d %d", &f->n_variaveis, &f->n_clausulas);
            break;
        }
    }
   
    // Aloca memória para as cláusulas
    f->clausulas = (int**) malloc(f->n_clausulas * sizeof(int*));
    f->tamanho = (int*) malloc(f->n_clausulas * sizeof(int));

    for (int i = 0; i < f->n_clausulas; i++) {
        f->clausulas[i] = (int*) malloc(f->n_variaveis * sizeof(int));
    }

    for (int i = 0; i < f->n_clausulas; i++) {
        f->tamanho[i] = 0;
    }

    // Lê as cláusulas do arquivo
    for (int i = 0; i < f->n_clausulas; i++)
    {
        for (int j = 0; j < f->n_variaveis; j++)
        {
            int x;
            fscanf (fp, "%d" , &x);
            if (x == 0) break; // 0 marca o fim da cláusula
            else
            {
                f->clausulas[i][f->tamanho[i]] = x;
                f->tamanho[i]++;
            }
        }
    }
    fclose(fp);
    return f;
}

/**
 * @brief Imprime as cláusulas de uma fórmula CNF de forma legível
 * 
 * @param f Ponteiro para a estrutura CNF
 * @param nome_arquivo Nome do arquivo original (para exibição)
 * 
 * @details Imprime a fórmula no formato:
 *          (X1 V ¬X2) ∧ (X2 V X3) ∧ ...
 */
void printar_clausulas(CNF *f,const char *nome_arquivo){
    printf("Clausulas do arquivo '%s'\n",nome_arquivo);
    for (int i = 0; i < f->n_clausulas; i++)
    {
        printf("(");
        for (int j = 0; j < f->tamanho[i]; j++)
        {
            if (f->clausulas[i][j] < 0)
            {
                printf(" ¬X%d ",(f->clausulas[i][j]*-1));
            }
            else if (f->clausulas[i][j] > 0)
            {
                printf (" X%d ",f->clausulas[i][j]);
            }
            if (j < f->tamanho[i] - 1) {
                printf(" V ");
            }
        }
        printf (")");
        if (i < f->n_clausulas - 1) {
            printf(" ∧ ");
        }
    }
    printf("\n");
}

/**
 * @brief Verifica se uma cláusula específica é satisfeita
 * 
 * @param clausula Array contendo os literais da cláusula
 * @param tamanho Número de literais na cláusula
 * @param variaveis0 Array com as atribuições atuais das variáveis
 * @return 1 se a cláusula é satisfeita, 0 caso contrário
 * 
 * @details Uma cláusula é satisfeita se pelo menos um de seus literais é verdadeiro:
 *          - Literal positivo é verdadeiro se a variável = 1
 *          - Literal negativo é verdadeiro se a variável = -1
 */
int clausula_satisfeita(int *clausula, int tamanho, int variaveis0[]) {
    for (int i = 0; i < tamanho; i++) {
        int literal = clausula[i];
        int var_index = abs(literal);
        int valor = variaveis0[var_index];

        if ((literal > 0 && valor == 1) || (literal < 0 && valor == -1)) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Verifica se toda a fórmula CNF é satisfeita
 * 
 * @param f Ponteiro para a estrutura CNF
 * @param variaveis0 Array com as atribuições atuais das variáveis
 * @return 1 se todas as cláusulas são satisfeitas, 0 caso contrário
 * 
 * @details Uma fórmula CNF é satisfeita se TODAS as suas cláusulas são satisfeitas
 */
int satisfaz(CNF *f, int variaveis0[]){
    for (int i = 0; i < f->n_clausulas; i++) {
        if (!clausula_satisfeita(f->clausulas[i], f->tamanho[i], variaveis0)) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Gera a árvore de decisão e busca por solução SAT
 * 
 * @param f Ponteiro para a estrutura CNF
 * @param a Ponteiro para o nó atual da árvore
 * @param nivel Nível atual da árvore (corresponde ao índice da variável)
 * @param variaveis0 Array com as atribuições das variáveis
 * @return 1 se uma solução foi encontrada, 0 caso contrário
 * 
 * @details Realiza busca em profundidade (DFS) testando todas as combinações possíveis:
 *          1. Se todas as variáveis foram atribuídas, verifica satisfatibilidade
 *          2. Caso contrário, explora subárvore esquerda (False) e depois direita (True)
 *          3. Retorna ao encontrar a primeira solução (backtracking implícito)
 * 
 * @note Atribuição de valores:
 *       - -1 representa False
 *       - 1 representa True
 *       - 0 representa variável não atribuída
 */
int gerarArvore(CNF *f,Arvorebinaria *a,int nivel,int variaveis0[]){
    // Caso base: todas as variáveis foram atribuídas
    if (nivel > f->n_variaveis)
    {
        if (satisfaz(f,variaveis0) == 1)
        {
            printf ("\nSAT\n");
            for (int i = 1; i <= f->n_variaveis; i++)
            {
                printf("x%d = %s\n", i, variaveis0[i] == 1 ? "true" : "FALSE");
            }
            return 1;
        }
        return 0;
    }

    // Explora subárvore esquerda (False)
    a->esquerda = criarNo(nivel,0);
    variaveis0[nivel]= -1;
    if (gerarArvore(f,a->esquerda,nivel+1,variaveis0))
    {
        return 1;
    }

    // Explora subárvore direita (True)
    a->direita = criarNo(nivel,1);
    variaveis0[nivel]= 1;
    if (gerarArvore(f,a->direita,nivel+1,variaveis0))
    {
        return 1;
    }

    // Backtracking: desfaz atribuição
    variaveis0[nivel] = 0;
    return 0;
}

/**
 * @brief Função principal do programa
 * 
 * @return 0 em caso de sucesso, 1 em caso de erro
 * 
 * @details Fluxo de execução:
 *          1. Solicita nome do arquivo de entrada
 *          2. Lê a fórmula CNF do arquivo
 *          3. Imprime as cláusulas
 *          4. Inicializa a árvore de decisão
 *          5. Executa o algoritmo de busca
 *          6. Imprime "SAT" com a solução ou "unsat" se não houver solução
 */
int main() {
    char nome_do_arquivo[256];
    printf ("digite o nome do arquivo: ");
    scanf("%255s",nome_do_arquivo);
    printf("\n");

    CNF *cnf = LerCNF(nome_do_arquivo);
    if (!cnf) return 1;

    printar_clausulas(cnf,nome_do_arquivo);

    // Inicializa árvore e array de variáveis
    Arvorebinaria *a = criarNo(0,0);
    int x = cnf->n_variaveis;
    int variaveis0[x + 1]; // +1 para usar índice começando em 1

    for (int i = 0; i <= x; i++) {
        variaveis0[i] = 0;
    }

    // Executa busca por solução
    if (gerarArvore(cnf,a,1,variaveis0) == 0)
    {
        printf("unsat\n");
    }

    return 0;
}
