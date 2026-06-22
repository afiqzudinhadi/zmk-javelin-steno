// Override Javelin's unit_test.h to suppress test body compilation.
// When Javelin source files #include "unit_test.h", this file is found
// first (via include path ordering) and replaces the original.

#pragma once
#include <assert.h>

// TEST_BEGIN opens a never-instantiated template function body.
// Code between TEST_BEGIN and TEST_END is syntactically parsed but
// never compiled (dead template), avoiding errors from RUN_TESTS-only APIs.

#define TEST_BEGIN__(text, file, line)                  \
  namespace DeadTest##line {                            \
    template<bool _enable=false>                        \
    static void Dead() {

#define TEST_END                                        \
    }                                                   \
  }

#define TEST_BEGIN_(desc, file, line) TEST_BEGIN__(desc, file, line)
#define TEST_BEGIN(desc) TEST_BEGIN_(desc, __FILE__, __LINE__)
