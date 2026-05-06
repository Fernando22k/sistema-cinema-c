#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===================== CONSTANTES =====================
#define MAX_FILA     50
#define MAX_PILHA    30
#define MAX_SALA     50
#define NUM_FILMES    3

// ===================== STRUCTS =====================
typedef struct {
    char nome_filme[100];
    int  sala;
    char horario[10];
    float preco;
} Ingresso;

typedef struct {
    char nome[100];
    int  idade;
    Ingresso ingresso;
} Cliente;

// ===================== PILHA (vetor) =====================
typedef struct {
    Ingresso dados[MAX_PILHA];
    int topo;
} Pilha;

void pilha_init(Pilha *p) { p->topo = -1; }
int  pilha_vazia(Pilha *p) { return p->topo == -1; }
int  pilha_cheia(Pilha *p) { return p->topo == MAX_PILHA - 1; }

int empilha(Pilha *p, Ingresso ing) {
    if (pilha_cheia(p)) return 0;
    p->dados[++(p->topo)] = ing;                           
    return 1;
}

int desempilha(Pilha *p, Ingresso *ing) {
    if (pilha_vazia(p)) return 0;
    *ing = p->dados[(p->topo)--];
    return 1;
}

void imprimir_pilha(Pilha *p, const char *titulo) {
    printf("\n===== %s =====\n", titulo);
    if (pilha_vazia(p)) { printf("  [pilha vazia]\n"); return; }
    for (int i = p->topo; i >= 0; i--) {
        printf("  [%d] Filme: %-25s | Sala: %d | Horario: %s | Preco: R$%.2f\n",
            i, p->dados[i].nome_filme, p->dados[i].sala,
            p->dados[i].horario, p->dados[i].preco);
    }
}

// ===================== FILA CIRCULAR (vetor) =====================
typedef struct {
    Cliente dados[MAX_FILA];
    int inicio, fim, tamanho;
} Fila;

void fila_init(Fila *f) { f->inicio = 0; f->fim = 0; f->tamanho = 0; }
int  fila_vazia(Fila *f) { return f->tamanho == 0; }
int  fila_cheia(Fila *f) { return f->tamanho == MAX_FILA; }

int enfileira(Fila *f, Cliente c) {
    if (fila_cheia(f)) return 0;
    f->dados[f->fim] = c;
    f->fim = (f->fim + 1) % MAX_FILA;
    f->tamanho++;
    return 1;
}

int desenfileira(Fila *f, Cliente *c) {
    if (fila_vazia(f)) return 0;
    *c = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % MAX_FILA;
    f->tamanho--;
    return 1;
}

// Busca cliente na fila pelo nome (nao remove)
int buscar_fila(Fila *f, const char *nome) {
    for (int i = 0; i < f->tamanho; i++) {
        int idx = (f->inicio + i) % MAX_FILA;
        if (strcmp(f->dados[idx].nome, nome) == 0) return idx;
    }
    return -1;
}

// Busca ingresso na pilha pelo nome do filme (nao remove)
int buscar_pilha(Pilha *p, const char *filme) {
    for (int i = p->topo; i >= 0; i--) {
        if (strcmp(p->dados[i].nome_filme, filme) == 0) return i;
    }
    return -1;
}

void imprimir_fila(Fila *f, const char *titulo) {
    printf("\n===== %s =====\n", titulo);
    if (fila_vazia(f)) { printf("  [fila vazia]\n"); return; }
    for (int i = 0; i < f->tamanho; i++) {
        int idx = (f->inicio + i) % MAX_FILA;
        Cliente *c = &f->dados[idx];
        printf("  [%d] %-20s | Idade: %d", i+1, c->nome, c->idade);
        if (strlen(c->ingresso.nome_filme) > 0)
            printf(" | Ingresso: %s (Sala %d, %s)", c->ingresso.nome_filme,
                   c->ingresso.sala, c->ingresso.horario);
        printf("\n");
    }
}

// ===================== DADOS GLOBAIS =====================
// Nomes dos filmes e salas

