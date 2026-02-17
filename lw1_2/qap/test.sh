#!/bin/bash

EXEC=./qap

echo "QAP tests is running"

for testfile in tests/*.in; do
    echo "Test: $testfile"
    $EXEC "$testfile"
    echo "---------------------------"
done

echo "All tests passed!"