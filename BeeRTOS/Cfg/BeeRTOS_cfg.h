/******************************************************************************************
 * @brief 
 * @file 
 * ****************************************************************************************/

#ifndef __BEERTOS_CFG_H__
#define __BEERTOS_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! Enable/Disable OS features */
#define BEERTOS_ALARM_MODULE_EN     (true)
#define BEERTOS_MESSAGE_MODULE_EN   (true)
#define BEERTOS_QUEUE_MODULE_EN     (true)
#define BEERTOS_MUTEX_MODULE_EN     (true)
#define BEERTOS_SEMAPHORE_MODULE_EN (true)

/* Stack size of the idle task in bytes - if callbacks not used you can set 16 bytes */
#define BEERTOS_IDLE_TASK_STACK_SIZE    (64U)
/* Callback when idle task is initialized (before entering the idle loop) */
#define BEERTOS_IDLE_TASK_INIT_CB()     {}
/* Callback when idle task is running (in the idle loop) */
#define BEERTOS_IDLE_TASK_CB()          {}

/* Enable this option to use the stack overflow detection feature. 
 * When tasks is created, the stack is filled with a pattern. Each context switch
 * the pattern is checked. If the pattern is not the same, then we can assume that
 * the stack has overflowed. */
#define BEERTOS_USE_TASK_STACK_MONITOR  (true)
/* Fast metod checks only the last 4 bytes of the stack each context switch. */
#define BEERTOS_USE_FAST_STACK_MONITOR  (true)
/* This method checks the OS_TASK_STACK_CHECK_BYTE_COUNT bytes of the stack each context switch. */
#define BEERTOS_USE_USER_STACK_MONITOR  (false)
#define OS_TASK_STACK_CHECK_BYTE_COUNT  (10U)

/*!
 *  @brief OS tasks configuration list - define your tasks here.
 *  In the current implementation, all tasks are created statically.
 *  The priority of the tasks is defined by their order in this list -
 *  the first task has the highest priority. Up to 64 tasks can be created,
 *  but one is reserved for the idle task.
 *
 *  Structure for defining a task: BEERTOS_TASK(task_id, function, stacksize, autostart, task_arg)
 *  @param task_id - Unique task identifier, created in the os_task_id_t enum.
 *  @param function - The task function to be executed.
 *  @param stacksize - The stack size for the task in bytes.
 *  @param autostart - If TRUE, the task is automatically started after OS initialization.
 *  @param task_arg - Argument passed to the task function. Use a void* pointer and cast
 *                    to the required type within the task function.
 *
 *  Example of defining a task function:
 *      void task_function(void* arg) {
 *          uint32_t my_arg = (uint32_t)arg; // Casting argument to the expected type
 *      }
 *
 *  @brief BeeRTOS mutex list - define your mutexes here.
 *  Mutexes are used to protect critical sections from concurrent access by multiple tasks.
 *  They are recursive, allowing the same task to lock the mutex multiple times, requiring
 *  an equal number of unlocks. Mutexes support priority inheritance to prevent priority inversion.
 *
 *  Structure for defining a mutex: BEERTOS_MUTEX(mutex_id, initial_count)
 *  @param mutex_id - Unique mutex identifier, created in the os_mutex_id_t enum.
 *  @param initial_count - The initial count for the mutex. For binary mutexes, this is usually 0 or 1.
 *
 *  @brief BeeRTOS alarm task configuration - define the alarm task here.
 *  There can only be one alarm task defined in the system. This task is responsible for
 *  handling all alarms.
 *
 *  Structure for defining the alarm task: BEERTOS_ALARM_TASK(task_id, stacksize)
 *  @param task_id - Task identifier for the alarm task, created in the os_task_id_t enum.
 *  @param stacksize - The stack size for the alarm task in bytes.
 */
/*! @brief BeeRTOS priority list - define tasks, mutexes, and alarm task here
 *
 *  This configuration defines system tasks, mutexes, and a single alarm task as per
 *  the new capabilities of the BeeRTOS system.
 */
#define BEERTOS_PRIORITY_LIST()                                             \
    BEERTOS_TASK(OS_TASK_UT_MAIN, ut_beertos_main_task, 128, true, NULL)    \
    BEERTOS_ALARM_TASK(OS_ALARM_TASK, 128)                                  \
    /* Priority test tasks */                                               \
    BEERTOS_TASK(OS_TASK_PRIO_3, ut_task_priority_highest, 128, true, NULL) \
    BEERTOS_TASK(OS_TASK_PRIO_2, ut_task_priority_medium, 128, true, NULL)  \
    BEERTOS_TASK(OS_TASK_PRIO_1, ut_task_priority_lowest, 128, true, NULL)  \
    /* Mutex test tasks */                                                  \
    BEERTOS_MUTEX(MUTEX_ONE, 0U)                                            \
    BEERTOS_MUTEX(MUTEX_TWO, 0U)                                            \
    BEERTOS_MUTEX(MUTEX_THREE, 0U)                                          \
    BEERTOS_TASK(OS_TASK_MUTEX_3, ut_task_mutex_3, 128, true, NULL)         \
    BEERTOS_TASK(OS_TASK_MUTEX_2, ut_task_mutex_2, 128, true, NULL)         \
    BEERTOS_TASK(OS_TASK_MUTEX_1, ut_task_mutex_1, 128, true, NULL)         \
    /* Message test tasks */                                                \
    BEERTOS_TASK(OS_TASK_MSG_2, ut_task_msg_2, 128, false, NULL)            \
    BEERTOS_TASK(OS_TASK_MSG_1, ut_task_msg_1, 128, false, NULL)


