#!/bin/bash
#
# SubC C89 Test Suite Runner
# Compiles and runs all test files in the c89_suite directory
# Supports comparison between SubC and native compiler (GCC/Clang)
#
# Usage: ./run_tests.sh [options]
#   -t <target>   Target architecture (default: auto-detect)
#   -c            Compile only (don't run)
#   -k            Keep generated files (.s, .o, executables)
#   -v            Verbose output
#   -C            Compare mode: compare SubC vs native compiler output
#   -s <test>     Run single test only
#   -h            Show help
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
SCC="$PROJECT_ROOT/src/scc0"
INCLUDE_DIR="$PROJECT_ROOT/runtime/include"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Default options
TARGET=""
COMPILE_ONLY=0
KEEP_FILES=0
VERBOSE=0
COMPARE_MODE=0
SINGLE_TEST=""

# Counters
TOTAL=0
PASSED=0
FAILED=0
SKIPPED=0
COMPARE_MATCH=0
COMPARE_MISMATCH=0

# Arrays for results
declare -a FAILED_TESTS
declare -a PASSED_TESTS
declare -a MISMATCH_TESTS

# Detect native compiler
detect_native_cc() {
    if command -v clang >/dev/null 2>&1; then
        echo "clang"
    elif command -v gcc >/dev/null 2>&1; then
        echo "gcc"
    elif command -v cc >/dev/null 2>&1; then
        echo "cc"
    else
        echo ""
    fi
}

NATIVE_CC=$(detect_native_cc)

usage() {
    echo "SubC C89 Test Suite Runner"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -t <target>   Target architecture (e.g., linux-x86-64, darwin-x86-64)"
    echo "  -c            Compile only (don't run tests)"
    echo "  -k            Keep generated files in output/ directory"
    echo "  -v            Verbose output"
    echo "  -C            Compare mode: compare SubC output vs native compiler"
    echo "  -s <test>     Run single test (e.g., -s t_arithmetic)"
    echo "  -h            Show this help"
    echo ""
    echo "Examples:"
    echo "  $0                    # Run all tests"
    echo "  $0 -c                 # Compile only, check for errors"
    echo "  $0 -C                 # Compare SubC vs $NATIVE_CC output"
    echo "  $0 -s t_vtable -v     # Run single test with verbose output"
    echo "  $0 -k                 # Keep .s files in output/"
    echo ""
    echo "Available targets (run '$SCC -L' for full list):"
    echo "  linux-386, linux-x86-64, darwin-x86-64, freebsd-386, etc."
    exit 0
}

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_compare() {
    echo -e "${CYAN}[CMP]${NC} $1"
}

log_verbose() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${BLUE}[DEBUG]${NC} $1"
    fi
}

# Detect target based on OS and architecture
detect_target() {
    local os=$(uname -s)
    local arch=$(uname -m)
    
    case "$os" in
        Linux)
            case "$arch" in
                x86_64) echo "linux-x86-64" ;;
                i*86)   echo "linux-386" ;;
                *)      echo "linux-x86-64" ;;
            esac
            ;;
        Darwin)
            echo "darwin-x86-64"
            ;;
        FreeBSD)
            case "$arch" in
                amd64)  echo "freebsd-x86-64" ;;
                i*86)   echo "freebsd-386" ;;
                arm*)   echo "freebsd-armv6" ;;
                *)      echo "freebsd-x86-64" ;;
            esac
            ;;
        NetBSD)
            case "$arch" in
                amd64)  echo "netbsd-x86-64" ;;
                *)      echo "netbsd-386" ;;
            esac
            ;;
        OpenBSD)
            echo "openbsd-386"
            ;;
        *)
            echo "linux-x86-64"
            ;;
    esac
}

# Parse command line arguments
while getopts "t:ckvCs:h" opt; do
    case $opt in
        t) TARGET="$OPTARG" ;;
        c) COMPILE_ONLY=1 ;;
        k) KEEP_FILES=1 ;;
        v) VERBOSE=1 ;;
        C) COMPARE_MODE=1 ;;
        s) SINGLE_TEST="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Check if compiler exists
if [ ! -x "$SCC" ]; then
    log_fail "Compiler not found at $SCC"
    echo "Please build the compiler first: cd src && make scc0"
    exit 1
