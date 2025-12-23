#!/bin/bash
#
# SubC Preprocessor Test Suite
#
# Tests preprocessor functionality including:
# - Predefined macros (__SUBC__, __linux__, __i386__, etc.)
# - #define / #undef
# - #ifdef / #ifndef / #else / #endif
# - #include
# - Multi-architecture macro validation
#
# Usage:
#   ./run_tests.sh              Run all tests
#   ./run_tests.sh -v           Verbose mode
#   ./run_tests.sh -s t_name    Run specific test
#   ./run_tests.sh -T target    Test with specific target
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SCC="${SCRIPT_DIR}/../../src/scc0"
OUTPUT_DIR="${SCRIPT_DIR}/output"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Options
VERBOSE=0
SPECIFIC_TEST=""
TARGET=""

# Parse arguments
while getopts "vs:T:h" opt; do
    case $opt in
        v) VERBOSE=1 ;;
        s) SPECIFIC_TEST="$OPTARG" ;;
        T) TARGET="$OPTARG" ;;
        h)
            echo "Usage: $0 [-v] [-s test_name] [-T target]"
            echo "  -v          Verbose mode"
            echo "  -s name     Run specific test (without .c extension)"
            echo "  -T target   Use specific target (e.g., linux-x86-64)"
            exit 0
            ;;
        *)
            echo "Unknown option: $opt"
            exit 1
            ;;
    esac
done

# Check if compiler exists
if [ ! -x "$SCC" ]; then
    echo -e "${RED}Error: Compiler not found at $SCC${NC}"
    echo "Please build the compiler first: cd src && make scc0"
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Build target flag
TARGET_FLAG=""
if [ -n "$TARGET" ]; then
    TARGET_FLAG="-T $TARGET"
fi

# Counters
PASSED=0
FAILED=0
SKIPPED=0

# Run a single test
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .c)
    local asm_file="${OUTPUT_DIR}/${test_name}.s"
    
    if [ $VERBOSE -eq 1 ]; then
        echo -n "Testing $test_name... "
    fi
    
    # Compile to assembly (preprocessor runs during compilation)
    if $SCC $TARGET_FLAG -S -o "$asm_file" "$test_file" 2>"${OUTPUT_DIR}/${test_name}.err"; then
        # Check if test expects specific output in assembly
        if grep -q "EXPECT_ASM:" "$test_file"; then
            local expected=$(grep "EXPECT_ASM:" "$test_file" | sed 's/.*EXPECT_ASM://' | tr -d ' ')
            if grep -q "$expected" "$asm_file"; then
                echo -e "${GREEN}PASS${NC}: $test_name"
                ((PASSED++))
            else
                echo -e "${RED}FAIL${NC}: $test_name (expected pattern not found: $expected)"
                ((FAILED++))
            fi
        else
            # Just check compilation succeeded
            echo -e "${GREEN}PASS${NC}: $test_name"
            ((PASSED++))
        fi
    else
        # Check if test expects failure
        if grep -q "EXPECT_FAIL" "$test_file"; then
            echo -e "${GREEN}PASS${NC}: $test_name (expected failure)"
            ((PASSED++))
        else
            echo -e "${RED}FAIL${NC}: $test_name (compilation failed)"
            if [ $VERBOSE -eq 1 ]; then
                cat "${OUTPUT_DIR}/${test_name}.err"
            fi
            ((FAILED++))
        fi
    fi
}

# Main test loop
echo "========================================"
echo "SubC Preprocessor Test Suite"
echo "========================================"
echo "Compiler: $SCC"
if [ -n "$TARGET" ]; then
    echo "Target: $TARGET"
else
    echo "Target: (default)"
fi
echo "----------------------------------------"

if [ -n "$SPECIFIC_TEST" ]; then
    # Run specific test
    test_file="${SCRIPT_DIR}/${SPECIFIC_TEST}.c"
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    else
        echo -e "${RED}Error: Test file not found: $test_file${NC}"
        exit 1
    fi
else
    # Run all tests
    for test_file in "${SCRIPT_DIR}"/t_*.c; do
        if [ -f "$test_file" ]; then
            run_test "$test_file"
        fi
    done
fi

echo "----------------------------------------"
echo "Results: ${GREEN}$PASSED passed${NC}, ${RED}$FAILED failed${NC}, $SKIPPED skipped"

if [ $FAILED -gt 0 ]; then
    exit 1
fi
exit 0
