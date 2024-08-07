/**
 * @file xf_heap_internal_config.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-18
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_HEAP_INTERNAL_CONFIG_H__
#define __XF_HEAP_INTERNAL_CONFIG_H__

/* ==================== [Includes] ========================================== */

#include "xf_heap_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* 锁的指针，这里是加入OS后互斥锁的对象 */
#ifndef XF_HEAP_LOCK_PTR
#define XF_HEAP_LOCK_PTR (void*)0
#endif // XF_HEAP_LOCK_PTR

/* 魔法数字，用于初始化标记 */
#ifndef XF_HEAP_MAGIC_NUM
#define XF_HEAP_MAGIC_NUM 0xDEADBEEF
#endif // XF_HEAP_MAGIC_NUM

/* 字节对齐，加速访问速度 */
#ifndef XF_HEAP_BYTE_ALIGNMENT
#define XF_HEAP_BYTE_ALIGNMENT 4
#endif // XF_HEAP_BYTE_ALIGNMENT

/**
 * @brief heap的错误类型
 *
 */
#ifndef XF_HEAP_ERR_TYPE
#define XF_HEAP_ERR_TYPE int
#endif

typedef XF_HEAP_ERR_TYPE xf_heap_err_t;

/**
 * @brief heap的错误码
 *
 */

#ifndef XF_HEAP_OK
#define XF_HEAP_OK (0)
#endif

#ifndef XF_HEAP_INITED
#define XF_HEAP_INITED (1)
#endif

#ifndef XF_HEAP_UNINIT
#define XF_HEAP_UNINIT (2)
#endif

/**
 * @brief heap的指针整数数类型
 * 
 */

#ifndef XF_HEAP_INTPTR_TYPE
#include <stdint.h>
#define XF_HEAP_INTPTR_TYPE intptr_t
#endif
 
typedef XF_HEAP_INTPTR_TYPE xf_heap_intptr_t;

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

/* 加入OS后对接互斥锁的加锁函数 */
#ifndef XF_HEAP_LOCK
#define XF_HEAP_LOCK(PLOCK) ((void)(PLOCK))
#endif // XF_HEAP_LOCK

/* 加入OS后对接互斥锁的解锁函数 */
#ifndef XF_HEAP_UNLOCK
#define XF_HEAP_UNLOCK(PLOCK) ((void)(PLOCK))
#endif // XF_HEAP_UNLOCK

#ifndef XF_HEAP_ASSERT
#define XF_HEAP_ASSERT(x)
#endif // XF_HEAP_ASSERT



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_HEAP_INTERNAL_CONFIG_H__
