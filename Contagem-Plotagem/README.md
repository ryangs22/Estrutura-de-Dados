# 📊 Análise de Desempenho: Lista Encadeada vs Árvore Binária de Busca

Este projeto realiza uma comparação de desempenho entre duas estruturas de dados: Lista Encadeada e Árvore Binária de Busca (ABB), focando na eficiência de operações de busca. O projeto visa ilustrar de forma prática as diferenças de eficiência entre uma estrutura linear (Lista Encadeada) e uma estrutura hierárquica (ABB), destacando como a organização dos dados impacta no tempo de busca.

## 🔍 Metodologia
- *Geração de dados*: São randomizados 1000 números no intervalo de 0 a 5000.
- *Análise de comparações*: Mede-se a quantidade de comparações necessárias para encontrar um número específico em cada estrutura.
- *Visualização*: Os resultados são compilados e plotados em gráficos para facilitar a comparação visual do desempenho.

## ⚙️ Funcionamento
Execute o código "countplot.c" e ele gerará um arquivo com os dados comparativos ("dados_comparacoes.csv"). No *RStudio*, selecione o diretório onde está salvo o código e o novo arquivo csv. Em seguida, aperte em *New File > R Script* e cole o script de geração de gráfico: "script.r". Para rodar o código use o comando "Ctrl + A" para selecionar tudo e em seguida "Ctrl + Enter". O gráfico será exibido na aba de Plots e salvo externamente como um arquivo png.