char *nomes_filmes[NUM_FILMES]  = {"Oppenheimer", "Barbie", "Interestelar"};
int   salas_filmes[NUM_FILMES]  = {1, 2, 3};
char *horarios[NUM_FILMES]      = {"15:00", "18:00", "21:00"};
float precos[NUM_FILMES]        = {30.00f, 27.00f, 35.00f};

Pilha pilhas[NUM_FILMES];     // 3 pilhas de ingressos
Fila  fila_clientes;          // fila principal de compra
Fila  filas_sala[NUM_FILMES]; // fila de exibicao por sala

// ===================== PREENCHER AUTOMATICO =====================
void preencher_automatico() {
    // --- Pilhas de ingressos ---
    for (int f = 0; f < NUM_FILMES; f++) {
        pilha_init(&pilhas[f]);
        for (int i = 0; i < 10; i++) {
            Ingresso ing;
            strcpy(ing.nome_filme, nomes_filmes[f]);
            ing.sala = salas_filmes[f];
            strcpy(ing.horario, horarios[f]);
            ing.preco = precos[f];
            empilha(&pilhas[f], ing);
        }
    }

    // --- Fila de clientes ---
fila_init(&fila_clientes);

char *nomes[] = {
    "Lucas Andrade", "Mariana Silva", "Pedro Henrique", "Juliana Costa",
    "Rafael Martins", "Beatriz Souza", "Thiago Oliveira", "Camila Fernandes",
    "Gabriel Rocha", "Larissa Alves"
};

int idades[] = {21, 27, 19, 24, 30, 22, 28, 26, 23, 25};

for (int i = 0; i < 10; i++) {
    Cliente c;
    strcpy(c.nome, nomes[i]);
    c.idade = idades[i];
    memset(&c.ingresso, 0, sizeof(Ingresso));
    enfileira(&fila_clientes, c);
}

    // --- Filas de sala ---
    for (int i = 0; i < NUM_FILMES; i++) fila_init(&filas_sala[i]);

    printf("\n[OK] Dados preenchidos: 10 ingressos por filme, 10 clientes na fila.\n");
}

// ===================== VENDER INGRESSO =====================
void vender_ingresso() {
    if (fila_vazia(&fila_clientes)) {
        printf("\n[!] Nao ha clientes na fila.\n");
        return;
    }

    printf("\nEscolha o filme:\n");
    for (int i = 0; i < NUM_FILMES; i++)
        printf("  %d - %s (Sala %d, %s) - R$%.2f\n",
               i+1, nomes_filmes[i], salas_filmes[i], horarios[i], precos[i]);
    printf("Opcao: ");

    int op;
    scanf("%d", &op);
    op--;
    if (op < 0 || op >= NUM_FILMES) { printf("[!] Opcao invalida.\n"); return; }
    if (pilha_vazia(&pilhas[op])) { printf("[!] Sem ingressos para este filme.\n"); return; }

    Cliente c;
    Ingresso ing;
    desenfileira(&fila_clientes, &c);
    desempilha(&pilhas[op], &ing);
    c.ingresso = ing;

    printf("\n>>> VENDA REALIZADA:\n");
    printf("    Cliente : %s (idade %d)\n", c.nome, c.idade);
    printf("    Ingresso: %s | Sala %d | %s | R$%.2f\n",
           ing.nome_filme, ing.sala, ing.horario, ing.preco);

    enfileira(&filas_sala[op], c);
    printf("    [%s enfileirado na Fila da Sala %d]\n", c.nome, ing.sala);
}

void vender_todos() {
    if (fila_vazia(&fila_clientes)) {
        printf("\n[!] Nao ha clientes na fila.\n");
        return;
    }
    printf("\nEscolha o filme para todos os clientes:\n");
    for (int i = 0; i < NUM_FILMES; i++)
        printf("  %d - %s\n", i+1, nomes_filmes[i]);
    printf("Opcao: ");
    int op; scanf("%d", &op); op--;
    if (op < 0 || op >= NUM_FILMES) { printf("[!] Invalido.\n"); return; }

    printf("\n--- Vendendo ingressos ate acabar a fila ou a pilha ---\n");
    while (!fila_vazia(&fila_clientes) && !pilha_vazia(&pilhas[op])) {
        Cliente c; Ingresso ing;
        desenfileira(&fila_clientes, &c);
        desempilha(&pilhas[op], &ing);
        c.ingresso = ing;
        printf("  %s -> %s (Sala %d, %s)\n", c.nome, ing.nome_filme, ing.sala, ing.horario);
        enfileira(&filas_sala[op], c);
    }
    if (!fila_vazia(&fila_clientes)) printf("[!] Ingressos acabaram antes dos clientes.\n");
    if (fila_vazia(&fila_clientes))  printf("[OK] Todos os clientes atendidos.\n");
}

