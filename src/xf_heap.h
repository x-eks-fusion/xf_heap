/**
 * @file xf_heap.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 这部分是给用户调用的
 *      @note 调用xf_heap_redirect函数，用户可以自己添加自己的内存管理函数
 *      当前的封装仅仅用于隔离内存管理算法，以便用户后期替换。当然，用户注册
 *      的xf_heap_redirect需要在一切开始之前才能生效。否则则会调用默认的内存
 *      管理算法xf_alloc.c
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __XF_HEAP_H__
#define __XF_HEAP_H__

/* ==================== [Includes] ========================================== */

#include "xf_heap_internal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] =========================================== */

typedef struct _xf_heap_region_t {
    unsigned char *stat_address;  /*!< 内存块起始地址 */
    unsigned int size_in_bytes;   /*!< 内存块大小 */
} xf_heap_region_t;

typedef struct _xf_alloc_func_t {
    void *(*malloc)(unsigned int size);
    void (*free)(void *pv);
    unsigned int (*init)(const xf_heap_region_t *const regions);
    unsigned int (*get_block_size)(void *pv); /*!< 获取内存块的大小 */
} xf_alloc_func_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 申请内存
 *
 * @param size 申请内存大小
 * @return void* 申请内存的地址
 */
void *xf_malloc(unsigned int size);

/**
 * @brief 释放内存
 *
 * @param pv 释放内存的大小
 *
 * @note 释放内存时记得将指针指向NULL，以防出现访问未申请的内存
 */
void xf_free(void *pv);

/**
 * @brief 相关申请的函数重定向
 *
 * @param func 重定向的函数
 *
 * @note 该函数只能在未初始化之前调用
* @return int 0 设置成功， -1 设置失败
 */
xf_heap_err_t xf_heap_redirect(xf_alloc_func_t func);

/**
 * @brief 内存初始化
 *
 * @param regions 注册不同内存区域，数组最后一个必须是{}
 *
 * @note 该函数只能在xf_malloc之前调用
 * 
 * @return int 0 设置成功， -1 设置失败 
 */
int xf_heap_init(const xf_heap_region_t *const regions);

/**
 * @brief 内存反初始化
 * 
 * @return int 0 设置成功， -1 设置失败 
 */
int xf_heap_uninit(void);

/**
 * @brief 获取内存总空闲大小
 *
 * @return unsigned int 内存总空闲大小
 */
unsigned int xf_heap_get_free_size(void);

/**
 * @brief 获取曾经剩余最少空闲内存
 *
 * @return size_t 空闲内存的字节数
 */
unsigned int xf_heap_get_min_ever_free_size(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_HEAP_H__