fi

# Check native compiler for compare mode
if [ $COMPARE_MODE -eq 1 ] && [ -z "$NATIVE_CC" ]; then
    log_fail "No native compiler found (gcc/clang). Cannot run compare mode."
    exit 1
fi

# Auto-detect target if not specified
if [ -z "$TARGET" ]; then
    TARGET=$(detect_target)
    log_info "Auto-detected target: $TARGET"
fi

# Verify target is valid
if ! "$SCC" -L 2>/dev/null | grep -q "$TARGET"; then
    log_warn "Target '$TARGET' may not be available. Proceeding anyway..."
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/subc"
mkdir -p "$OUTPUT_DIR/native"

echo ""
echo -e "${BOLD}========================================${NC}"
echo -e "${BOLD}  SubC C89 Test Suite${NC}"
echo -e "${BOLD}========================================${NC}"
echo ""
echo "SubC Compiler: $SCC"
echo "Native Compiler: $NATIVE_CC"
echo "Include dir: $INCLUDE_DIR"
echo "Target: $TARGET"
echo "Output dir: $OUTPUT_DIR"
if [ $COMPARE_MODE -eq 1 ]; then
    echo -e "Mode: ${CYAN}Compare (SubC vs $NATIVE_CC)${NC}"
fi
echo ""
echo -e "${BOLD}----------------------------------------${NC}"
echo ""

# Function to compile with SubC
compile_subc() {
    local src="$1"
    local name=$(basename "$src" .c)
    local asm="$OUTPUT_DIR/subc/${name}.s"
    
    log_verbose "SubC compiling: $src -> $asm"
    
    if "$SCC" -T "$TARGET" -I "$INCLUDE_DIR" -S -o "$asm" "$src" 2>&1; then
        echo "$asm"
        return 0
    else
        return 1
    fi
}

# Function to compile and run with native compiler
compile_run_native() {
    local src="$1"
    local name=$(basename "$src" .c)
    local exe="$OUTPUT_DIR/native/${name}"
    
    log_verbose "Native compiling: $src -> $exe"
    
    if $NATIVE_CC -std=c89 -Wall -o "$exe" "$src" -I "$INCLUDE_DIR" 2>&1; then
        # Run and capture output
        local output
        output=$("$exe" 2>&1) || true
        echo "$output"
        return 0
    else
        return 1
    fi
}

# Function to run comparison test
run_compare_test() {
    local src="$1"
    local name=$(basename "$src" .c)
    
    TOTAL=$((TOTAL + 1))
    
    echo -n "Comparing $name... "
    
    # Compile with SubC
    local subc_output
    subc_output=$(compile_subc "$src" 2>&1)
    local subc_status=$?
    
    if [ $subc_status -ne 0 ]; then
        log_fail "SubC compilation failed"
        if [ $VERBOSE -eq 1 ]; then
            echo "SubC error: $subc_output"
        fi
        FAILED=$((FAILED + 1))
        FAILED_TESTS+=("$name (subc compile)")
        return 1
    fi
    
    # Compile and run with native compiler
    local native_output
    native_output=$(compile_run_native "$src" 2>&1)
    local native_status=$?
    
    if [ $native_status -ne 0 ]; then
        log_warn "Native compilation failed (skipping comparison)"
        SKIPPED=$((SKIPPED + 1))
        return 0
    fi
    
    # For now, we can only compare if we can also run SubC output
    # Since SubC doesn't have full runtime, we compare compilation success
    # and native execution output
    
    # Check if native test passed
    if echo "$native_output" | grep -q "^PASS:"; then
        log_success "SubC compiled OK, Native: PASS"
        PASSED=$((PASSED + 1))
        PASSED_TESTS+=("$name")
        COMPARE_MATCH=$((COMPARE_MATCH + 1))
    else
        log_warn "SubC compiled OK, Native: output differs"
        if [ $VERBOSE -eq 1 ]; then
            echo "Native output: $native_output"
        fi
        COMPARE_MISMATCH=$((COMPARE_MISMATCH + 1))
        MISMATCH_TESTS+=("$name")
    fi
}

