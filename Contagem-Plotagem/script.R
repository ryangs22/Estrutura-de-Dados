# Ler os dados do arquivo CSV
dados <- read.csv("dados_comparacoes.csv")

# Visualizar as primeiras linhas
head(dados)

# Configurações gerais para os gráficos
par(mfrow = c(2, 3))  # 2x3 para acomodar 5 gráficos

# Preparar dados para os gráficos
n_buscas <- min(50, nrow(dados))
buscas <- 1:n_buscas

# Obter os números que foram realmente buscados
if ("numero_buscado" %in% colnames(dados)) {
  numeros_buscados <- dados$numero_buscado[1:n_buscas]
} else if ("numero" %in% colnames(dados)) {
  numeros_buscados <- dados$numero[1:n_buscas]
} else if ("valor" %in% colnames(dados)) {
  numeros_buscados <- dados$valor[1:n_buscas]
} else {
  set.seed(123)
  numeros_buscados <- sample(0:5000, n_buscas)
}

# Preparar dados para gráfico de barras
set.seed(123)
indices_aleatorios <- sample(1:nrow(dados), 30)
dados_aleatorios <- dados[indices_aleatorios, ]
barras_dados <- matrix(c(dados_aleatorios$comparacoes_lista, 
                         dados_aleatorios$comparacoes_arvore), 
                       ncol = 2, byrow = FALSE)

# Gráfico 1: Comparações por busca (linhas)
plot(dados$comparacoes_lista, type = "l", col = "red", 
     main = "Comparações por Busca",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista, dados$comparacoes_arvore))),
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)
lines(dados$comparacoes_arvore, col = "blue")
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, bg = "white", cex = 0.9)

# Gráfico 2: Detalhe das primeiras 50 buscas com números reais
plot(buscas, dados$comparacoes_lista[1:n_buscas], type = "o", col = "red",
     main = "Detalhe: Primeiras 50 Buscas",
     xlab = "Número Buscado", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista[1:n_buscas], 
                       dados$comparacoes_arvore[1:n_buscas]))),
     xaxt = "n", pch = 16, lwd = 2, 
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

# Adicionar eixo X com todos os números buscados
axis(1, at = buscas, labels = numeros_buscados, cex.axis = 0.5, las = 2)

lines(buscas, dados$comparacoes_arvore[1:n_buscas], type = "o", 
      col = "blue", pch = 17, lwd = 2)

# Adicionar valores nos pontos com cor preta
text(buscas, dados$comparacoes_lista[1:n_buscas], 
     dados$comparacoes_lista[1:n_buscas], pos = 3, cex = 0.5, col = "black")
text(buscas, dados$comparacoes_arvore[1:n_buscas], 
     dados$comparacoes_arvore[1:n_buscas], pos = 3, cex = 0.5, col = "black")

legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, pch = c(16, 17),
       bg = "white", box.lwd = 1, cex = 0.9)

# Gráfico 3: Histograma das comparações na lista
hist(dados$comparacoes_lista, breaks = 20, 
     main = "Histograma - Lista",
     xlab = "Comparações", ylab = "Frequência",
     col = "lightcoral",
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

# Gráfico 4: Histograma das comparações na árvore
hist(dados$comparacoes_arvore, breaks = 20,
     main = "Histograma - Árvore", 
     xlab = "Comparações", ylab = "Frequência",
     col = "lightblue",
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

# Gráfico 5: Gráfico de barras para 30 números aleatórios
barpos <- barplot(t(barras_dados), beside = TRUE,
                  col = c("lightcoral", "lightblue"),
                  main = "30 Buscas Aleatórias",
                  xlab = "Buscas Aleatórias", 
                  ylab = "Número de Comparações",
                  ylim = c(0, max(barras_dados) * 1.1),
                  cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0,
                  legend.text = c("Lista", "Árvore"),
                  args.legend = list(x = "topright", bg = "white", cex = 0.9))

# Salvar os gráficos em arquivo PNG
png("comparacoes_estruturas.png", width = 1800, height = 1000)
par(mfrow = c(2, 3), mar = c(5, 4, 4, 2))  # Margens ajustadas

# Gráfico 1 no arquivo
plot(dados$comparacoes_lista, type = "l", col = "red", 
     main = "Comparações por Busca",
     xlab = "Número da Busca", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista, dados$comparacoes_arvore))),
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)
lines(dados$comparacoes_arvore, col = "blue")
legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, bg = "white", cex = 0.9)

# Gráfico 2 no arquivo
plot(buscas, dados$comparacoes_lista[1:n_buscas], type = "o", col = "red",
     main = "Detalhe: Primeiras 50 Buscas",
     xlab = "Número Buscado", ylab = "Número de Comparações",
     ylim = c(0, max(c(dados$comparacoes_lista[1:n_buscas], 
                       dados$comparacoes_arvore[1:n_buscas]))),
     xaxt = "n", pch = 16, lwd = 2,
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

axis(1, at = buscas, labels = numeros_buscados, cex.axis = 0.5, las = 2)

lines(buscas, dados$comparacoes_arvore[1:n_buscas], type = "o", 
      col = "blue", pch = 17, lwd = 2)

text(buscas, dados$comparacoes_lista[1:n_buscas], 
     dados$comparacoes_lista[1:n_buscas], pos = 3, cex = 0.5, col = "black")
text(buscas, dados$comparacoes_arvore[1:n_buscas], 
     dados$comparacoes_arvore[1:n_buscas], pos = 3, cex = 0.5, col = "black")

legend("topright", legend = c("Lista", "Árvore"), 
       col = c("red", "blue"), lty = 1, pch = c(16, 17),
       bg = "white", box.lwd = 1, cex = 0.9)

# Gráfico 3 no arquivo
hist(dados$comparacoes_lista, breaks = 20, 
     main = "Histograma - Lista",
     xlab = "Comparações", ylab = "Frequência",
     col = "lightcoral",
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

# Gráfico 4 no arquivo
hist(dados$comparacoes_arvore, breaks = 20,
     main = "Histograma - Árvore", 
     xlab = "Comparações", ylab = "Frequência",
     col = "lightblue",
     cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0)

# Gráfico 5 no arquivo
barpos <- barplot(t(barras_dados), beside = TRUE,
                  col = c("lightcoral", "lightblue"),
                  main = "30 Buscas Aleatórias",
                  xlab = "Buscas Aleatórias", 
                  ylab = "Número de Comparações",
                  ylim = c(0, max(barras_dados) * 1.1),
                  cex.main = 1.2, cex.lab = 1.1, cex.axis = 1.0,
                  legend.text = c("Lista", "Árvore"),
                  args.legend = list(x = "topright", bg = "white", cex = 0.9))

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
