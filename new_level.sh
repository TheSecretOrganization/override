#!/bin/bash

set -e

if [[ -z $1 ]]; then
    echo "Usage: $0 <level_name>"
    exit 1
else
    level_name="./$1"
fi

mkdir -p "$level_name/Ressources"
touch "$level_name/flag" "$level_name/source.c" "$level_name/walkthrough.md"
