#include "gtest/gtest.h"

namespace
{
    // Tests that the Foo::Bar() method does Abc.
    TEST(Example, Test)
    {
        EXPECT_TRUE(false);
    }
} // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    __debugbreak(); // So we can actually see the result before we quit
    return result;
}