# FastPropagation - Implementação de Algoritmos de Geração Procedural de Conteúdo

## Resumo

Este projeto implementa uma comparação entre diferentes algoritmos de geração procedural de conteúdo baseados em restrições, incluindo Wave Function Collapse (WFC), Fast Propagation (FP) e Nested Wave Function Collapse (NWFC). A implementação foi desenvolvida em C++ e permite a geração de mapas bidimensionais utilizando conjuntos de tiles com restrições de adjacência.

## Arquitetura do Sistema

### 1. Estrutura de Classes Fundamentais

#### 1.1 Classe Tile (`Tile.hpp` / `Tile.cpp`)

A classe `Tile` representa a unidade básica do sistema, definindo as restrições de adjacência para cada elemento do conjunto de tiles.

**Atributos:**
- `std::string id`: Identificador único do tile
- `std::string north, south, east, west`: Restrições de adjacência para cada direção cardinal

**Métodos principais:**
- `set_tile_constraints(std::string constraints, std::string id)`: Define as restrições do tile a partir de uma string de 4 caracteres
- `print_tile_constraints()`: Método auxiliar para visualização das restrições

#### 1.2 Classe Cell (`Cell.hpp` / `Cell.cpp`)

A classe `Cell` representa uma célula individual na matriz de geração, mantendo o domínio de tiles possíveis e o estado de colapso.

**Atributos:**
- `int collapsed`: Indica se a célula foi colapsada (-1 para não colapsada, ID do tile caso contrário)
- `std::vector<Tile> domain`: Conjunto de tiles válidos para esta posição

**Métodos principais:**
- `print_domain()`: Visualiza todos os tiles possíveis no domínio
- `print_domain_size()`: Exibe o tamanho atual do domínio

#### 1.3 Classe Matrix (`Matrix.hpp` / `Matrix.cpp`)

A classe `Matrix` gerencia a grade bidimensional de células, fornecendo funcionalidades de inicialização e visualização.

**Atributos:**
- `std::vector<std::vector<Cell>> matrix`: Matriz bidimensional de células
- `int rows, columns`: Dimensões da matriz

**Métodos principais:**
- `initialize_matrix(int rows, int columns, Cell c)`: Inicializa a matriz com células idênticas
- `print_possibilities()`: Visualiza o número de possibilidades em cada célula
- `print_ids()`: Exibe os IDs dos tiles colapsados na matriz

### 2. Módulo de Leitura e Processamento

#### 2.1 Classe Reader (`Reader.hpp` / `Reader.cpp`)

Responsável pela leitura e processamento dos conjuntos de tiles a partir do sistema de arquivos.

**Funcionalidades:**
- `read_files(std::string filepath)`: Lê todos os arquivos PNG de um diretório e extrai os nomes como restrições
- `generate_domain()`: Cria o domínio inicial de tiles com base nos arquivos encontrados
- `print_constraints()`: Método auxiliar para visualização das restrições carregadas

### 3. Algoritmos de Geração

#### 3.1 Fast Propagation (`FastPropagation.hpp` / `FastPropagation.cpp`)

Implementa o algoritmo Fast Propagation, uma versão otimizada para cenários onde a propagação completa não é necessária.

**Características técnicas:**
- **Ordem de processamento**: Linear (linha por linha) ou diagonal
- **Estratégia de colapso**: Seleção aleatória uniforme do domínio
- **Propagação**: Limitada aos vizinhos diretos (direita e abaixo)

**Métodos principais:**
- `initialize_fp(int rows, int columns, Cell c, unsigned int seed)`: Inicialização com semente para reprodutibilidade
- `FP()`: Implementação do algoritmo com ordem linear
- `Diag()`: Implementação com processamento em anti-diagonais
- `collapse(int i, int j)`: Colapsa uma célula selecionando aleatoriamente do domínio
- `propagate(int i, int j)`: Propaga restrições para vizinhos não processados

#### 3.2 Wave Function Collapse (`WFC.hpp` / `WFC.cpp`)

Implementação completa do algoritmo Wave Function Collapse com propagação de restrições AC-3.

**Características técnicas:**
- **Heurística de seleção**: Minimum Remaining Values (MRV) ou processamento diagonal
- **Propagação**: AC-3 completo com fila de arcos
- **Garantias**: Satisfação global de restrições

**Métodos principais:**
- `initialize_wfc(int rows, int columns, Cell c, unsigned int seed)`: Inicialização do algoritmo
- `MRV()`: Implementação da heurística MRV para seleção de células
- `Diag()`: Processamento em ordem diagonal
- `propagate(int start_i, int start_j)`: Implementação AC-3 com fila de arcos para propagação global

