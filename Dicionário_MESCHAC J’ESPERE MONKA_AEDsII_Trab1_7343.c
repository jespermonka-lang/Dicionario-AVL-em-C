#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constantes
#define MAX_PALAVRA 50      //aqui é o Tamanho máximo para o campo palavra
#define MAX_SIGNIFICADO 200 //E aqui o Tamanho máximo para o campo significado

// Estrutura de dados (NÓ AVL)
typedef struct NoAVL {
    char palavra[MAX_PALAVRA];
    char significado[MAX_SIGNIFICADO];
    int altura;
    struct NoAVL *esquerda;
    struct NoAVL *direita;
    struct NoAVL *pai;
} NoAVL;


// as funçoẽs auxiliares


//para  Retornar o maior entre dois números
int maior(int a, int b) { return (a > b) ? a : b; }

//para Retornar a altura de um nó. Retorna -1 se o nó é NULL.
int altura_no(NoAVL *no) {
    return (no == NULL) ? -1 : no->altura;
}

//aqui é pra Calcular o Fator de Balanceamento (FB = Altura(Esq) - Altura(Dir))
int fator_balanceamento(NoAVL *no) {
    if (no == NULL) return 0;
    return altura_no(no->esquerda) - altura_no(no->direita);
}

//pra Atualizar a altura de um nó com base nas alturas dos seus filhos
void atualizar_altura(NoAVL *no) {
    no->altura = 1 + maior(altura_no(no->esquerda), altura_no(no->direita));
}

//para criar_arvore_vazia e inicializar um novo nó AVL
NoAVL *novo_no(const char *palavra, const char *significado) {
    NoAVL *novo = (NoAVL *)malloc(sizeof(NoAVL));
    if (novo == NULL) {
        fprintf(stderr, "Erro ao alocar novo no\n");
        exit(EXIT_FAILURE);
    }
    //para Copiar o conteúdo da palavra e tbm o significado
    strncpy(novo->palavra, palavra, MAX_PALAVRA - 1);
    novo->palavra[MAX_PALAVRA - 1] = '\0';
    strncpy(novo->significado, significado, MAX_SIGNIFICADO - 1);
    novo->significado[MAX_SIGNIFICADO - 1] = '\0';
    novo->altura = 0;
    // a altura inicial é 0 para um nó folha
    novo->esquerda = novo->direita = novo->pai = NULL;
    return novo;
}


// As rotaçoes AVL 


// (Caso LL)
NoAVL *rotacao_direita(NoAVL *z) {
    NoAVL *y = z->esquerda;
    // y vai ser a nova raiz
    NoAVL *T3 = y->direita;
    // T3 é o filho de y que vai ser realocado

    //aqui vai Executar a rotação
    y->direita = z;
    z->esquerda = T3;

    // Atualizar os ponteiros pai
    if (T3) T3->pai = z;
    y->pai = z->pai;
    z->pai = y;

    //E atualizar as alturas (de baixo para cima)
    atualizar_altura(z);
    atualizar_altura(y);
    return y;
    // Retorna a nova raiz
}

// (Caso RR)
NoAVL *rotacao_esquerda(NoAVL *z) {
    NoAVL *y = z->direita;
    // y será a nova raiz
    NoAVL *T2 = y->esquerda;
    // T2 é o filho de y que vai ser realocado

    // Executar a rotação
    y->esquerda = z;
    z->direita = T2;

    // Atualizar os ponteiros pai
    if (T2) T2->pai = z;
    y->pai = z->pai;
    z->pai = y;

    // Atualizar as alturas
    atualizar_altura(z);
    atualizar_altura(y);
    return y;
    // Retorna a nova raiz
}

// Rotação Dupla Esquerda-Direita (Caso LR)
NoAVL *rotacao_esquerda_direita(NoAVL *z) {
    z->esquerda = rotacao_esquerda(z->esquerda);
    // Primeiro vai fazer rotação à esquerda no filho esquerdo
    return rotacao_direita(z);
    // E depois a rotação à direita na raiz
}

// Rotação Dupla Direita-Esquerda (Caso RL)
NoAVL *rotacao_direita_esquerda(NoAVL *z) {
    z->direita = rotacao_direita(z->direita);
    // Primeiro a rotação à direita no filho direito
    return rotacao_esquerda(z);
    // e after a rotação à esquerda na raiz
}


// REBALANCEAMENTO 


