#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int **clausulas; //Matriz para armazenar cada clausulas EX (x1 ∨ ¬x2)
    int *tamanho; //Número de literais por cláusula
    int n_clausulas; // Quantidade total de cláusulas
    int n_variaveis; // Quantidade total de variaveis
}CNF;

typedef struct arvorebinaria {
    int valor;
    int nivel;
    struct arvorebinaria *direita; // True
    struct arvorebinaria *esquerda; // False
} Arvorebinaria;

Arvorebinaria* criarNo(int variavel, int valor) {
    Arvorebinaria *novo = (Arvorebinaria*)malloc(sizeof(Arvorebinaria));
    novo->nivel = variavel;
    novo->valor = valor;
    novo->direita = NULL;
    novo->esquerda = NULL;
    return novo;
}

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
   
    f->clausulas = (int**) malloc(f->n_clausulas * sizeof(int*));
    f->tamanho = (int*) malloc(f->n_clausulas * sizeof(int));

    for (int i = 0; i < f->n_clausulas; i++) {
        f->clausulas[i] = (int*) malloc(f->n_variaveis * sizeof(int));
    }

    for (int i = 0; i < f->n_clausulas; i++) {
        f->tamanho[i] = 0;
    }

    for (int i = 0; i < f->n_clausulas; i++)
    {
        for (int j = 0; j < f->n_variaveis; j++)
        {
            int x;
            fscanf (fp, "%d" , &x);
            if (x == 0) break;
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

int satisfaz(CNF *f, int variaveis0[]){
    for (int i = 0; i < f->n_clausulas; i++) {
        if (!clausula_satisfeita(f->clausulas[i], f->tamanho[i], variaveis0)) {
            return 0;
        }
    }
    return 1;
}

int gerarArvore(CNF *f,Arvorebinaria *a,int nivel,int variaveis0[]){
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

    a->esquerda = criarNo(nivel,0);
    variaveis0[nivel]= -1;
    if (gerarArvore(f,a->esquerda,nivel+1,variaveis0))
    {
        return 1;
    }

    a->direita = criarNo(nivel,1);
    variaveis0[nivel]= 1;
    if (gerarArvore(f,a->direita,nivel+1,variaveis0))
    {
        return 1;
    }

    variaveis0[nivel] = 0;
    return 0;
}

int main() {
    char nome_do_arquivo[256];
    printf ("digite o nome do arquivo: ");
    scanf("%255s",nome_do_arquivo);
    printf("\n");

    CNF *cnf = LerCNF(nome_do_arquivo);
    if (!cnf) return 1;

    printar_clausulas(cnf,nome_do_arquivo);

    Arvorebinaria *a = criarNo(0,0);
    int x = cnf->n_variaveis;
    int variaveis0[x + 1]; // +1 para usar índice começando em 1

    for (int i = 0; i <= x; i++) {
        variaveis0[i] = 0;
    }

    if (gerarArvore(cnf,a,1,variaveis0) == 0)
    {
        printf("unsat\n");
    }

    return 0;
}
