#include "unity/unity.h"
#include "unity/unity_fixture.h"


TEST_GROUP_RUNNER(heap_group)
{
    RUN_TEST_CASE(heap_group, heap_init);
    RUN_TEST_CASE(heap_group, heap_get_free_size);
    RUN_TEST_CASE(heap_group, heap_malloc);
    RUN_TEST_CASE(heap_group, heap_get_min_free_size);
    RUN_TEST_CASE(heap_group, heap_free);
    RUN_TEST_CASE(heap_group, heap_uninit);
}


