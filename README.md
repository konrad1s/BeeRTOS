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

Enable or disable various OS modules according to your needs:

```c
#define BEERTOS_ALARM_MODULE_EN     (true)
#define BEERTOS_MESSAGE_MODULE_EN   (true)
#define BEERTOS_QUEUE_MODULE_EN     (true)
#define BEERTOS_MUTEX_MODULE_EN     (true)
#define BEERTOS_SEMAPHORE_MODULE_EN (true)
```
