# Trabalho Prático 2 - Sistemas Distribuídos

## Visão geral

Este projeto reúne duas implementações concorrentes em C:

1. `sum`, que compara a soma sequencial de um vetor com uma soma paralela realizada com threads.
2. `pub_sub`, que implementa o problema clássico produtor-consumidor com buffer circular, semáforos e controle de término.

Os arquivos de interface pública estão em `include/` e as implementações estão em `src/`. O diretório `src/` também contém o `Makefile` responsável pela compilação dos binários.

## Organização do projeto

```text
include/
	sum.h
	pub_sub.h
src/
	Makefile
	sum.c
	pub_sub.c
```

O cabeçalho de cada módulo contém apenas os tipos e protótipos necessários para a sua compilação. As definições das funções permanecem nos respectivos arquivos `.c`, o que reduz acoplamento e segue a prática usual de separar interface e implementação.

## Requisitos

O projeto depende de um ambiente POSIX com suporte a:

- compilador C compatível com C11;
- `pthread`;
- semáforos POSIX;
- `make`.

## Compilação

Entre no diretório `src/` para compilar os programas.

```bash
cd src
make
```

O comando acima compila os dois executáveis:

- `sum`
- `pub_sub`

Também é possível compilar apenas uma das partes:

```bash
make sum
make pub_sub
```

Para remover os artefatos gerados:

```bash
make clean
```

O alvo `clean` remove os binários gerados e o arquivo `buffer_occupancy.csv`, produzido por `pub_sub` durante a execução.

## Makefile

O `Makefile` foi estruturado para oferecer três formas de uso:

- `make`: compila todos os programas do projeto;
- `make sum`: compila somente o executável `sum`;
- `make pub_sub`: compila somente o executável `pub_sub`.

A compilação utiliza as opções:

- `-std=c11`, para garantir a linguagem C11;
- `-Wall -Wextra -pedantic`, para aumentar a qualidade das verificações;
- `-O2`, para otimização;
- `-I../include`, para localizar os cabeçalhos;
- `-pthread`, para habilitar suporte a threads.

## Programa `sum`

### Objetivo

O programa `sum` recebe o tamanho do vetor e a quantidade de threads. Ele então gera um vetor aleatório, calcula a soma sequencial como referência e calcula a soma paralela para validação.

### Execução

```bash
./sum <N> <K>
```

Onde:

- `N` é a quantidade de elementos do vetor;
- `K` é o número de threads.

### Lógica de funcionamento

O fluxo principal do programa é o seguinte:

1. Aloca um vetor de `int8_t` com `N` posições.
2. Preenche o vetor com valores aleatórios no intervalo de `-100` a `100`.
3. Calcula a soma sequencial para servir como referência de corretude.
4. Divide o vetor em `K` blocos contíguos.
5. Cria `K` threads, cada uma responsável por somar seu bloco localmente.
6. Cada thread acumula o resultado em uma variável local e, ao final, atualiza a soma global protegida por um spinlock.
7. Ao término, compara a soma paralela com a soma sequencial e imprime o resultado.

### Concorrência

O compartilhamento da soma global é protegido por um spinlock implementado com `atomic_flag`. A decisão evita condições de corrida quando múltiplas threads atualizam o acumulador comum.

### Medição de tempo

O tempo total da parte paralela é medido com `clock_gettime(CLOCK_MONOTONIC, ...)` e convertido para milissegundos por meio da função `time_ms`.

## Programa `pub_sub`

### Objetivo

O programa `pub_sub` implementa um sistema produtor-consumidor com buffer circular. Produtores inserem valores inteiros no buffer, consumidores removem esses valores e aplicam uma verificação de primalidade.

### Execução

```bash
./pub_sub <buffer_size> <producers> <consumers> <max_items>
```

Onde:

- `buffer_size` é a capacidade do buffer circular;
- `producers` é a quantidade de threads produtoras;
- `consumers` é a quantidade de threads consumidoras;
- `max_items` é o número máximo de itens consumidos antes da parada.

### Lógica de funcionamento

O programa utiliza os seguintes elementos:

- um buffer circular compartilhado;
- índices de entrada e saída para controle do buffer;
- um contador de ocupação atual;
- três semáforos:
	- `empty_slots`, que representa posições livres;
	- `full_slots`, que representa posições ocupadas;
	- `mutex`, que protege a região crítica do buffer;
- variáveis atômicas para controlar a quantidade de itens consumidos e o término da execução.

O fluxo operacional é o seguinte:

1. O programa aloca o buffer e a memória usada para registrar a ocupação histórica.
2. Inicializa os semáforos com o número adequado de vagas livres, vagas ocupadas e exclusão mútua.
3. Cria as threads produtoras e consumidoras.
4. Cada produtor gera números aleatórios positivos e tenta inseri-los no buffer circular.
5. Cada consumidor retira um valor do buffer, atualiza a ocupação e executa a função de teste de primalidade.
6. A cada inserção ou remoção, a ocupação do buffer é registrada em memória para posterior exportação.
7. Quando o número de itens consumidos atinge `max_items`, o programa sinaliza parada, libera threads possivelmente bloqueadas e encerra a execução.
8. Ao final, gera o arquivo `buffer_occupancy.csv` com o histórico da ocupação do buffer.

### Controle de concorrência

A sincronização entre produtores e consumidores é feita com semáforos POSIX. O par `empty_slots` e `full_slots` controla a disponibilidade de espaço e de itens no buffer, enquanto `mutex` garante acesso exclusivo às estruturas compartilhadas durante a atualização dos índices e da ocupação.

### Saída gerada

Além das mensagens na saída padrão, o programa cria o arquivo `buffer_occupancy.csv` no diretório corrente. Esse arquivo contém a evolução da ocupação do buffer ao longo da execução.

## Observações sobre a implementação

- Os dois programas foram organizados para manter as definições das funções nos arquivos `.c` e as declarações públicas nos arquivos `.h`.
- A configuração do `Makefile` favorece a compilação separada, o que facilita testes e manutenção.
- O projeto foi escrito com foco em clareza estrutural e na demonstração de mecanismos básicos de concorrência em C.
