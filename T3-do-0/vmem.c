// vmem.c
// gerenciamento de estruturas de memória virtual
// simulador de computador
// so25b

#include "vmem.h"
#include "err.h"

#include <stdlib.h>
#include <assert.h>

struct vm_estado_t {
  int num_quadros;
  quadro_desc_t *quadros;
  int num_paginas_sec;
  pagina_sec_desc_t *paginas_sec;
  mem_t *mem_secundaria;
  int tam_mem_sec;
};

static void inicializa_quadros(vm_estado_t *estado)
{
  for (int i = 0; i < estado->num_quadros; i++) {
    quadro_desc_t *q = &estado->quadros[i];
    q->livre = true;
    q->dono_pid = -1;
    q->pagina_virtual = -1;
    q->carimbo_fifo = 0;
    q->idade = 0;
  }
}

static void inicializa_paginas(vm_estado_t *estado)
{
  for (int i = 0; i < estado->num_paginas_sec; i++) {
    pagina_sec_desc_t *p = &estado->paginas_sec[i];
    p->ocupado = false;
    p->dono_pid = -1;
    p->pagina_virtual = -1;
    p->base_endereco = -1;
    p->tamanho = 0;
  }
}

vm_estado_t *vm_estado_cria(int num_quadros, int num_paginas_sec)
{
  vm_estado_t *estado = malloc(sizeof(*estado));
  assert(estado != NULL);

  estado->num_quadros = num_quadros;
  estado->num_paginas_sec = num_paginas_sec;
  estado->mem_secundaria = NULL;
  estado->tam_mem_sec = 0;

  if (num_quadros > 0) {
    estado->quadros = malloc(sizeof(*estado->quadros) * num_quadros);
    assert(estado->quadros != NULL);
  } else {
    estado->quadros = NULL;
  }

  if (num_paginas_sec > 0) {
    estado->paginas_sec = malloc(sizeof(*estado->paginas_sec) * num_paginas_sec);
    assert(estado->paginas_sec != NULL);
  } else {
    estado->paginas_sec = NULL;
  }

  vm_estado_reseta(estado);
  return estado;
}

void vm_estado_destroi(vm_estado_t *estado)
{
  if (estado == NULL) {
    return;
  }
  if (estado->mem_secundaria != NULL) {
    mem_destroi(estado->mem_secundaria);
  }
  free(estado->quadros);
  free(estado->paginas_sec);
  free(estado);
}

int vm_estado_num_quadros(const vm_estado_t *estado)
{
  return estado != NULL ? estado->num_quadros : 0;
}

int vm_estado_num_paginas_sec(const vm_estado_t *estado)
{
  return estado != NULL ? estado->num_paginas_sec : 0;
}

quadro_desc_t *vm_estado_quadro(vm_estado_t *estado, int indice)
{
  if (estado == NULL || indice < 0 || indice >= estado->num_quadros) {
    return NULL;
  }
  return &estado->quadros[indice];
}

pagina_sec_desc_t *vm_estado_pagina_sec(vm_estado_t *estado, int indice)
{
  if (estado == NULL || indice < 0 || indice >= estado->num_paginas_sec) {
    return NULL;
  }
  return &estado->paginas_sec[indice];
}

void vm_estado_reseta(vm_estado_t *estado)
{
  if (estado == NULL) {
    return;
  }
  if (estado->quadros != NULL) {
    inicializa_quadros(estado);
  }
  if (estado->paginas_sec != NULL) {
    inicializa_paginas(estado);
  }
}

int vm_estado_busca_quadro_livre(vm_estado_t *estado)
{
  if (estado == NULL) {
    return -1;
  }
  for (int i = 0; i < estado->num_quadros; i++) {
    if (estado->quadros[i].livre) {
      return i;
    }
  }
  return -1;
}

void vm_estado_ocupa_quadro(vm_estado_t *estado, int indice, int pid, int pagina_virtual, unsigned long carimbo)
{
  quadro_desc_t *quadro = vm_estado_quadro(estado, indice);
  if (quadro == NULL) {
    return;
  }
  quadro->livre = false;
  quadro->dono_pid = pid;
  quadro->pagina_virtual = pagina_virtual;
  quadro->carimbo_fifo = carimbo;
  quadro->idade = 0;
}

void vm_estado_libera_quadro(vm_estado_t *estado, int indice)
{
  quadro_desc_t *quadro = vm_estado_quadro(estado, indice);
  if (quadro == NULL) {
    return;
  }
  quadro->livre = true;
  quadro->dono_pid = -1;
  quadro->pagina_virtual = -1;
}

int vm_estado_busca_pagsec_livre(vm_estado_t *estado)
{
  if (estado == NULL) {
    return -1;
  }
  for (int i = 0; i < estado->num_paginas_sec; i++) {
    if (!estado->paginas_sec[i].ocupado) {
      return i;
    }
  }
  return -1;
}

void vm_estado_ocupa_pagsec(vm_estado_t *estado, int indice, int pid, int pagina_virtual, int base_endereco, int tamanho)
{
  pagina_sec_desc_t *pagina = vm_estado_pagina_sec(estado, indice);
  if (pagina == NULL) {
    return;
  }
  pagina->ocupado = true;
  pagina->dono_pid = pid;
  pagina->pagina_virtual = pagina_virtual;
  pagina->base_endereco = base_endereco;
  pagina->tamanho = tamanho;
}

void vm_estado_libera_pagsec(vm_estado_t *estado, int indice)
{
  pagina_sec_desc_t *pagina = vm_estado_pagina_sec(estado, indice);
  if (pagina == NULL) {
    return;
  }
  pagina->ocupado = false;
  pagina->dono_pid = -1;
  pagina->pagina_virtual = -1;
  pagina->base_endereco = -1;
  pagina->tamanho = 0;
}

void vm_estado_configura_mem_sec(vm_estado_t *estado, int tamanho)
{
  if (estado == NULL) {
    return;
  }
  if (estado->mem_secundaria != NULL) {
    mem_destroi(estado->mem_secundaria);
    estado->mem_secundaria = NULL;
  }
  if (tamanho <= 0) {
    estado->tam_mem_sec = 0;
    return;
  }
  estado->mem_secundaria = mem_cria(tamanho);
  estado->tam_mem_sec = tamanho;
}

err_t vm_estado_sec_escreve(vm_estado_t *estado, int endereco, int valor)
{
  if (estado == NULL || estado->mem_secundaria == NULL) {
    return ERR_OP_INV;
  }
  return mem_escreve(estado->mem_secundaria, endereco, valor);
}

err_t vm_estado_sec_le(vm_estado_t *estado, int endereco, int *valor)
{
  if (estado == NULL || estado->mem_secundaria == NULL) {
    return ERR_OP_INV;
  }
  return mem_le(estado->mem_secundaria, endereco, valor);
}
