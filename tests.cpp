// Compile using "make tests"
// On Debian I installed libgtest-dev libgtest0 from apt, likely Mint can do the
// same.
// More info can be found at:
// http://code.google.com/p/googletest/wiki/Primer
#include <gtest/gtest.h>
#include "iomanager.h"

// Simple test, just checks that the constructor doesn't throw or segfault.
TEST(IOManager, CanCreateManager) { IOManager io; }

TEST(ProcessManager, WriteBlockWorks)
{
  IOManager io;
  io.write_block(1, "Hey");
  char p[64];
  io.read_block(1, p);
  // Use GTest's own c-string equality assert
  ASSERT_STREQ("Hey", p);
}

TEST(ProcessManager, SaveWorks)
{
  IOManager io;
  io.write_block(1, "Hello There");
  io.save("TestOutput.txt");
  io.write_block(1, "Hey");
  char p[64];
  io.load("TestOutput.txt");
  io.read_block(1, p);
  // Use std::string's == operateor. Either work, this is for demonstration.
  ASSERT_EQ(std::string("Hello There"), p);
}

/*
 * Asserts:
 * ASSERT_X checks X, and if it's not true the test fails.
 * EXPECT_X checks X, but if it's not true it notes it but keeps going.
 * I always use ASSERT_X, but it's up to you.
 * X can be:
 * TRUE, FALSE (Checks a boolean)
 * EQ, NE, LT, LE, GT, GE (calls operator ==, !=, <, <=, >, >=)
 * STREQ, STRNE, STRCASEEQ, STRCASENE
 * (null-terminated strings are equal, not equal, equal ignoring case, not equal
 * ignoring case)
 */
