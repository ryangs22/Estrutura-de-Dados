#include <stdio.h>
#include <stdlib.h>

// estrutura que representa cada nó da árvore da union find
typedef struct 
{
    int pai;      // indice do pai/representante na árvore
    int rank;     // altura aproximada da árvore (para otimização da union_set)
} no;

// inicializa a estrutura de union find
void make_set(no *conjunto, int n) 
{
    for (int i = 1; i <= n; i++) 
    {
        conjunto[i].pai = i;    // cada sagui é seu próprio pai/representante
        conjunto[i].rank = 0;   // rank/altura inicial é zero
    }
}

// encontra o representante/raiz do conjunto com compressão de caminho
int find_set(no *conjunto, int x) 
{
    if (conjunto[x].pai != x) {
        // atualiza o pai diretamente para a raiz
        conjunto[x].pai = find_set(conjunto, conjunto[x].pai);
    }

    return conjunto[x].pai;
}

// une dois conjuntos usando união por rank
// permite que a árvore fique mais "curta"
void union_set(no *conjunto, int x, int y) 
{
    int raizX = find_set(conjunto, x);
    int raizY = find_set(conjunto, y);
    
    // se já estão no mesmo conjunto, não faz nada
    if (raizX == raizY) 
    {
        return;
    }
    
    // União por rank: árvore menor aponta para árvore maior
    if (conjunto[raizX].rank < conjunto[raizY].rank) 
    {
        // a raiz menor (X) aponta para a raiz maior (Y)
        // Y se torna o líder do grupo unificado
        conjunto[raizX].pai = raizY;
    } 
    else if (conjunto[raizX].rank > conjunto[raizY].rank) 
    {
        // A raiz menor (Y) aponta para a raiz maior (X)
        // X se torna o líder do grupo unificado
        conjunto[raizY].pai = raizX;
    } 
    else 
    {
        // ranks iguais:
        // escolhemos arbitrariamente X como novo líder
        // incrementamos o rank de X, pois a altura aumentou em 1
        conjunto[raizY].pai = raizX;
        conjunto[raizX].rank++;
    }
}

// Verifica se dois saguins estão no mesmo grupo
int mesmoGrupo(no *conjunto, int x, int y) {
    return find_set(conjunto, x) == find_set(conjunto, y);
}

int main() {
    // numero de saguis = n
    // numero de encontros = m
    int n, m;
    scanf("%d %d", &n, &m);
    
    // aloca array de nos (índices de 1 a n)
    no *conjunto = (no *)malloc((n + 1) * sizeof(no));
    
    // inicializa cada sagui em seu próprio grupo
    make_set(conjunto, n);
    
    // processa os M encontros
    for (int i = 0; i < m; i++) {
        int sagui1, sagui2;
        scanf("%d %d", &sagui1, &sagui2);
        union_set(conjunto, sagui1, sagui2);
    }
    
    // lê a consulta final
    int x, y;
    scanf("%d %d", &x, &y);
    
    // verifica se X e Y estão no mesmo grupo
    if (mesmoGrupo(conjunto, x, y)) 
    {
        printf("SIM\n");
    } else {
        printf("NAO\n");
    }
    
    free(conjunto);
    
    return 0;
}