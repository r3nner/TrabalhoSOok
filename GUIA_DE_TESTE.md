# Guia de Teste - Simulador de Memória Virtual (T3)

## Como Testar o Programa

### 1. Compilar o Projeto

```bash
cd /home/pao/Área\ de\ Trabalho/SO2/TrabalhoSOok
make clean
make
```

Se tudo compilar sem erros, você está pronto para testar.

### 2. Executar o Simulador Interativo

```bash
./main
```

O simulador abrirá uma interface interativa (com curses/ncurses) mostrando:
- Estado da CPU (registradores, PC, etc)
- Memória principal e secundária
- Processos em execução
- Estatísticas de falhas de página
- Mensagens de log do SO

**Controles:**
- `n` ou ENTER: próxima instrução
- `c`: executar até próxima interrupção
- `r`: reiniciar
- `q`: sair

### 3. Executar Testes Automatizados (Versão Simplificada)

```bash
./test_memory_experiments.sh
```

Este script:
- Compila o projeto
- Executa 3 testes com durações diferentes
- Captura logs em `results/test*.log`
- Extrai dados de desempenho (falhas de página, etc)

### 4. Analisar Resultados

Os logs estão em `results/`:

```bash
# Ver log do primeiro teste
cat results/test1_quick.log | grep "page fault\|bloqueado"

# Ver resumo de desempenho (se implementado)
grep -A 20 "RESUMO DE DESEMPENHO" results/test*.log
```

### 5. Modificar Configurações para Experimentar

Para alterar o tamanho da memória ou das páginas:

**Edite `so.h` ou `dispositivos.h`:**

```c
// Tamanho de uma página (em palavras)
#define TAM_PAGINA 8    // Altere para testar diferentes tamanhos

// Tamanho da memória principal (em palavras)
// Procure por constantes de memória e altere conforme necessário
```

Depois recompile:
```bash
make clean
make
```

### 6. Verificar Estatísticas de Desempenho

O SO imprime automaticamente durante a execução:
- Número de falhas de página por processo
- Tempo de bloqueio por transferência de página
- Informações sobre swap in/out

Procure por logs com:
```bash
grep "page fault\|bloqueado\|swap" results/*.log
```

### 7. Estrutura dos Programas Testados

O simulador carrega e executa:
- **init.maq** - Programa inicial (PID 1)
- **ex1.maq a ex6.maq** - Programas de exemplo com diferentes padrões de acesso
- **p1.maq a p3.maq** - Programas de teste adicionais

Cada programa testa comportamentos diferentes do sistema de memória virtual:
- Acesso sequencial
- Acesso aleatório
- Padrões de localidade

### 8. Rodar um Teste Mais Completo (Manual)

Para coletar dados de um experimento específico:

```bash
# Teste 1: Memória cheia (sem page faults esperados)
make clean
make
timeout 20 ./main 2>&1 | tee full_memory.log

# Teste 2: Meia memória (vários page faults)
# (Altere TAM_PAGINA ou constante de memória em dispositivos.h)
make clean
make
timeout 20 ./main 2>&1 | tee half_memory.log

# Teste 3: Memória mínima (muitos page faults)
# (Reduza ainda mais)
make clean
make
timeout 20 ./main 2>&1 | tee minimal_memory.log
```

### 9. Gerar Relatório de Análise

Crie um script Python para processar os dados:

```bash
# Extrair dados de falhas de página
grep "page fault" full_memory.log | wc -l
grep "page fault" half_memory.log | wc -l
grep "page fault" minimal_memory.log | wc -l
```

Compare e documente as diferenças em um arquivo `RELATORIO.md`.

### 10. Checklist de Testes

- [ ] Compilação sem erros
- [ ] Execução interativa funciona (pode navegar com n, c, q)
- [ ] Testes automatizados completam sem travar
- [ ] Logs mostram falhas de página sendo tratadas
- [ ] Pode-se coletar dados variando tamanho de memória
- [ ] Pode-se coletar dados variando tamanho de página
- [ ] Dados mostram diferenças esperadas entre configurações

## Arquivo de Log Esperado

Um log típico deve conter:

```
SO: recebi IRQ 1 (TEMPO)
SO: Falta de página no endereco virtual 512 (PID 1)
SO: page fault tratado PID 1 pagina 64 -> quadro 100 (total 1), transfers=1, bloqueado_ate=150
SO: desbloqueando PID 1 (agora=150)
...
==== RESUMO DE DESEMPENHO ====
PID 1: Falhas de página = 245, Último bloqueio = 5000
============================
```

## Dúvidas/Problemas

- **Simulador travando:** Use `timeout` para interromper
- **Memória insuficiente para compilar:** Limpe com `make clean`
- **Dados não aparecem:** Procure por "page fault" ou "RESUMO" nos logs
- **Quer desabilitar/abilitar LRU:** Altere `self->usar_lru` em `so_cria()`

---

**Próximo passo:** Usar este guia para coletar dados de diferentes configurações e gerar o relatório final com análises de desempenho!
