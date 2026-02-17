#!/bin/bash

EXEC=./knapsack
TESTS_DIR=./tests_knapsack

echo "Running Knapsack tests..."

# Создание директории для тестов
rm -rf $TESTS_DIR
mkdir -p $TESTS_DIR

# ---------------------------------------------------------
# Test 1: Все предметы влезают
# Ожидается: Max Cost: 15 (5+4+6)
# ---------------------------------------------------------
echo "Test 1: All items fit"
cat > $TESTS_DIR/test1.txt <<EOF
3 20
2 5
3 4
5 6
EOF

$EXEC $TESTS_DIR/test1.txt > $TESTS_DIR/out1.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 15" $TESTS_DIR/out1.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: All items fit test failed"
    cat $TESTS_DIR/out1.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 2: Точное совпадение веса (граничный случай)
# Лимит 10, два предмета по 5. Сумма стоимостей 20.
# ---------------------------------------------------------
echo "Test 2: Exact weight limit"
cat > $TESTS_DIR/test2.txt <<EOF
2 10
5 10
5 10
EOF

$EXEC $TESTS_DIR/test2.txt > $TESTS_DIR/out2.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 20" $TESTS_DIR/out2.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Exact weight limit test failed"
    cat $TESTS_DIR/out2.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 3: Ни один предмет не влезает
# Лимит 1, предметы весом 2 и 3.
# ---------------------------------------------------------
echo "Test 3: Nothing fits"
cat > $TESTS_DIR/test3.txt <<EOF
2 1
2 10
3 20
EOF

$EXEC $TESTS_DIR/test3.txt > $TESTS_DIR/out3.txt 2>&1
if [ $? -eq 0 ] && grep -q "No valid combination found" $TESTS_DIR/out3.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Nothing fits test failed"
    cat $TESTS_DIR/out3.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 4: Выбор более дорогого предмета
# Лимит 5. Предметы: (2, 10) и (5, 100).
# Нельзя взять оба (вес 7). Должен выбрать второй (стоимость 100).
# ---------------------------------------------------------
echo "Test 4: Choose most expensive valid combination"
cat > $TESTS_DIR/test4.txt <<EOF
2 5
2 10
5 100
EOF

$EXEC $TESTS_DIR/test4.txt > $TESTS_DIR/out4.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 100" $TESTS_DIR/out4.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Logic test (max cost selection) failed"
    cat $TESTS_DIR/out4.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 5: Один предмет, который влезает
# ---------------------------------------------------------
echo "Test 5: Single item fits"
cat > $TESTS_DIR/test5.txt <<EOF
1 10
5 100
EOF

$EXEC $TESTS_DIR/test5.txt > $TESTS_DIR/out5.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 100" $TESTS_DIR/out5.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Single item fit test failed"
    cat $TESTS_DIR/out5.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 6: Один предмет, который НЕ влезает
# ---------------------------------------------------------
echo "Test 6: Single item too heavy"
cat > $TESTS_DIR/test6.txt <<EOF
1 5
10 100
EOF

$EXEC $TESTS_DIR/test6.txt > $TESTS_DIR/out6.txt 2>&1
if [ $? -eq 0 ] && grep -q "No valid combination found" $TESTS_DIR/out6.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Single item too heavy test failed"
    cat $TESTS_DIR/out6.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 7: Пустой список предметов (n=0)
# ---------------------------------------------------------
echo "Test 7: Zero items"
cat > $TESTS_DIR/test7.txt <<EOF
0 100
EOF

$EXEC $TESTS_DIR/test7.txt > $TESTS_DIR/out7.txt 2>&1
if [ $? -eq 0 ] && grep -q "No items to process" $TESTS_DIR/out7.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Zero items test failed"
    cat $TESTS_DIR/out7.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 8: Стресс-тест (12 предметов)
# Проверка, что перебор 2^12 работает корректно.
# ---------------------------------------------------------
echo "Test 8: Stress test (12 items)"
echo "12 100" > $TESTS_DIR/test8.txt
for i in {1..12}; do echo "1 1" >> $TESTS_DIR/test8.txt; done

