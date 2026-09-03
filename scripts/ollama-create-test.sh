#!/bin/bash

set -u

MODELS=(
  "qwen2.5:7b"
  "qwen3:4b"
  "qwen3.5:4b"
  "qwen3.5:9b"
  "gemma4:e4b"
  "llama3.2:3b"
  "phi4-mini:3.8b"
  "ministral-3:3b"
  "ministral-3:8b"
  "granite4.2:8b"

)

CTX_SIZES=(16384 32768)

TMPFILE=$(mktemp)
trap 'rm -f "$TMPFILE"' EXIT

for model in "${MODELS[@]}"; do
  echo ">>> Pull di $model"
  if ! ollama pull "$model"; then
    echo "!! Impossibile scaricare $model, salto"
    continue
  fi

  for ctx in "${CTX_SIZES[@]}"; do
    name="${model}-${ctx/000/k}"   # es. qwen2.5:7b-16k
    echo ">>> Creo variante $name (num_ctx=$ctx)"
    cat > "$TMPFILE" <<EOF
FROM $model
PARAMETER num_ctx $ctx
EOF
    ollama create "$name" -f "$TMPFILE" || echo "!! Errore creando $name"
  done
done

echo ""
echo "=== Modelli installati ==="
ollama list
