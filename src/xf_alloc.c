/**
 * @file xf_alloc.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief 采用链表管理，对空闲内存采取相邻合并策略，且能注册多处不同的内存
 *      @note 主体部分借鉴了freeRTOS的heap_5.c的功能，在此之上将非内存管理算法
 *      的部分剥离了出去，单独形成xf_heap.c。相当于xf_malloc的默认内存管理方式
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_heap_config.h"
#include "xf_alloc.h"

/* ==================== [Defines] =========================================== */

/* 字节对齐的掩码 */
#define BYTE_ALIGNMENT_MASK (XF_HEAP_BYTE_ALIGNMENT - 1)

/* 内存块最小所需的空间大小 */
#define MINIMUM_BLOCK_SIZE  ((unsigned int) (heap_struct_size << 1))

/* ==================== [Typedefs] ========================================== */

typedef struct _block_link_t {
    struct _block_link_t *next_free_block;  /*!< 下一个区块的位置 */
    unsigned int block_size;                      /*!< 当前区块的大小 */
} block_link_t;

/* ==================== [Static Prototypes] ================================= */

static void insert_block_into_free_list(block_link_t *block_to_insert);

/* ==================== [Static Variables] ================================== */

/* 计算内存块结构体占用大小，并内存对齐 */
static const unsigned int heap_struct_size =
    (sizeof(block_link_t)
     + ((unsigned int)(XF_HEAP_BYTE_ALIGNMENT - 1))) & ~((unsigned int)BYTE_ALIGNMENT_MASK);

/* 内存块大小的最高位掩码，最高位用于检测内存块是否为空闲 */
static const unsigned int block_allocate_bit = ((unsigned int) 1) << ((sizeof(unsigned int) * 8) - 1);

/**
 * @brief 闲内存块链表的起点和终点。
 * 用户在注册的时候末尾 next_free_block 为 (void*) 0，block_size 为 0
 *
 * @note 注意：这里终点是指针。
 */
static block_link_t start, *end = (void *)0;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

void *xf_heap_malloc(unsigned int size)
{
    block_link_t *block, *previous_block, *new_block_link;
    void *ret = (void*) 0;

    XF_HEAP_ASSERT(end);

    if ((size & block_allocate_bit) == 0) {
        /* 申请内存大小进行对齐 */
        if ((size > 0) && ((size + heap_struct_size) > size)) {
            size += heap_struct_size;

            if ((size & BYTE_ALIGNMENT_MASK) != 0x00) {
                if ((size + (XF_HEAP_BYTE_ALIGNMENT - (size & BYTE_ALIGNMENT_MASK))) > size) {
                    size += (XF_HEAP_BYTE_ALIGNMENT - (size & BYTE_ALIGNMENT_MASK));
                } else {
                    size = 0;
                }
            }
        } else {
            size = 0;
        }

        /**
         * 内存剩余足够，则切割成已使用内存块和更小的空闲内存块，
         *  空闲内存块插入空闲内存块链表
         */
        if ((size > 0)) {
            previous_block = &start;
            block = start.next_free_block;

            while ((block->block_size < size) && (block->next_free_block != (void*) 0)) {
                previous_block = block;
                block = block->next_free_block;
            }

            if (block != end) {
                ret = (void *)(((unsigned char *) previous_block->next_free_block) + heap_struct_size);

                previous_block->next_free_block = block->next_free_block;

                if ((block->block_size - size) > MINIMUM_BLOCK_SIZE) {
                    new_block_link = (void *)((unsigned char *) block + size);

                    new_block_link->block_size = block->block_size - size;
                    block->block_size = size;

                    insert_block_into_free_list((new_block_link));
                }

                block->block_size |= block_allocate_bit;
                block->next_free_block = (void*) 0;
            }
        }
    }

    return ret;
}

void xf_heap_free(void *pv)
{
    unsigned char *puc = (unsigned char *) pv;
    block_link_t *link;

    if (pv != (void*) 0) {
        puc -= heap_struct_size;

        link = (void *) puc;

        XF_HEAP_ASSERT((link->block_size & block_allocate_bit) != 0);
        XF_HEAP_ASSERT(link->next_free_block == (void*) 0);

        if ((link->block_size & block_allocate_bit) != 0) {
            if (link->next_free_block == (void*) 0) {
                link->block_size &= ~block_allocate_bit;
                insert_block_into_free_list(((block_link_t *) link));
            }
        }
    }
}

