# Ler os dados do arquivo CSV
dados <- read.csv("dados_comparacoes.csv")

# Visualizar as primeiras linhas
head(dados)

# Configurações gerais para os gráficos
par(mfrow = c(2, 2))

# Gráfico 1: Comparações por busca (linhas)
plot(dados$comparacoes_lista, type = "l", col = "red", 
     main = "Comparações por Busca",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista, dados$comparacoes_arvore))))
lines(dados$comparacoes_arvore, col = "blue")
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, bg = "white")

# Gráfico 2: Gráfico de linha estilo da imagem (MODIFICADO)
# Selecionar 50 primeiras buscas
n_buscas <- min(50, nrow(dados))
buscas <- 1:n_buscas

plot(buscas, dados$comparacoes_lista[1:n_buscas], type = "o", col = "red",
     main = "Detalhe: Comparações nas Primeiras 50 Buscas",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista[1:n_buscas], 
                       dados$comparacoes_arvore[1:n_buscas]))),
     pch = 16, lwd = 2)

lines(buscas, dados$comparacoes_arvore[1:n_buscas], type = "o", 
      col = "blue", pch = 17, lwd = 2)

# Adicionar valores nos pontos PRIMEIRO
text(buscas, dados$comparacoes_lista[1:n_buscas], 
     dados$comparacoes_lista[1:n_buscas], pos = 3, cex = 0.6, col = "red")
text(buscas, dados$comparacoes_arvore[1:n_buscas], 
     dados$comparacoes_arvore[1:n_buscas], pos = 3, cex = 0.6, col = "blue")

# Adicionar legenda POR ÚLTIMO para ficar sobreposta
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, pch = c(16, 17),
       bg = "white", box.lwd = 1)

# Gráfico 3: Histograma das comparações na lista
hist(dados$comparacoes_lista, breaks = 20, 
     main = "Histograma - Comparações na Lista",
     xlab = "Comparações", ylab = "Frequência",
     col = "lightcoral")

# Gráfico 4: Histograma das comparações na árvore
hist(dados$comparacoes_arvore, breaks = 20,
     main = "Histograma - Comparações na Árvore", 
     xlab = "Comparações", ylab = "Frequência",
     col = "lightblue")

# Salvar os gráficos em arquivo PNG
png("comparacoes_estruturas.png", width = 1200, height = 800)
par(mfrow = c(2, 2))

# Repetir os gráficos para o arquivo

# Gráfico 1 no arquivo
plot(dados$comparacoes_lista, type = "l", col = "red", 
     main = "Comparações por Busca",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista, dados$comparacoes_arvore))))
lines(dados$comparacoes_arvore, col = "blue")
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, bg = "white")

# Gráfico 2 no arquivo - Gráfico de linha detalhado
plot(buscas, dados$comparacoes_lista[1:n_buscas], type = "o", col = "red",
     main = "Detalhe: Comparações nas Primeiras 50 Buscas",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista[1:n_buscas], 
                       dados$comparacoes_arvore[1:n_buscas]))),
     pch = 16, lwd = 2)

lines(buscas, dados$comparacoes_arvore[1:n_buscas], type = "o", 
      col = "blue", pch = 17, lwd = 2)

# Adicionar valores nos pontos PRIMEIRO
text(buscas, dados$comparacoes_lista[1:n_buscas], 
     dados$comparacoes_lista[1:n_buscas], pos = 3, cex = 0.6, col = "red")
text(buscas, dados$comparacoes_arvore[1:n_buscas], 
     dados$comparacoes_arvore[1:n_buscas], pos = 3, cex = 0.6, col = "blue")

# Adicionar legenda POR ÚLTIMO para ficar sobreposta
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, pch = c(1, 17),
       bg = "white", box.lwd = 1)

# Gráfico 3 no arquivo
hist(dados$comparacoes_lista, breaks = 20, 
     main = "Histograma - Comparações na Lista",
     xlab = "Comparações", ylab = "Frequência",
     col = "lightcoral")

# Gráfico 4 no arquivo
hist(dados$comparacoes_arvore, breaks = 20,
     main = "Histograma - Comparações na Árvore", 
     xlab = "Comparações", ylab = "Frequência",
     col = "lightblue")

dev.off()

# Estatísticas descritivas
cat("=== ESTATÍSTICAS DESCRITIVAS ===\n")
cat("Lista - Média:", mean(dados$comparacoes_lista), 
    "| Mediana:", median(dados$comparacoes_lista),
    "| Desvio Padrão:", sd(dados$comparacoes_lista), "\n")
cat("Árvore - Média:", mean(dados$comparacoes_arvore),
    "| Mediana:", median(dados$comparacoes_arvore),
    "| Desvio Padrão:", sd(dados$comparacoes_arvore), "\n")

cat("\nGráficos salvos em 'comparacoes_estruturas.png'\n")
