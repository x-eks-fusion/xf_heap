#include "unity/unity.h"
#include "unity/unity_fixture.h"


TEST_GROUP_RUNNER(heap_redirect_group)
{
    RUN_TEST_CASE(heap_redirect_group, heap_redirect_func);
}