// ===================== BUSCA =====================
void menu_busca() {
    int op;
    printf("\nBuscar em:\n  1 - Fila de clientes (por nome)\n  2 - Pilha de ingressos (por filme)\nOpcao: ");
    scanf("%d", &op);
    if (op == 1) {
        char nome[100];
        printf("Nome do cliente: ");
        scanf(" %[^\n]", nome);
        int idx = buscar_fila(&fila_clientes, nome);
        if (idx == -1) printf("[!] Cliente nao encontrado na fila.\n");
        else {
            Cliente *c = &fila_clientes.dados[idx];
            printf("[OK] Encontrado: %s | Idade: %d\n", c->nome, c->idade);
        }
    } else if (op == 2) {
        printf("Escolha o filme:\n");
        for (int i = 0; i < NUM_FILMES; i++) printf("  %d - %s\n", i+1, nomes_filmes[i]);
        printf("Opcao: ");
        int f; scanf("%d", &f); f--;
        if (f < 0 || f >= NUM_FILMES) { printf("[!] Invalido.\n"); return; }
        int idx = buscar_pilha(&pilhas[f], nomes_filmes[f]);
        if (idx == -1) printf("[!] Nenhum ingresso encontrado.\n");
        else printf("[OK] Ingresso encontrado na posicao %d da pilha.\n", idx);
    }
}

// ===================== IMPRIMIR ESTRUTURAS =====================
void menu_imprimir() {
    int op;
    printf("\nImprimir:\n"
           "  1 - Fila de compra (clientes)\n"
           "  2 - Fila de sala (exibicao)\n"
           "  3 - Pilha de ingressos\n"
           "  4 - Tudo\n"
           "Opcao: ");
    scanf("%d", &op);

    if (op == 1 || op == 4) {
        imprimir_fila(&fila_clientes, "FILA DE COMPRA");
    }
    if (op == 2 || op == 4) {
        for (int i = 0; i < NUM_FILMES; i++) {
            char titulo[60];
            sprintf(titulo, "FILA SALA %d - %s", salas_filmes[i], nomes_filmes[i]);
            imprimir_fila(&filas_sala[i], titulo);
        }
    }
    if (op == 3 || op == 4) {
        for (int i = 0; i < NUM_FILMES; i++) {
            char titulo[60];
            sprintf(titulo, "PILHA DE INGRESSOS - %s", nomes_filmes[i]);
            imprimir_pilha(&pilhas[i], titulo);
        }
    }
}

// ===================== MAIN =====================
int main() {
    // Inicializa estruturas
    for (int i = 0; i < NUM_FILMES; i++) {
        pilha_init(&pilhas[i]);
        fila_init(&filas_sala[i]);
    }
    fila_init(&fila_clientes);

    int opcao;
    do {
        printf("\n========================================\n");
        printf("       CINECAPI - Sistema de Ingressos  \n");
        printf("========================================\n");
        printf("1 - Inserir dados automaticamente\n");
        printf("2 - Realizar venda de ingresso (1 cliente)\n");
        printf("3 - Vender ingressos (ate esvaziar fila)\n");
        printf("4 - Buscar\n");
        printf("5 - Imprimir estruturas\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: preencher_automatico(); break;
            case 2: vender_ingresso();      break;
            case 3: vender_todos();         break;
            case 4: menu_busca();           break;
            case 5: menu_imprimir();        break;
            case 0: printf("\nSaindo...\n"); break;
            default: printf("[!] Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
