# Guia de Benchmarks

## Visão geral

O conjunto de benchmarks fornece avaliação automatizada dos dois programas implementados neste projeto: `sum` e `pub_sub`. Os scripts coletam medições de tempo de execução, agregam resultados e geram gráficos de visualização para facilitar a análise de desempenho.

## Início rápido

Para executar todos os benchmarks e gerar gráficos, execute o seguinte na raiz do projeto:

```bash
cd benchmarks
make
```

Este único comando irá:

1. Compilar os programas C em `src/`.
2. Executar os scripts de benchmark para ambos os módulos.
3. Gerar gráficos de desempenho.

## Estrutura do diretório

Os benchmarks estão organizados da seguinte forma:

```
benchmarks/
  Makefile
  README.md
  sum/
    bench.py
    plot.py
    sum_results.csv
    sum_performance.png
  pub_sub/
    bench.py
    plot.py
    pub_sub_results.csv
    pub_sub_performance.png
```

Cada módulo possui scripts dedicados para benchmark e visualização.

## Pré-requisitos

Garantir que o seguinte esteja instalado e disponível:

- `python3` com as bibliotecas `pandas` e `matplotlib`
- `gcc-14` (ou compilador C compatível com o padrão C11)
- `make`
- Ambiente compatível com POSIX com suporte a `pthread`

Instale as dependências do Python com:

```bash
pip install pandas matplotlib
```
No caso de utilização numa distro Linux, como o Ubuntu, instale as bibliotecas com o gerenciador de pacotes nativos, como o `apt-get`

```bash
sudo apt-get install python3-pandas python3-matplotlib
```

## Alvos disponíveis do Make

### Fluxo completo

```bash
make
```

Compila os programas, executa benchmarks e gera todos os gráficos.

### Apenas compilação

```bash
make build
```

Compila os programas C sem executar benchmarks.

### Executar benchmarks

```bash
make bench
```

Executa todos os scripts de benchmark.

```bash
make sum-bench
make pub_sub-bench
```

Executa benchmarks para módulos individuais.

### Gerar gráficos

```bash
make plot
```

Cria gráficos de visualização a partir dos arquivos CSV existentes.

```bash
make sum-plot
make pub_sub-plot
```

Gera gráficos para módulos individuais.

### Limpeza

```bash
make clean
```

Remove todos os arquivos gerados: binários, resultados CSV e gráficos PNG.

## Programa: `sum`

### Descrição do benchmark

O benchmark `sum` mede o desempenho do algoritmo de soma paralela em diferentes tamanhos de vetor e quantidades de threads. O script repete cada configuração várias vezes para levar em conta a variabilidade do sistema e relata o tempo médio de execução.

### Configuração

- **Tamanhos de vetor**: 10^7 e 10^8 elementos
- **Quantidades de threads**: 1, 2, 4, 8, 16, 32, 64, 128, 256
- **Repetições**: 10 execuções por configuração

### Saída

Os resultados são salvos em `benchmarks/sum/sum_results.csv` com as seguintes colunas:

- `N`: tamanho do vetor
- `K`: número de threads
- `tempo_ms`: tempo médio de execução em milissegundos

Um gráfico comparando tempos de execução em diferentes tamanhos de vetor é salvo como `benchmarks/sum/sum_performance.png`.

### Interpretação

O gráfico exibe o tempo de execução no eixo Y e a quantidade de threads (em escala logarítmica) no eixo X. Observe:

- Como o tempo de execução muda conforme as threads aumentam;
- se existe uma quantidade ótima de threads;
- diferenças de escalabilidade entre os tamanhos de vetor.

## Programa: `pub_sub`

### Descrição do benchmark

O benchmark `pub_sub` avalia o sistema produtor-consumidor em diferentes tamanhos de buffer e configurações de threads produtor-consumidor. O desempenho é medido como o tempo total de execução para processar um número fixo de itens.

### Configuração

- **Tamanhos de buffer**: 1, 10, 100, 1000 posições
- **Configurações de threads**: (1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1) representando (produtores, consumidores)
- **Itens a processar**: 100.000
- **Repetições**: 10 execuções por configuração

### Saída

Os resultados são salvos em `benchmarks/pub_sub/pub_sub_results.csv` com as seguintes colunas:

- `buffer_size`: capacidade do buffer circular
- `producers`: número de threads produtoras
- `consumers`: número de threads consumidoras
- `average_time_ms`: tempo médio de execução em milissegundos

Um gráfico comparando tempos de execução em diferentes tamanhos de buffer e configurações de threads é salvo como `benchmarks/pub_sub/pub_sub_performance.png`.

### Interpretação

O gráfico mostra o tempo de execução para cada configuração produtor-consumidor, agrupado por tamanho de buffer. Observe:

- Como diferentes tamanhos de buffer afetam o desempenho geral;
- o impacto das proporções de threads na vazão;
- comportamento do sistema em diferentes proporções produtor-consumidor.

## Executando benchmarks individuais

Para fazer benchmark de apenas um programa:

```bash
cd benchmarks
python3 sum/bench.py
python3 sum/plot.py

python3 pub_sub/bench.py
python3 pub_sub/plot.py
```

Esta abordagem é útil para desenvolvimento iterativo ou ao modificar parâmetros de benchmark.

## Modificando parâmetros de benchmark

Os scripts de benchmark estão localizados em `benchmarks/sum/bench.py` e `benchmarks/pub_sub/bench.py`. Para ajustar os parâmetros de teste, edite as definições de constantes no início de cada script:

### Para `sum`:

```python
VECTOR_SIZES = [10**7, 10**8]
THREAD_COUNTS = [1, 2, 4, 8, 16, 32, 64, 128, 256]
REPETITIONS = 10
```

### Para `pub_sub`:

```python
BUFFER_SIZES = [1, 10, 100, 1000]
CONFIGURATIONS = [(1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1)]
MAX_ITEMS = 100000
REPETITIONS = 10
```

## Notas sobre reprodutibilidade

Ambos os programas em teste geram dados de entrada aleatórios. Como resultado:

- Os tempos de execução variarão entre as execuções.
- Os scripts de benchmark mitigam essa variabilidade por meio de repetição e média.
- Fatores ambientais, como carga do sistema e escalonamento de frequência da CPU, podem afetar os resultados.

Para resultados mais consistentes, execute benchmarks em isolamento e quando o sistema estiver inativo.

## Resolução de problemas

### "No such file or directory" para binários

Garantir que os programas C sejam compilados antes de executar benchmarks:

```bash
make build
```

### Falha na geração de gráficos

Verifique se `matplotlib` e `pandas` estão instalados:

```bash
python3 -m pip install pandas matplotlib
```

### Benchmarks travarem ou demorarem muito

O conjunto de benchmarks pode levar tempo significativo dependendo do sistema. Para testes mais rápidos, reduza o valor de `REPETITIONS` ou `VECTOR_SIZES` e `BUFFER_SIZES` nos scripts respectivos.

### Arquivos CSV vazios ou corrompidos

Exclua os arquivos CSV e execute novamente os benchmarks:

```bash
make clean
make bench
```

## Fluxo de análise de desempenho

1. **Executar benchmarks**: Execute `make` para coletar dados de tempo.
2. **Examinar gráficos**: Revise os arquivos PNG gerados para identificar padrões e anomalias.
3. **Analisar dados CSV**: Carregue os arquivos CSV com ferramentas de planilha ou análise de dados para inspeção detalhada.
4. **Ajustar parâmetros**: Modifique as configurações de benchmark se necessário e execute novamente.
5. **Documentar conclusões**: Registre observações e conclusões para referência futura.
