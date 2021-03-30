#include <gtest/gtest.h>
#include "TestFixture.hpp"

#include "../TestClass.hpp"

TEST_F(TestFixture, TestClass_DefaultCtor) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  ASSERT_EXIT(
    {
      test::TestClass t;

      exit(0);
    },
    ::testing::ExitedWithCode(0), "");
}

TEST_F(TestFixture, TestClass_getters) {


  test::TestClass t(5, 10);
  EXPECT_EQ(5, t.getI());
  EXPECT_EQ(10, t.getJ());

}
