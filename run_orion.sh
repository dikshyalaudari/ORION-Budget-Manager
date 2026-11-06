#!/bin/bash

echo "🔧 Compiling Orion..."
gcc -o orion Orion.c

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful."
    echo "🚀 Launching Orion..."
    ./orion
else
    echo "❌ Compilation failed. Fix the errors and try again."
fi
