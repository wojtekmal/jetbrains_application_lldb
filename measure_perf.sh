#!/bin/bash

test_executable="test_programs/test_perf"
gwatch="build/bin/gwatch"
symbol="sum"

echo "Running native benchmark..."
native_time=$((time -p "$test_executable") 2>&1 | awk '/^real/ {print $2}' )
#time -p "$test_executable"
echo "Native time: ${native_time}s."

echo "Running watched benchmark..."
watched_time=$((time -p "$gwatch" --var "$symbol" --exec "$test_executable" -q) 2>&1 | awk '/^real/ {print $2}' )
#time -p "$gwatch" --var "$symbol" --exec "$test_executable" -q
echo "Watched time: ${watched_time}s."

echo "----------------------"
slowdown=$(echo "$watched_time / $native_time" | bc -l)
printf "Slowdown factor: %2fx.\n" "$slowdown"

if (( $(echo "$slowdown <= 2.0" | bc -l) ))
then
    echo "PASSED - within 2x performance requirement."
else
    echo "FAILED - not within 2x performance requirement."
fi