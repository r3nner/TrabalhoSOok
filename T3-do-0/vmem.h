// vmem.h
// gerenciamento de estruturas de memória virtual
// simulador de computador
// so25b

#ifndef VMEM_H
#define VMEM_H

#include <stdbool.h>
#include <stddef.h>

#include "memoria.h"

// descreve um quadro físico da memória principal
typedef struct {
  bool livre;             // true se o quadro está disponível
  int dono_pid;           // pid do processo que ocupa o quadro, ou -1
  int pagina_virtual;     // índice da página virtual ocupante, ou -1
  unsigned long carimbo_fifo; // usado para FIFO/clock
  unsigned long idade;    // usado para envelhecimento/LRU aproximado
} quadro_desc_t;

// descreve uma página armazenada na memória secundária
typedef struct {
  bool ocupado;           // true se a página secundária está em uso
  int dono_pid;           // processo proprietário, ou -1
  int pagina_virtual;     // página correspondente, ou -1
  int base_endereco;      // endereço base na memória secundária
  int tamanho;            // tamanho da região em palavras
} pagina_sec_desc_t;

// estado global do gerenciador de memória virtual
typedef struct vm_estado_t vm_estado_t;

// cria um estado de memória virtual contendo 'num_quadros' quadros
//   físicos e 'num_paginas_sec' slots de páginas secundárias
vm_estado_t *vm_estado_cria(int num_quadros, int num_paginas_sec);

// destrói o estado e libera recursos associados
void vm_estado_destroi(vm_estado_t *estado);

// devolve o número de quadros físicos gerenciados
int vm_estado_num_quadros(const vm_estado_t *estado);

// devolve o número de páginas secundárias gerenciadas
int vm_estado_num_paginas_sec(const vm_estado_t *estado);

// obtém um ponteiro mutável para um descritor de quadro; retorna NULL se índice inválido
quadro_desc_t *vm_estado_quadro(vm_estado_t *estado, int indice);

// obtém um ponteiro mutável para um descritor de página secundária; retorna NULL se índice inválido
pagina_sec_desc_t *vm_estado_pagina_sec(vm_estado_t *estado, int indice);

// zera todos os descritores, marcando quadros e páginas como livres
void vm_estado_reseta(vm_estado_t *estado);

// encontra o índice de um quadro livre; retorna -1 se nenhum disponível
int vm_estado_busca_quadro_livre(vm_estado_t *estado);

// marca um quadro como ocupado pelo pid/página informados
void vm_estado_ocupa_quadro(vm_estado_t *estado, int indice, int pid, int pagina_virtual, unsigned long carimbo);

// libera um quadro ocupado, preservando carimbos para depuração
void vm_estado_libera_quadro(vm_estado_t *estado, int indice);

// encontra o índice de um slot livre na memória secundária
int vm_estado_busca_pagsec_livre(vm_estado_t *estado);

// ocupa um slot da secundária
void vm_estado_ocupa_pagsec(vm_estado_t *estado, int indice, int pid, int pagina_virtual, int base_endereco, int tamanho);

// libera um slot da secundária
void vm_estado_libera_pagsec(vm_estado_t *estado, int indice);

// configura o tamanho da memória secundária (em palavras) e cria o mem_t correspondente
void vm_estado_configura_mem_sec(vm_estado_t *estado, int tamanho);

// grava uma palavra na memória secundária
err_t vm_estado_sec_escreve(vm_estado_t *estado, int endereco, int valor);

// lê uma palavra da memória secundária
err_t vm_estado_sec_le(vm_estado_t *estado, int endereco, int *valor);

#endif // VMEM_H
