#include "unity/unity_fixture.h"

static void RunAllTests(void)
{
    RUN_TEST_GROUP(heap_group);
    RUN_TEST_GROUP(heap_redirect_group);
}

int main(int argc, const char *argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}