unsigned int xf_heap_region(const xf_heap_region_t *const heap_regions)
{
    block_link_t *first_free_block_in_region = (void*) 0, *previous_free_block;
    xf_heap_intptr_t aligned_heap;
    unsigned int total_region_size, total_heap_size = 0;
    long defined_regions = 0;
    xf_heap_intptr_t address;
    const xf_heap_region_t *heap_region;

    XF_HEAP_ASSERT(end == (void*) 0);

    heap_region = &(heap_regions[defined_regions]);

    /* 循环将heap_regions内的内存分别注册进空闲内存块链表里  */
    while (heap_region->size_in_bytes > 0) {
        total_region_size = heap_region->size_in_bytes;

        address = (xf_heap_intptr_t) heap_region->stat_address;

        if ((address & BYTE_ALIGNMENT_MASK) != 0) {
            address += (XF_HEAP_BYTE_ALIGNMENT - 1);
            address &= ~BYTE_ALIGNMENT_MASK;

            total_region_size -= address - (xf_heap_intptr_t) heap_region->stat_address;
        }

        aligned_heap = address;

        if (defined_regions == 0) {
            start.next_free_block = (block_link_t *) aligned_heap;
            start.block_size = (unsigned int) 0;
        } else {
            XF_HEAP_ASSERT(end != (void*) 0);
            XF_HEAP_ASSERT(address > (unsigned int) end);
        }

        previous_free_block = end;
        address = aligned_heap + total_region_size;
        address -= heap_struct_size;
        address &= ~BYTE_ALIGNMENT_MASK;
        end = (block_link_t *) address;
        end->block_size = 0;
        end->next_free_block = (void*) 0;

        first_free_block_in_region = (block_link_t *) aligned_heap;
        first_free_block_in_region->block_size = address - (xf_heap_intptr_t) first_free_block_in_region;
        first_free_block_in_region->next_free_block = end;

        if (previous_free_block != (void*) 0) {
            previous_free_block->next_free_block = first_free_block_in_region;
        }

        total_heap_size += first_free_block_in_region->block_size;

        defined_regions++;
        heap_region = &(heap_regions[defined_regions]);
    }

    XF_HEAP_ASSERT(total_heap_size);

    return total_heap_size;
}

unsigned int xf_heap_get_block_size(void *pv)
{
    unsigned int block_size = 0;
    unsigned char *puc = (unsigned char *) pv;
    block_link_t *link;

    if (pv != (void*) 0) {
        puc -= heap_struct_size;

        link = (void *) puc;

        if ((link->block_size & block_allocate_bit) != 0) {
            if (link->next_free_block == (void*) 0) {
                block_size = link->block_size & (~block_allocate_bit);
                return block_size;
            }
        }
    }
    return 0;
}


/* ==================== [Static Functions] ================================== */

/**
 * @brief 将内存块插入空闲链表中，前后内存连续则进行合并
 *
 * @param block_to_insert 空闲内存区域数组，需要结尾为{(void*) 0, 0}为最后一个内存块
 */
static void insert_block_into_free_list(block_link_t *block_to_insert)
{
    block_link_t *iterator;
    unsigned char *puc;

    for (iterator = &start; iterator->next_free_block < block_to_insert; iterator = iterator->next_free_block) {
    }

    puc = (unsigned char *) iterator;

    if ((puc + iterator->block_size) == (unsigned char *) block_to_insert) {
        iterator->block_size += block_to_insert->block_size;
        block_to_insert = iterator;
    }

    puc = (unsigned char *) block_to_insert;

    if ((puc + block_to_insert->block_size) == (unsigned char *) iterator->next_free_block) {
        if (iterator->next_free_block != end) {
            block_to_insert->block_size += iterator->next_free_block->block_size;
            block_to_insert->next_free_block = iterator->next_free_block->next_free_block;
        } else {
            block_to_insert->next_free_block = end;
        }
    } else {
        block_to_insert->next_free_block = iterator->next_free_block;
    }

    if (iterator != block_to_insert) {
        iterator->next_free_block = block_to_insert;
    }
}
