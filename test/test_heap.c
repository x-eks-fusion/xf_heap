/**
 * @file test_heap.c
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

TEST_GROUP(heap_group);

static char s_heap_arr[6144] = {0};

static int *s_ptr = 0;

size_t s_size = 0;

TEST_SETUP(heap_group)
{
}

TEST_TEAR_DOWN(heap_group)
{
}

TEST(heap_group, heap_init)
{
    xf_heap_region_t heap_regions[] = {
        {(uint8_t *)s_heap_arr, 6144},
        {NULL, 0}
    };
    TEST_ASSERT_EQUAL(0, xf_heap_init(heap_regions));
}

TEST(heap_group, heap_get_free_size)
{
    size_t _size = xf_heap_get_free_size();
    TEST_ASSERT_NOT_EQUAL(0, _size);
    s_size = _size;
}

TEST(heap_group, heap_malloc)
{
    s_ptr = xf_malloc(sizeof(int));
    TEST_ASSERT_NOT_NULL(s_ptr);
}

TEST(heap_group, heap_get_min_free_size)
{
    size_t _size = xf_heap_get_min_ever_free_size();
    TEST_ASSERT_NOT_EQUAL(0, _size);
}

TEST(heap_group, heap_free)
{
    xf_free(s_ptr);
    size_t _size = xf_heap_get_free_size();
    TEST_ASSERT_EQUAL(s_size, _size);
}


TEST(heap_group, heap_uninit)
{
    TEST_ASSERT_EQUAL(0, xf_heap_uninit());
}


