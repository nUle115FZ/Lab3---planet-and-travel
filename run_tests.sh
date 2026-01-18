#!/bin/bash

# Скрипт для быстрого запуска тестов

cd "$(dirname "$0")"

echo "🧪 Запуск unit-тестов для StarGraph..."
echo ""

cd tests
make clean > /dev/null 2>&1
make

if [ $? -eq 0 ]; then
    echo ""
    ./test_graph
    exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo ""
        echo "✨ Все тесты успешно пройдены!"
    else
        echo ""
        echo "⚠️  Некоторые тесты провалились!"
    fi
    
    exit $exit_code
else
    echo "❌ Ошибка компиляции тестов!"
    exit 1
fi

