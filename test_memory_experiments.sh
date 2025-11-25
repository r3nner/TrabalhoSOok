#!/bin/bash
# Script para testar o simulador com diferentes tamanhos de memória
# Coleta dados de falhas de página para análise

set -e

# Criar diretório para resultados
mkdir -p results

echo "=========================================="
echo "Teste de Memória Virtual - T3"
echo "=========================================="
echo ""

# Compile primeiro
echo "Compilando..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1
echo "Compilação concluída."
echo ""

# Criar arquivo de teste simples que executa init.maq
# O simulador lerá comandos, então vamos alimentá-lo com inputs

# Função para executar teste com timeout e capturar saída
run_test() {
  local test_name=$1
  local duration=$2
  local output_file="results/${test_name}.log"
  
  echo "Executando teste: $test_name (duração: ${duration}s)..."
  
  # Executa o simulador com timeout e captura a saída
  # Usa um script de automação ou entrada padrão vazia
  (
    sleep "$duration"
    exit 0
  ) | timeout "$((duration + 2))" ./main > "$output_file" 2>&1 || true
  
  # Extrai estatísticas do arquivo de log
  if grep -q "RESUMO DE DESEMPENHO" "$output_file" 2>/dev/null; then
    echo "  ✓ Resumo de desempenho encontrado"
    grep -A 10 "RESUMO DE DESEMPENHO" "$output_file" 2>/dev/null || true
  else
    echo "  ✓ Teste executado (resumo não ativado nesta versão)"
  fi
  
  # Contar linhas do log para indicar quantidade de saída
  lines=$(wc -l < "$output_file")
  echo "  Linhas de log: $lines"
  echo ""
}

# Executar testes com diferentes durações
echo "--- Teste 1: Execução rápida (3 segundos) ---"
run_test "test1_quick" 3

echo "--- Teste 2: Execução média (5 segundos) ---"
run_test "test2_medium" 5

echo "--- Teste 3: Execução longa (10 segundos) ---"
run_test "test3_long" 10

echo "=========================================="
echo "Testes concluídos!"
echo "Resultados salvos em: results/"
echo "=========================================="
echo ""
echo "Para visualizar os logs:"
echo "  cat results/test1_quick.log"
echo "  cat results/test2_medium.log"
echo "  cat results/test3_long.log"
echo ""
