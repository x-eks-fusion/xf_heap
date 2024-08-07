#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "xf_heap.h"

static char s_stack_arr[6144] = {0};
static char *s_heap_arr = NULL;

int main(void)
{
    s_heap_arr = (char *)malloc(6144);
    if (s_heap_arr == NULL) {            /* 申请内存失败 */
        printf("malloc error\n");
        return -1;
    }

    /* 构造内存池 */
    xf_heap_region_t heap_regions[] = {
        {(uint8_t *)s_stack_arr, 6144},
        {(uint8_t *)s_heap_arr, 6144},
        {NULL, 0}
    };

    /* 初始化内存池 */
    xf_heap_init(heap_regions);

    /* 查询内存 */
    size_t free_size = xf_heap_get_free_size();
    printf("free_size = %ld\n", free_size);

    /* 申请内存 */
    int *p = (int*)xf_malloc(sizeof(int));
    if (p == NULL) {
        printf("xf_malloc error\n");
        return -1;
    }

    /* 查询内存 */
    free_size = xf_heap_get_free_size();
    printf("free_size = %ld\n", free_size);

    /* 使用内存 */
    *p = 123;
    printf("p = %p, *p = %d\n", p, *p);

    /* 查询最小申请过的大小 */
    size_t min_stack_size = xf_heap_get_min_ever_free_size();
    printf("min_stack_size = %ld\n", min_stack_size);

    /* 释放内存 */
    xf_free(p);

    /* 查询内存 */
    free_size = xf_heap_get_free_size();
    printf("free_size = %ld\n", free_size);


    return 0;
}
