// Force-included before all Javelin sources to suppress test compilation.
// Redefine TEST macros to produce no function body, avoiding compilation
// of test code that references RUN_TESTS-only APIs.

#ifndef JAVELIN_SUPPRESS_TESTS_H
#define JAVELIN_SUPPRESS_TESTS_H

// Override the test macros from unit_test.h.
// Original (without RUN_TESTS):
//   TEST_BEGIN → namespace UnitTestNNN { static void Test() {
//   TEST_END   → } }
//
// Our override makes Test() an empty function, then opens a dead namespace
// that swallows the original test body code until TEST_END closes it.
// The swallowed code goes into a struct with an unused template, so it
// never gets instantiated or compiled.

#undef TEST_BEGIN__
#undef TEST_BEGIN_
#undef TEST_BEGIN
#undef TEST_END

#define TEST_BEGIN__(text, file, line)                  \
  namespace UnitTest##line {                            \
    static inline void Test() {}                        \
    template<bool=false> struct Unused {                 \
      static void Swallowed() {

#define TEST_END                                        \
      }                                                 \
    };                                                  \
  }

#define TEST_BEGIN_(desc, file, line) TEST_BEGIN__(desc, file, line)
#define TEST_BEGIN(desc) TEST_BEGIN_(desc, __FILE__, __LINE__)

#endif
