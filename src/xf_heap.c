/**
 * @file xf_heap.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief 提供给外界调用的malloc API
 *      @note 加入了计算剩余大小，添加了线程安全
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_heap.h"
#include "xf_alloc.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

typedef struct _heap_t {
    xf_alloc_func_t func;
    void *lock;
    unsigned int init;
    unsigned int free_bytes;
    unsigned int min_ever_free_bytes_remaining;
} heap_t;

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/*初始化默认参数*/
static heap_t s_heap = {
    .lock = XF_HEAP_LOCK_PTR,
    .init = 0,
    .free_bytes = 0,
    .min_ever_free_bytes_remaining = 0,
    .func = {
        .malloc = xf_heap_malloc,
        .free = xf_heap_free,
        .init = xf_heap_region,
        .get_block_size = xf_heap_get_block_size,
    }
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_heap_err_t xf_heap_redirect(xf_alloc_func_t func)
{
    if (s_heap.init != XF_HEAP_MAGIC_NUM && func.malloc &&
            func.free && func.init) {
        s_heap.func.malloc = func.malloc;
        s_heap.func.free = func.free;
        s_heap.func.init = func.init;
        s_heap.func.get_block_size = func.get_block_size;
        return XF_HEAP_OK;
    }
    return XF_HEAP_INITED;
}

int xf_heap_init(const xf_heap_region_t *const regions)
{
    unsigned int total_size = 0;

    if (s_heap.init == XF_HEAP_MAGIC_NUM) {
        return XF_HEAP_INITED;
    }
    s_heap.init = XF_HEAP_MAGIC_NUM;
    total_size = s_heap.func.init(regions);
    s_heap.free_bytes = total_size;
    s_heap.min_ever_free_bytes_remaining = total_size;

    return XF_HEAP_OK;
}

int xf_heap_uninit(void)
{
    if (s_heap.init != XF_HEAP_MAGIC_NUM) {
        return XF_HEAP_UNINIT;
    }

    s_heap.init = 0;
    s_heap.free_bytes = 0;
    s_heap.min_ever_free_bytes_remaining = 0;

    return XF_HEAP_OK;
}

void *xf_malloc(unsigned int size)
{
    void *res = (void*) 0;
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return (void*) 0;
        }
        res = s_heap.func.malloc(size);
        if (res != (void*) 0) {
            s_heap.free_bytes -= s_heap.func.get_block_size(res);
            if (s_heap.min_ever_free_bytes_remaining > s_heap.free_bytes) {
                s_heap.min_ever_free_bytes_remaining = s_heap.free_bytes;
            }
        }
    }
    XF_HEAP_UNLOCK(s_heap.lock);
    return res;
}

void xf_free(void *pv)
{
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return;
        }
        if (pv != (void*) 0) {
            s_heap.free_bytes += s_heap.func.get_block_size(pv);
        }
        s_heap.func.free(pv);
    }
    XF_HEAP_UNLOCK(s_heap.lock);
}

unsigned int xf_heap_get_free_size(void)
{
    unsigned int res = 0;
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return 0;
        }

        res = s_heap.free_bytes;
    }
    XF_HEAP_UNLOCK(s_heap.lock);

    return res;
}

unsigned int xf_heap_get_min_ever_free_size(void)
{
    unsigned int res = 0;
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return 0;
        }

        res = s_heap.min_ever_free_bytes_remaining;
    }
    XF_HEAP_UNLOCK(s_heap.lock);

    return res;
}

/* ==================== [Static Functions] ================================== */