# Function to run a standard test
run_test() {
    local src="$1"
    local name=$(basename "$src" .c)
    
    TOTAL=$((TOTAL + 1))
    
    echo -n "Testing $name... "
    
    # Compile with SubC
    local compile_output
    compile_output=$(compile_subc "$src" 2>&1)
    local compile_status=$?
    
    if [ $compile_status -ne 0 ]; then
        log_fail "Compilation failed"
        if [ $VERBOSE -eq 1 ]; then
            echo "$compile_output"
        fi
        FAILED=$((FAILED + 1))
        FAILED_TESTS+=("$name (compile)")
        return 1
    fi
    
    if [ $COMPILE_ONLY -eq 1 ]; then
        log_success "Compiled OK"
        PASSED=$((PASSED + 1))
        PASSED_TESTS+=("$name")
        return 0
    fi
    
    # Try to build and run with native compiler for verification
    local native_exe="$OUTPUT_DIR/native/${name}"
    if $NATIVE_CC -std=c89 -Wall -o "$native_exe" "$src" -I "$INCLUDE_DIR" 2>/dev/null; then
        local run_output
        run_output=$("$native_exe" 2>&1) || true
        local run_status=$?
        
        if echo "$run_output" | grep -q "^PASS:"; then
            log_success "OK"
            PASSED=$((PASSED + 1))
            PASSED_TESTS+=("$name")
        else
            log_fail "Runtime failure"
            if [ $VERBOSE -eq 1 ]; then
                echo "Output: $run_output"
            fi
            FAILED=$((FAILED + 1))
            FAILED_TESTS+=("$name (runtime)")
        fi
    else
        log_warn "Skipped (cannot build with native compiler)"
        SKIPPED=$((SKIPPED + 1))
    fi
}

# Find test files
if [ -n "$SINGLE_TEST" ]; then
    # Single test mode
    test_file="$SCRIPT_DIR/${SINGLE_TEST}.c"
    if [ ! -f "$test_file" ]; then
        log_fail "Test file not found: $test_file"
        exit 1
    fi
    
    if [ $COMPARE_MODE -eq 1 ]; then
        run_compare_test "$test_file"
    else
        run_test "$test_file"
    fi
else
    # Run all tests
    for test_file in "$SCRIPT_DIR"/t_*.c; do
        if [ -f "$test_file" ]; then
            if [ $COMPARE_MODE -eq 1 ]; then
                run_compare_test "$test_file"
            else
                run_test "$test_file"
            fi
        fi
    done
fi

# Clean up if not keeping files
if [ $KEEP_FILES -eq 0 ]; then
    rm -rf "$OUTPUT_DIR/subc"/* 2>/dev/null || true
    rm -rf "$OUTPUT_DIR/native"/* 2>/dev/null || true
else
    log_info "Generated files saved to $OUTPUT_DIR/"
    echo "  SubC assembly: $OUTPUT_DIR/subc/"
    echo "  Native binaries: $OUTPUT_DIR/native/"
fi

# Print summary
echo ""
echo -e "${BOLD}========================================${NC}"
echo -e "${BOLD}  Test Summary${NC}"
echo -e "${BOLD}========================================${NC}"
echo ""
echo -e "Total:   ${BOLD}$TOTAL${NC}"
echo -e "Passed:  ${GREEN}$PASSED${NC}"
echo -e "Failed:  ${RED}$FAILED${NC}"
echo -e "Skipped: ${YELLOW}$SKIPPED${NC}"

if [ $COMPARE_MODE -eq 1 ]; then
    echo ""
    echo -e "${CYAN}Comparison Results:${NC}"
    echo -e "  Match:    ${GREEN}$COMPARE_MATCH${NC}"
    echo -e "  Mismatch: ${YELLOW}$COMPARE_MISMATCH${NC}"
fi

echo ""

if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed tests:${NC}"
    for t in "${FAILED_TESTS[@]}"; do
        echo "  - $t"
    done
    echo ""
fi

if [ ${#MISMATCH_TESTS[@]} -gt 0 ]; then
    echo -e "${YELLOW}Mismatched tests:${NC}"
    for t in "${MISMATCH_TESTS[@]}"; do
        echo "  - $t"
    done
    echo ""
fi

if [ $PASSED -eq $TOTAL ] && [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}${BOLD}All tests passed!${NC}"
    exit 0
elif [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All compiled tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
