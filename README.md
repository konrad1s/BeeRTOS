# BeeRTOS

BeeRTOS is a lightweight, efficient, and scalable real-time operating system (RTOS) specifically designed for embedded systems. With a focus on simplicity, low memory consumption, and speed of operation, BeeRTOS provides a robust solution for managing concurrent tasks in microcontroller-based applications. It provides multitasking, synchronization primitives, inter-task communication, and timing services, all configurable to fit specific application requirements.

## Key Features

- **Simplicity:** BeeRTOS offers an intuitive API and straightforward task management, making it accessible for developers of all experience levels.
- **Low Memory Consumption:** Optimized to require minimal memory resources, ensuring that even devices with limited RAM can run efficiently.
- **Speed of Operation:** Designed for fast context switching and efficient task scheduling, maximizing system responsiveness and task throughput.
- **Preemptive Multitasking:** Supports concurrent execution of multiple tasks, leveraging CPU resources effectively.
- **Modular Design:** Easily extendable and adaptable to various hardware platforms, allowing for seamless integration with different microcontroller architectures.
- **Inter-Task Communication**: Use message queues and signals for communication between tasks.
- **Synchronization Primitives**: Mutexes with priority ceiling protocol and semaphores for task synchronization.
- **Timing Services**: Utilize alarms for scheduling actions at specific times or intervals.


## Configuration Overview

BeeRTOS's behavior and capabilities can be tailored through the `BeeRTOS_cfg.h` configuration header. This file allows for the enabling or disabling of OS modules, the configuration of system tasks, and the setup of synchronization primitives and communication mechanisms, all without the need for dynamic memory allocation.

### Enabling OS Features

You can enable or disable specific OS modules to optimize the runtime environment according to your project's needs. This modular approach ensures that you only include the necessary components, contributing to a smaller memory footprint.
- **Alarm Module:** Allows for the scheduling of tasks or callbacks at specific intervals or points in time.
- **Message Module:** Enables inter-task communication through messages, facilitating data exchange between tasks.
- **Queue Module:** Similar to messages but more flexible, queues support FIFO communication patterns with variable data types.
- **Mutex Module:** Provides mutual exclusion capabilities to prevent simultaneous access to shared resources, with support for priority inheritance to avoid priority inversion problems.
- **Semaphore Module:** Offers a mechanism for synchronizing tasks, including binary and counting semaphores for various use cases.

```c
#define BEERTOS_ALARM_MODULE_EN     (true)
#define BEERTOS_MESSAGE_MODULE_EN   (true)
#define BEERTOS_QUEUE_MODULE_EN     (true)
#define BEERTOS_MUTEX_MODULE_EN     (true)
#define BEERTOS_SEMAPHORE_MODULE_EN (true)
```

### System Task Configuration
The *BEERTOS_PRIORITY_LIST()* macro is used to define tasks, mutexes and alarams.

The priority of tasks is implicitly defined by their order in the *BEERTOS_PRIORITY_LIST()* macro. The first task listed has the highest priority, and subsequent tasks have decreasing priorities. This ordering determines the scheduling behavior of the system, with higher priority tasks preempting lower priority ones.

Task, mutex, and alarm IDs are automatically generated as enum types by BeeRTOS based on their definitions in the BEERTOS_PRIORITY_LIST() macro. This automatic ID generation simplifies the process of referring to tasks and synchronization primitives throughout the application code.

#### Task configuration
Each task is defined using the macro:
```c
BEERTOS_TASK(task_id, function, stacksize, autostart, task_arg)
```
The structure of a task definition is as follows:

- **task_id:** A unique identifier for the task. This ID is used internally by BeeRTOS for task management and scheduling.
- **function:** The main function that the task will execute. This function must adhere to a specific signature defined by BeeRTOS.
- **stacksize:** The size of the stack allocated for the task. Stack size should be carefully chosen based on the task's needs to avoid stack overflows while minimizing memory usage.
- **autostart:** A boolean value indicating whether the task should start automatically upon system initialization (true) or if it should be started manually at a later time (false).
- **task_arg:** A pointer to any arguments that should be passed to the task function. This allows for flexible task configuration and initialization.

#### Mutex configuration
The priority ceiling mechanism in BeeRTOS requires the specification of mutex priorities. This is essential for preventing priority inversion problems, where a lower priority task holds a mutex needed by a higher priority task. The priority ceiling protocol elevates the priority of the task holding the mutex to the highest priority level of any task that may use the mutex, ensuring timely mutex release.

The *BEERTOS_MUTEX()* macro is used similarly for mutexes configuration. 

```c
BEERTOS_MUTEX(mutex_id, initial_count)
```

- **mutex_id:** A unique identifier for the mutex.
- **initial_count:** The initial count for the mutex. For binary mutexes, this is usually set to 0 or 1.

#### Alarm configuration
Alarm tasks are specialized tasks used for timing and scheduling purposes. They can be used to trigger actions at specific intervals or after a certain amount of time has elapsed. 

Alarm tasks are configured using the *BEERTOS_ALARM_TASK()* macro:

```c
BEERTOS_ALARM_TASK(task_id, stacksize)
```

- **task_id:** A unique identifier for the alarm task.
- **stacksize:** The size of the stack allocated for the alarm task.

### Inter-task communication mechanisms configuration
