# XF_HEAP

## 项目特点

本仓库参考了freeRTOS的内存管理，有以下特点：
1. 支持替换内存算法，可以注册修改其内存算法实现
2. 内存管理算法通过合并相邻的空闲块来减少内存碎片，从而提高内存利用率。
3. 可以使用堆或全局数据区或外部空间初始化内存池，针对不同内存需求提供内存管理。
4. 可以查询剩余内存大小和曾经最小空闲内存
5. 内存对齐，访问速度
6. 对OS调用增加 XF_HEAP_LOCK 对内存申请进行保护
7. C99 标准，无任何依赖（包括libc），方便移植到各个嵌入式代码中

## 运行测试

```bash
xmake b                 # 编译
xmake r xf_heap         # 运行例程
xmake r xf_heap_test    # 运行单元测试
```

## 运行结果

**例程运行结果**
```bash
free_size = 12256
free_size = 12236
p = 0x556abe3dc0b0, *p = 123
min_stack_size = 12256
free_size = 12256
```

**单元测试运行结果**
```bash
Unity test run 1 of 1
.......

-----------------------
7 Tests 0 Failures 0 Ignored
OK
```

## 基础API
```c
/**
 * @brief 申请内存
 *
 * @param size 申请内存大小
 * @return void* 申请内存的地址
 */
void *xf_malloc(size_t size);

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
int xf_heap_redirect(xf_alloc_func_t func);

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
 * @return size_t 内存总空闲大小
 */
size_t xf_heap_get_free_size(void);

/**
 * @brief 获取曾经最少空闲内存
 *
 * @return size_t 空闲内存的字节数
 */
size_t xf_heap_get_min_ever_free_size(void);
```

## 移植建议

移植只需要复制src里面的文件即可，需要给一个 xf_heap_config.h （空白则为全部使用默认配置）文件作为配置文件。
可配置的内容和默认配置可以在 xf_heap_internal_config.h 中查看