/*! @brief BeeRTOS message list - define your messages here
 * Messages are more specific than queues, they can store only one type of data
 * Messages in BeeRTOS are implemented as circular buffers, if message is full, new elements
 * will not overwrite old ones, task will be delayed until there is space in message, or
 * timeout occurs.
 *
 * Structure: OS_MESSAGE(message_id, messages_count, message_size)
 * @param message_id - message id (created in os_message_id_t enum), must be unique
 * @param messages_count - number of messages that can be stored in queue
 * @param message_size - size of single message in bytes
 */
#define OS_MESSAGES_LIST()              \
    OS_MESSAGE(MESSAGE_ONE,   2,  8)    \
    OS_MESSAGE(MESSAGE_TWO,   10, 4)    \
    OS_MESSAGE(MESSAGE_THREE, 10, 4)


/*! @brief BeeRTOS queue list - define your queues here
 *  Queues are more general than messages, they can store any type of data
 *  There can be pushed any number of elements to queue, as long as queue is not full.
 *  There can be popped any number of elements from queue, as long as queue is not empty.
 *  Queues in BeeRTOS are implemented as circular buffers, if queue is full, new elements
 *  will not overwrite old ones, but will be discarded without task delay.
 * 
 *  Structure: OS_QUEUE(queue_id, queue size)
 * @param queue_id - queue id (created in os_queue_id_t enum), must be unique
 * @param queue size - queue size in bytes - maximum number of elements that can be stored in queue
 */
#define BEERTOS_QUEUE_LIST()    \
    OS_QUEUE(QUEUE_1, 10U)      \
    OS_QUEUE(QUEUE_2, 10U)      \


/*! @brief BeeRTOS semaphore list - define your semaphores here
 * Semaphores are used to synchronize tasks and to protect shared resources from being
 * accessed by more than one task at the same time. Semaphores can be used to implement
 * mutual exclusion, but mutexes are preferred for this purpose. Semaphores do not implement
 * priority inheritance. Semaphores can be used for signaling between tasks.
 * 
 * Structure: BEERTOS_SEMAPHORE(semaphore_id, initial_count, type)
 * @param semaphore_id - semaphore id (created in os_semaphore_id_t enum), must be unique
 * @param initial_count - initial count of the semaphore
 * @param type - semaphore type (counting or binary)
 */
#define BEERTOS_SEMAPHORE_LIST()               \
    BEERTOS_SEMAPHORE(SEMAPHORE_UT1,     10U,   SEMAPHORE_TYPE_COUNTING) \
    BEERTOS_SEMAPHORE(SEMAPHORE_UT2,     0U,    SEMAPHORE_TYPE_BINARY)   \
    BEERTOS_SEMAPHORE(SEMAPHORE_TWO,     0U,    SEMAPHORE_TYPE_BINARY)


/*!
 *  @brief Define your alarms here
 *  @note Structure: BEERTOS_ALARM(alarm_id, callback, autostart, default_period, periodic)
 * 
 *  @param alarm_id - alarm id (created in os_alarm_id_t enum), must be unique
 *  @param callback - callback function to be called when alarm expires
 *  @param autostart - if TRUE, alarm will be started automatically after OS initialization
 *  @param default_period - default alarm period in ticks, set only if autostart is TRUE,
 *                          otherwise use os_alarm_start with period parameter
 *  @param periodic - if TRUE, alarm will be periodic, otherwise it will be one-shot,
 *                    set only if autostart is TRUE, otherwise use os_alarm_start with periodic parameter
 */
#define BEERTOS_ALARM_LIST() \
    BEERTOS_ALARM(ALARM_ONE,      alarm1_callback, false, 0U, false) \
    BEERTOS_ALARM(ALARM_TWO,      alarm2_callback, false, 0U, false) \
    BEERTOS_ALARM(ALARM_THREE,    alarm3_callback, false, 0U, false)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

extern void ut_beertos_main_task(void *arg);

extern void ut_task_priority_lowest(void *arg);
extern void ut_task_priority_medium(void *arg);
extern void ut_task_priority_highest(void *arg);

extern void ut_task_mutex_1(void *arg);
extern void ut_task_mutex_2(void *arg);
extern void ut_task_mutex_3(void *arg);

extern void ut_task_msg_1(void *arg);
extern void ut_task_msg_2(void *arg);

extern void alarm1_callback(void);
extern void alarm2_callback(void);
extern void alarm3_callback(void);

#endif /* __BEERTOS_CFG_H__ */