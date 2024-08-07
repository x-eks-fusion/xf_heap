/**
 * @file xf_alloc.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 对xf_heap.c提供最基础的内存管理部分API
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023, CorAL. All rights reserved.
 *
 */

#ifndef __XF_ALLOC_H__
#define __XF_ALLOC_H__

/* ==================== [Includes] ========================================== */

#include "xf_heap_internal_config.h"

#include "xf_heap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 带内存管理的内存申请函数
 *
 * @param size 申请内存的大小
 * @return void* 申请内存地址
 */
void *xf_heap_malloc(unsigned int size);

/**
 * @brief 带内存管理的内存释放函数
 *
 * @param pv 需要释放的指针地址
 */
void xf_heap_free(void *pv);

/**
 * @brief 内存注册，需要在使用xf_heap_malloc之前注册
 *
 * @param heap_regions 注册内存的数据信息
 * @return unsigned int 总共可用内存大小
 */
unsigned int xf_heap_region(const xf_heap_region_t *const heap_regions);

/**
 * @brief 获取内存块的实际大小
 *
 * @param pv 内存块指针
 * @return unsigned int 内存块实际占用内存大小
 */
unsigned int xf_heap_get_block_size(void *pv);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_ALLOC_H__
