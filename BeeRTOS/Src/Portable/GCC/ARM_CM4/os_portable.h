/******************************************************************************************
 * @brief Header File for BeeRTOS Portable Layer
 * @file os_portable.h
 * This header file defines the portable layer's interface for BeeRTOS, providing essential
 * utilities and definitions that abstract architecture-specific details, ensuring the kernel
 * can operate across different hardware platforms. It includes macros for computing the
 * highest priority task from a set of tasks, based on their ready states, and defines the
 * stack type used by the tasks.
 ******************************************************************************************/

#ifndef __OS_PORTABLE_H__
#define __OS_PORTABLE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define OS_LOG2(x) (32 - __builtin_clz(x))

#define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK8(mask)  (32 - __builtin_clz(mask))
#define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK16(mask) (32 - __builtin_clz(mask))
#define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK32(mask) (32 - __builtin_clz(mask))
#define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK64(mask) (64 - __builtin_clzll(mask))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

typedef uint32_t os_stack_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /* __OS_PORTABLE_H__ */