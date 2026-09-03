#!/bin/bash

# Rimuove tutti i modelli da Ollama

echo "Modelli installati:"
ollama list

echo ""
read -p "Sicuro di voler rimuovere TUTTI i modelli? (s/n): " conferma
if [[ "$conferma" != "s" ]]; then
    echo "Annullato."
    exit 0
fi

ollama list | tail -n +2 | awk '{print $1}' | while read -r model; do
    [ -z "$model" ] && continue
    echo "Rimozione: $model"
    ollama rm "$model"
done

echo ""
echo "Fatto. Modelli rimasti:"
ollama list