**Detalhes da implementação AC-3:**
```cpp
bool is_compatible(const Tile& t_from, const Tile& t_to, int direction)
```
Função que verifica compatibilidade entre tiles adjacentes considerando as restrições direcionais.

#### 3.3 Nested Wave Function Collapse (`NWFC.hpp` / `NWFC.cpp`)

Algoritmo hierárquico que divide o problema em subgrids sobrepostos, aplicando WFC localmente.

**Características técnicas:**
- **Decomposição**: División en subgrids de tamaño configurable
- **Sobreposição**: Células compartilhadas entre subgrids adjacentes para continuidade
- **Algoritmo local**: WFC aplicado em cada subgrid
- **Propagação entre subgrids**: Manutenção de consistência nas bordas

**Métodos principais:**
- `initialize_nwfc(int rows, int columns, int subgrid_size, Cell c, unsigned int seed)`: Inicialização com tamanho de subgrid
- `run()`: Processamento sequencial de subgrids com propagação de restrições

### 4. Geração de Imagens

#### 4.1 Classe ImageGenerator (`ImageGenerator.hpp` / `ImageGenerator.cpp`)

Responsável pela renderização visual dos resultados usando a biblioteca STB.

**Funcionalidades:**
- **Carregamento de tiles**: Leitura automática de imagens PNG dos tiles
- **Composição**: Montagem da imagem final através da concatenação de tiles
- **Otimização de memória**: Verificação de limites de memória antes da alocação
- **Formatos suportados**: PNG e JPG para saída

**Métodos principais:**
- `initialize(const Reader& reader, const std::string& folder_path)`: Inicialização com mapeamento de IDs para arquivos
- `generate_image(const Matrix& matrix, const std::string& output_filename)`: Geração da imagem final

### 5. Sistema de Execução

#### 5.1 Programa Principal (`main.cpp`)

O programa principal implementa um sistema de benchmark completo com suporte a múltiplas execuções e análise estatística.

**Parâmetros de entrada:**
```
main <algoritmo> <pasta> <tamanho_matriz> <seed> <gerar_imagem> <num_runs> [tamanho_subgrid]
```

**Algoritmos suportados:**
- `FP`: Fast Propagation linear
- `FP_DIAGONAL`: Fast Propagation diagonal
- `WFC`: Wave Function Collapse com MRV
- `WFC_DIAGONAL`: Wave Function Collapse diagonal
- `NWFC`: Nested Wave Function Collapse

**Conjuntos de tiles disponíveis:**
- `Roads`, `Roads--`, `Roads++`: Conjuntos de estradas com diferentes complexidades
- `Carcassonne`, `Carcassonne++`: Baseados no jogo Carcassonne
- `Artigo`, `Carnaval`, `Completo`, `Incompleto`: Conjuntos experimentais

**Sistema de benchmark:**
- **Múltiplas execuções**: Suporte para N execuções com sementes incrementais
- **Métricas temporais**: 
  - Tempo de inicialização
  - Tempo de execução do algoritmo
  - Tempo total de execução
- **Análise estatística**: Cálculo automático de médias e totais
- **Reprodutibilidade**: Sistema de sementes para resultados determinísticos

**Exemplo de saída:**
```
Running WFC on 10x10 grid with tileset 'Roads' and seed 1234 for 5 runs
Run 1/5...
Run 2/5...
...

=== RESULTS SUMMARY ===
Number of runs: 5
Read constraints: 2.345 ms
Average init time: 0.123 ms
Average run time: 45.678 ms
Average total execution time: 45.801 ms
Total init time: 0.615 ms
Total run time: 228.390 ms
Total execution time: 229.005 ms
```

## Explicação dos Algoritmos

### 1. Fast Propagation (FP)

O **Fast Propagation** é um algoritmo simplificado que prioriza velocidade sobre garantias de satisfação global de restrições. Sua principal característica é a propagação limitada apenas aos vizinhos não processados.

**Funcionamento:**
- **Ordem de processamento**: Linear (linha por linha) ou diagonal
- **Estratégia de colapso**: Seleção aleatória uniforme do domínio local
- **Propagação**: Restrita apenas às células adjacentes ainda não processadas (direita e abaixo)
- **Vantagens**: Extremamente rápido, complexidade O(n²)
- **Limitações**: Não garante satisfação global de restrições, pode gerar inconsistências

**Variantes implementadas:**
- `FP`: Processamento sequencial linha por linha
- `FP_DIAGONAL`: Processamento em anti-diagonais para melhor distribuição de restrições

### 2. Wave Function Collapse (WFC)

O **Wave Function Collapse** é o algoritmo padrão para geração procedural baseada em restrições, garantindo satisfação global através de propagação completa.