//Aqui vai verificar o fator de balanceamento e executar as rotações necessárias
NoAVL *rebalancear(NoAVL *no) {
    if (no == NULL) return NULL;
    atualizar_altura(no); // Recalcular a altura

    int fb = fator_balanceamento(no);
    // Calcula o FB

    // Desbalanceado à Esquerda (por acaso quando FB > 1)
    if (fb > 1) {
        if (fator_balanceamento(no->esquerda) >= 0)
            return rotacao_direita(no);
        // Caso LL
        else
            return rotacao_esquerda_direita(no);
        // Caso LR
    }

    // Desbalanceado à Direita (quando FB < -1)
    if (fb < -1) {
        if (fator_balanceamento(no->direita) <= 0)
            return rotacao_esquerda(no);
        // Caso RR
        else
            return rotacao_direita_esquerda(no);
        // Caso RL
    }

    return no;
    // Já está balanceado
}


// As operações principais


// 1. Criar a árvore vazia
NoAVL *criar_arvore_vazia() {
    printf("Árvore criada sucesso\n");
    return NULL; // A raiz é inicializada como NULL.
}

// 3. para inserir uma palavra na árvore AVL
NoAVL *inserir_palavra(NoAVL *raiz, const char *palavra, const char *significado, int *sucesso) {
    if (raiz == NULL) {
        *sucesso = 1; // Marca sucesso na inserção.
        printf("Inserção de %s com sucesso\n", palavra);
        return novo_no(palavra, significado); // Cria e retorna o novo nó folha.
    }

    int cmp = strcmp(palavra, raiz->palavra); // Compara a palavra para decidir a direção.
    if (cmp < 0) {
        NoAVL *nova = inserir_palavra(raiz->esquerda, palavra, significado, sucesso); // Desce recursivamente à esquerda.
        if (nova != raiz->esquerda) { // Se o ponteiro mudou (após recursão/rotação)
            raiz->esquerda = nova;
            if (nova) nova->pai = raiz; // Define o ponteiro pai para o novo filho.
        }
    } else if (cmp > 0) {
        NoAVL *nova = inserir_palavra(raiz->direita, palavra, significado, sucesso); // Desce recursivamente à direita.
        if (nova != raiz->direita) { // Se o ponteiro mudou (após recursão/rotação)
            raiz->direita = nova;
            if (nova) nova->pai = raiz; // Define o ponteiro pai para o novo filho.
        }
    } else {
        *sucesso = 0; // Palavra duplicada.
        printf("Operação de Inserção de %s sem sucesso. Palavra já existe no dicionário\n", palavra);
        return raiz; // Retorna o nó atual sem alteração.
    }

    return rebalancear(raiz); // Rebalanceia no retorno da recursão
}

// 4. Busca uma palavra na árvore
NoAVL *buscar_palavra(NoAVL *raiz, const char *palavra) {
    if (raiz == NULL) return NULL; // Retorna NULL se não for encontrada.
    int cmp = strcmp(palavra, raiz->palavra); //vai Comparar o termo de busca.
    if (cmp == 0) return raiz; // Palavra encontrada.
    if (cmp < 0) return buscar_palavra(raiz->esquerda, palavra); // Continua a busca na subárvore esquerda.
    return buscar_palavra(raiz->direita, palavra); // Continua a busca na subárvore direita.
}

// Encontra o nó de menor valor na subárvore (usado para remoção)
NoAVL *no_minimo(NoAVL *no) {
    while (no && no->esquerda) no = no->esquerda; // Itera para o filho mais à esquerda.
    return no; // Retorna o nó com o menor valor.
}

// 2. Remove uma palavra da árvore AVL
NoAVL *remover_palavra(NoAVL *raiz, const char *palavra, int *sucesso) {
    char palavra_original[MAX_PALAVRA];
    if (raiz) strcpy(palavra_original, palavra); // Salve a palavra original para impressão.
    
    if (raiz == NULL) {
        *sucesso = 0;
        printf("Operação de remoção inválida\n");
        return raiz;
    }

    int cmp = strcmp(palavra, raiz->palavra);
    if (cmp < 0) {
        raiz->esquerda = remover_palavra(raiz->esquerda, palavra, sucesso); // Desce à esquerda.
        if (raiz->esquerda) raiz->esquerda->pai = raiz; // Atualiza o ponteiro pai após a recursão.
    } else if (cmp > 0) {
        raiz->direita = remover_palavra(raiz->direita, palavra, sucesso); // Desce à direita.
        if (raiz->direita) raiz->direita->pai = raiz; // Atualiza o ponteiro pai após a recursão.
    } else {
        // Nó encontrado (raiz->palavra = palavra)

        if (raiz->esquerda == NULL || raiz->direita == NULL) {
            // Caso 0 ou 1 filho
            NoAVL *temp = raiz->esquerda ? raiz->esquerda : raiz->direita; // Pega o único filho ou NULL.
            
            if (temp == NULL) { // 0 filho (folha)
                temp = raiz; 
                raiz = NULL; // O nó folha é removido, a raiz do sub-árvore vira NULL.
            } else { // 1 filho: copia o conteúdo do filho
                *raiz = *temp; // O conteúdo do filho substitui o conteúdo do pai.
            }

            if (*sucesso == 1) { 
                printf("%s removido com sucesso\n", palavra_original); 
            }

            free(temp); // Libera o nó que foi substituído ou o nó folha original.
            *sucesso = 1; // Marca sucesso.
            
            if (raiz == NULL) return raiz;
            
            // Corrije ponteiros pai
            if (raiz->esquerda) raiz->esquerda->pai = raiz; 
            if (raiz->direita) raiz->direita->pai = raiz;

        } else {
            // Caso 2 filhos: usa o sucessor in-order
            NoAVL *temp = no_minimo(raiz->direita); // Encontra o sucessor (menor valor à direita).
            
            printf("%s removido com %s\n", palavra_original, temp->palavra); 
            
            // Copia os dados do sucessor para o nó atual
            strcpy(raiz->palavra, temp->palavra);
            strcpy(raiz->significado, temp->significado);
            
            // Remove o sucessor da subárvore direita.
            int temp_sucesso = *sucesso;
            *sucesso = 0; // Limpa o sucesso para evitar mensagem dupla no caso de remoção do sucessor (0/1 filho).
            raiz->direita = remover_palavra(raiz->direita, temp->palavra, sucesso);
            *sucesso = temp_sucesso; // Restaura o flag principal.
            
            if (raiz->direita) raiz->direita->pai = raiz;
            *sucesso = 1; 
        }
    }
    
    if (raiz == NULL) return raiz; // Nada para rebalancear se o nó foi removido e a raiz ficou NULL.
    return rebalancear(raiz); // Rebalanceia no retorno
}

