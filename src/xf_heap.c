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

static void *_xf_malloc(unsigned int size);
static void _xf_free(void *pv);
static unsigned int _xf_heap_get_free_size(void);
static unsigned int _xf_heap_get_min_ever_free_size(void);

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

const static xf_heap_api_t *s_heap_api = {0};
const static xf_heap_api_t _heap_api = {
    .malloc = _xf_malloc,
    .free = _xf_free,
    .get_free_size = _xf_heap_get_free_size,
    .get_min_ever_free_size = _xf_heap_get_min_ever_free_size
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int xf_heap_redirect(const xf_heap_api_t *api)
{
    if (api == (xf_heap_api_t *)0) {
        return XF_HEAP_FAIL;
    }

    s_heap_api = api;
    return XF_HEAP_OK;
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

    s_heap_api = &_heap_api;

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

    s_heap_api = (xf_heap_api_t *)0;

    return XF_HEAP_OK;
}

void *_xf_malloc(unsigned int size)
{
    if (s_heap_api == (xf_heap_api_t *)0) {
        return (void *) 0;
    }

    return s_heap_api->malloc(size);
}

void _xf_free(void *pv)
{
    if (s_heap_api == (xf_heap_api_t *)0) {
        return;
    }
    s_heap_api->free(pv);
}

unsigned int _xf_heap_get_free_size(void)
{
    if (s_heap_api == (xf_heap_api_t *)0) {
        return (unsigned int) -1;
    }
    return s_heap_api->get_free_size();
}

unsigned int _xf_heap_get_min_ever_free_size(void)
{
    if (s_heap_api == (xf_heap_api_t *)0) {
        return (unsigned int) -1;
    }
    return s_heap_api->get_min_ever_free_size();
}

/* ==================== [Static Functions] ================================== */

static void *_xf_malloc(unsigned int size)
{
    void *res = (void *) 0;
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return (void *) 0;
        }
        res = s_heap.func.malloc(size);
        if (res != (void *) 0) {
            s_heap.free_bytes -= s_heap.func.get_block_size(res);
            if (s_heap.min_ever_free_bytes_remaining > s_heap.free_bytes) {
                s_heap.min_ever_free_bytes_remaining = s_heap.free_bytes;
            }
        }
    }
    XF_HEAP_UNLOCK(s_heap.lock);
    return res;
}

static void _xf_free(void *pv)
{
    XF_HEAP_LOCK(s_heap.lock);
    {
        if (s_heap.init != XF_HEAP_MAGIC_NUM) {
            return;
        }
        if (pv != (void *) 0) {
            s_heap.free_bytes += s_heap.func.get_block_size(pv);
        }
        s_heap.func.free(pv);
    }
    XF_HEAP_UNLOCK(s_heap.lock);
}

static unsigned int _xf_heap_get_free_size(void)
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

static unsigned int _xf_heap_get_min_ever_free_size(void)
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