$EXEC $TESTS_DIR/test8.txt > $TESTS_DIR/out8.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 12" $TESTS_DIR/out8.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Stress test failed"
    cat $TESTS_DIR/out8.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 9: Ошибка - файл не найден
# ---------------------------------------------------------
echo "Test 9: File not found"
$EXEC ./nonexistent_file.txt > $TESTS_DIR/out9.txt 2>&1
if [ $? -ne 0 ] && grep -q "Cannot open file" $TESTS_DIR/out9.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: File not found test failed"
    cat $TESTS_DIR/out9.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 10: Ошибка - некорректный заголовок
# ---------------------------------------------------------
echo "Test 10: Invalid header format"
echo "abc 100" > $TESTS_DIR/test10.txt

$EXEC $TESTS_DIR/test10.txt > $TESTS_DIR/out10.txt 2>&1
if [ $? -ne 0 ] && grep -q "Invalid header" $TESTS_DIR/out10.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Invalid header test failed"
    cat $TESTS_DIR/out10.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 11: Ошибка - отрицательный вес
# ---------------------------------------------------------
echo "Test 11: Negative weight"
cat > $TESTS_DIR/test11.txt <<EOF
1 10
-5 100
EOF

$EXEC $TESTS_DIR/test11.txt > $TESTS_DIR/out11.txt 2>&1
if [ $? -ne 0 ] && grep -q "non-positive" $TESTS_DIR/out11.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Negative weight test failed"
    cat $TESTS_DIR/out11.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 12: Ошибка - недостаточно данных в файле
# ---------------------------------------------------------
echo "Test 12: Missing item data"
cat > $TESTS_DIR/test12.txt <<EOF
2 10
5 10
EOF

$EXEC $TESTS_DIR/test12.txt > $TESTS_DIR/out12.txt 2>&1
if [ $? -ne 0 ] && grep -q "Failed to read item" $TESTS_DIR/out12.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Missing data test failed"
    cat $TESTS_DIR/out12.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 13: Предупреждение о лишних данных (успех + warning)
# ---------------------------------------------------------
echo "Test 13: Warning on extra data"
cat > $TESTS_DIR/test13.txt <<EOF
1 10
5 100
99 99
EOF

$EXEC $TESTS_DIR/test13.txt > $TESTS_DIR/out13.txt 2>&1
# Код возврата должен быть 0, но в выводе должно быть предупреждение
if [ $? -eq 0 ] && grep -q "Warning: Extra data" $TESTS_DIR/out13.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: Extra data warning test failed"
    cat $TESTS_DIR/out13.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 14: Отсутствие аргументов
# ---------------------------------------------------------
echo "Test 14: No arguments provided"
$EXEC > $TESTS_DIR/out14.txt 2>&1
if [ $? -ne 0 ] && grep -q "Expected exactly one argument" $TESTS_DIR/out14.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: No arguments test failed"
    cat $TESTS_DIR/out14.txt
    exit 1
fi

# ---------------------------------------------------------
# Test 15: Help опция
# ---------------------------------------------------------
echo "Test 15: Help option"
if $EXEC -h > /dev/null 2>&1; then
    echo "[PASS]"
else
    echo "[FAIL]: Help option failed"
    exit 1
fi

# ---------------------------------------------------------
# Test 16: Оптимальный выбор
# Ожидается: Max Cost: 20
# ---------------------------------------------------------
echo "Test 16: Have to fit better cost"
cat > $TESTS_DIR/test16.txt <<EOF
4 20
20 15
10 14
10 2
10 6
EOF

$EXEC $TESTS_DIR/test16.txt > $TESTS_DIR/out16.txt 2>&1
if [ $? -eq 0 ] && grep -q "Max Cost: 20" $TESTS_DIR/out16.txt; then
    echo "[PASS]"
else
    echo "[FAIL]: All items fit test failed"
    cat $TESTS_DIR/out16.txt
    exit 1
fi

echo "All tests passed!"
rm -rf $TESTS_DIR
exit 0