// 5. Imprim a árvore em ordem alfabética
void imprimir_em_ordem(NoAVL *raiz) {
    if (raiz) {
        imprimir_em_ordem(raiz->esquerda); // Visita a subárvore esquerda (menores).
        printf("%s h=%d ", raiz->palavra, raiz->altura); // Imprime o nó atual.
        imprimir_em_ordem(raiz->direita); // Visita a subárvore direita (maiores).
    }
}

// pra liberar toda a memória alocada pela árvore (depois da ordenação)
void destruir_arvore(NoAVL *raiz) {
    if (raiz) {
        destruir_arvore(raiz->esquerda); // Recursivamente, libera a subárvore esquerda.
        destruir_arvore(raiz->direita); // Recursivamente, libera a subárvore direita.
        free(raiz); // Libera o nó atual.
    }
}


// PROGRAMA PRINCIPAL


int main() {
    // A leitura diretamente de STDIN (entrada padrão).
    NoAVL *raiz = NULL; // Inicializa a raiz da árvore.
    int comando, sucesso; // 'comando' para a operação, 'sucesso' para checagem interna.
    char palavra[MAX_PALAVRA], significado[MAX_SIGNIFICADO]; // um Buffers para a leitura dos dados.
    // Loop que lê os comandos (do números 1 a 5) até o fim do arquivo/entrada
    while (scanf("%d", &comando) == 1) { // Continua lendo comandos enquanto o 'scanf' for ok (bem-sucedido).
        switch (comando) {
            case 1:
                raiz = criar_arvore_vazia(); // Inicializa a árvore.
                break;
            case 2:
                scanf("%s", palavra); // Lê a palavra a ser removida.
                // NOTA: O tratamento das mensagens de sucesso/falha é feito DENTRO da função remover_palavra.
                raiz = remover_palavra(raiz, palavra, &sucesso); 
                break;
            case 3:
                scanf("%s", palavra); // Lê a palavra a ser inserida.
                getchar(); // Consome a quebra de linha após o scanf da palavra
                fgets(significado, MAX_SIGNIFICADO, stdin); // Ler o significado (peut contenir des espaços)
                significado[strcspn(significado, "\n")] = '\0'; // Remove o '\n' final do fgets
                raiz = inserir_palavra(raiz, palavra, significado, &sucesso);
                break;
            case 4: {
                scanf("%s", palavra); // Lê a palavra a ser buscada.
                NoAVL *res = buscar_palavra(raiz, palavra);
                if (res) {

                    printf("%s encontrado no dicionário h=%d\n", res->palavra, res->altura);
                    printf("%s: %s\n", res->palavra, res->significado); // Imprime a palavra e seu significado.
                } else {
                    printf("%s não encontrado no dicionário\n", palavra);
                }
                break;
            }
            case 5:
                imprimir_em_ordem(raiz); // Inicia o print em ordem
                printf("\n"); 
                break;
            default:
                // Comando desconhecido ou final: destroy a árvore e sai
                destruir_arvore(raiz); // Libera a memória antes de encerrar.
                return 0;
        }
    }

    // pra garantir de que a memória seja liberada ao final da execução (caso o loop termine por end of file ,tipo quando a ultima linha do arquivo vai ser lida e q nao vai mais ter nada a ser lida).
    destruir_arvore(raiz);
    return 0;
}