**Funcionamento:**
- **Heurística de seleção**: Minimum Remaining Values (MRV) - seleciona sempre a célula com menor domínio
- **Estratégia de colapso**: Seleção aleatória ponderada ou uniforme
- **Propagação**: AC-3 completo com fila de arcos para garantir consistência global
- **Vantagens**: Garantia de satisfação de restrições, resultados sempre válidos
- **Limitações**: Maior complexidade computacional O(n²d), onde d é o tamanho médio do domínio

**Detalhes da implementação AC-3:**
```cpp
// Propagação global usando fila de arcos
std::deque<std::tuple<int,int,int>> queue; // (linha, coluna, direção)
```

**Variantes implementadas:**
- `WFC`: Utiliza heurística MRV para seleção ótima de células
- `WFC_DIAGONAL`: Processamento em ordem diagonal com propagação AC-3

### 3. Nested Wave Function Collapse (NWFC)

O **Nested Wave Function Collapse** é uma abordagem hierárquica que decompõe o problema em subgrids sobrepostos, aplicando WFC localmente em cada região.

**Funcionamento:**
- **Decomposição**: Divisão da matriz em subgrids de tamanho configurável com sobreposição
- **Processamento sequencial**: Cada subgrid é resolvido usando WFC antes de prosseguir
- **Continuidade**: Células nas bordas dos subgrids mantêm consistência entre regiões adjacentes
- **Propagação inter-subgrid**: Restrições são propagadas entre subgrids através das células compartilhadas

**Características técnicas:**
- **Tamanho da matriz resultante**: `(n*(k-1)+1) × (m*(k-1)+1)` onde k é o tamanho do subgrid
- **Sobreposição**: Células nas bordas são compartilhadas entre subgrids adjacentes
- **Vantagens**: Melhor controle local, possibilidade de paralelização, redução da complexidade por região
- **Limitações**: Requer gerenciamento cuidadoso das bordas, pode introduzir artifacts nas transições

**Processo de execução:**
1. Criação de subgrids sobrepostos
2. Cópia do estado atual para cada subgrid
3. Aplicação de restrições de borda (células já colapsadas)
4. Execução do WFC local
5. Cópia dos resultados de volta para a matriz principal

### Comparação de Performance

| Algoritmo | Complexidade | Garantias | Velocidade | Uso Recomendado |
|-----------|-------------|-----------|------------|----------------- |
| **FP** | O(n²) | Locais apenas | Muito Alta | Prototipagem rápida, demonstrações |
| **FP_DIAGONAL** | O(n²) | Locais apenas | Muito Alta | Melhor distribuição que FP linear |
| **WFC** | O(n²d) | Globais | Moderada | Resultados de alta qualidade |
| **WFC_DIAGONAL** | O(n²d) | Globais | Moderada | Padrões direcionais específicos |
| **NWFC** | O(k²×(n/k)²×d) | Regionais | Variável | Mapas grandes, controle hierárquico |

### Considerações de Implementação

**Fast Propagation:**
- Ideal para casos onde velocidade é prioritária
- Recomendado para conjuntos de tiles com alta compatibilidade
- Pode requerer pós-processamento para corrigir inconsistências

**Wave Function Collapse:**
- Algoritmo de referência para qualidade
- Garante sempre resultados válidos
- Tempo de execução pode variar significativamente baseado na complexidade das restrições

**Nested Wave Function Collapse:**
- Melhor escalabilidade para matrizes grandes
- Permite controle fino sobre diferentes regiões
- Requer ajuste cuidadoso do tamanho do subgrid para resultados ótimos

## Considerações Técnicas

### Complexidade Computacional

- **Fast Propagation**: O(n²) para grade n×n, propagação limitada
- **Wave Function Collapse**: O(n²d) onde d é o tamanho médio do domínio
- **NWFC**: O(k²×(n/k)²×d) onde k é o tamanho do subgrid

### Otimizações Implementadas

1. **Uso de move semantics** para operações de domínio
2. **Verificação de limites de memória** na geração de imagens
3. **Propagação incremental** no WFC
4. **Reutilização de domínios** no NWFC

### Dependências

- **STB Image/Image Write**: Para processamento de imagens
- **C++17**: Para std::filesystem e recursos modernos
- **Biblioteca padrão**: chrono, random, vector, deque

## Conclusões

Esta implementação fornece uma base sólida para pesquisa em algoritmos de geração procedural de conteúdo, oferecendo tanto implementações eficientes quanto ferramentas de análise robustas. A arquitetura modular permite fácil extensão para novos algoritmos e conjuntos de tiles, enquanto o sistema de benchmark facilita a comparação objetiva entre diferentes abordagens.
