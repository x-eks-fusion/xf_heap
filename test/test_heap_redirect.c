/**
 * @file test_heap_redirect.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 * 
 */

#include "unity/unity.h"
#include "unity/unity_fixture.h"
#include "xf_heap.h"

TEST_GROUP(heap_redirect_group);

static unsigned int  s_count = 0;

static void* _malloc(unsigned int size)
{
    s_count++;
    return NULL;
}

static void _free(void *pv)
{
    s_count--;
}

unsigned int init(const xf_heap_region_t *const regions)
{
    return 0;
}

TEST_SETUP(heap_redirect_group)
{

}

TEST_TEAR_DOWN(heap_redirect_group)
{
}


TEST(heap_redirect_group, heap_redirect_func)
{
    xf_alloc_func_t func;
    func.malloc = _malloc;
    func.free = _free;
    func.init = init;
    TEST_ASSERT_EQUAL_INT(0, xf_heap_redirect(func));
    TEST_ASSERT_EQUAL_INT(0, xf_heap_init(NULL));
    TEST_ASSERT_EQUAL_INT(0, s_count);
    xf_malloc(0);
    TEST_ASSERT_EQUAL_INT(1, s_count);
    xf_free(NULL);
    TEST_ASSERT_EQUAL_INT(0, s_count);
}


