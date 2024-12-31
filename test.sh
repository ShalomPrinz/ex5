#!/bin/bash

# Compile the C program
gcc ex5.c -o program -lm -g -std=c99 -DNDEBUG

# Function to run a single test case
run_test_case() {
    local input="$1"
    local expected_output="$2"
    local test_name="$3"

    # Run with valgrind
    actual_output=$(valgrind --leak-check=full --error-exitcode=1 ./program 2>&1 <<< "$input")

    # Check exit status
    if [ $? -ne 0 ]; then
        echo "Valgrind errors in test: $test_name"
        echo "$actual_output"
        return 1
    fi

    # Run the program with input and capture output
    actual_output=$(echo "$input" | ./program | tr -d '\r')
    actual_output="${actual_output}
"

    # Create temporary files for comparison
    echo "$actual_output" > actual_temp.txt
    echo "$expected_output" > expected_temp.txt


    if diff -Z <(expand -t 8 actual_temp.txt) expected_temp.txt > /dev/null; then
        echo "PASS: Test $test_name"
        rm actual_temp.txt expected_temp.txt
        return 0
    else
        echo "FAIL: Test $test_name"
        echo "--- Differences ---"
        diff -Z -U 2 <(expand -t 8 actual_temp.txt) expected_temp.txt
        rm actual_temp.txt expected_temp.txt
        return 1
    fi
}

# Initialize test counters
total_tests=0
passed_tests=0

# Parse test cases
current_name=""
current_input=""
current_output=""
is_parsing_output=0
is_parsing_input=0
output_buffer=""

# Process the test cases file
while IFS= read -r line; do
    # Remove leading/trailing whitespace
    trimmed_line=$(echo "$line" | xargs)

    # Start of a new test case
    if [[ "$trimmed_line" =~ ^-\ name: ]]; then
        # Run previous test case if exists
        if [[ -n "$current_name" && -n "$current_input" ]]; then
            ((total_tests++))
            if run_test_case "$current_input" "${current_output%?}" "$current_name"; then
                ((passed_tests++))
            fi

            # Reset for next test
            current_input=""
            current_output=""
            is_parsing_output=0
        fi

        # Extract new test name
        current_name=$(echo "$trimmed_line" | cut -d: -f2- | xargs)
        continue
    fi

    # Start of input block
    if [[ "$trimmed_line" =~ ^input:\ \| ]]; then
        is_parsing_input=1
        current_input=""
        continue
    fi

    # Parsing input block
    if [[ $is_parsing_input -eq 1 ]]; then
        # Stop parsing input if we hit output marker
        if [[ "$trimmed_line" =~ ^output:\ \| ]]; then
            is_parsing_input=0
        else
            current_input+="$line"$'\n'
            continue
        fi
    fi

    # Start of output block
    if [[ "$trimmed_line" =~ ^output:\ \| ]]; then
        is_parsing_output=1
        current_output=""
        continue
    fi

    # Parsing output block
    if [[ $is_parsing_output -eq 1 ]]; then
        # Stop parsing output if we hit another test case marker
        if [[ "$trimmed_line" =~ ^-\ name: ]]; then
            is_parsing_output=0
            continue
        fi
        current_output+="$line"$'\n'
    fi
done < test_cases.txt

# Run the last test case
if [[ -n "$current_name" && -n "$current_input" ]]; then
    ((total_tests++))
    if run_test_case "$current_input" "$current_output" "$current_name"; then
        ((passed_tests++))
    fi
fi

# Print summary
echo "-------------------"
echo "Test Summary:"
echo "Total Tests: $total_tests"
echo "Passed Tests: $passed_tests"
echo "Failed Tests: $((total_tests - passed_tests))"

# Exit with non-zero status if any tests failed
[ $passed_tests -eq $total_tests ]

rm program
