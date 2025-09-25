# Qoraal  
### Embedded Application Framework  
#### ChibiOS • FreeRTOS • ThreadX • POSIX  

---  

## Overview  

**Leveled vs. Layered Programming**—the eternal debate.  

Leveled programming is a structured, top-down hierarchy where each layer is a rung on a ladder, carefully arranged, singular in purpose. 
It's rigid, predictable, and linear.  

Layered programming, on the other hand, is a living, breathing organism. 
It’s about synergy where modules coexist, collaborate, and communicate. 
Layers aren’t stacked in a strict order—they are fluid, dynamic, and adaptable.  

## What’s Qoraal About?  

Qoraal is **layered**—not because it rejects structure, but because it thrives on flexibility. 
It’s built for embedded systems that demand responsiveness, adaptability, and efficiency. 
A framework that moves with your application, rather than forcing your application to conform to it.  

At its core, Qoraal defines an event-driven **Application Framework** designed to handle complexity without the overhead. 
Instead of rigid, monolithic layers, it offers a set of interwoven, 
modular services that operate independently but harmoniously.  

### The Layers of Qoraal  

Each layer is a self-sufficient entity, designed to work in sync while maintaining its own domain:  

- **Qoraal OS Abstraction Layer** – The foundation. It abstracts system calls, threading, and synchronization across a variety of environments—including ChibiOS, FreeRTOS, ThreadX, and POSIX—so you can run your core logic anywhere without rewriting a single line.  
- **Qoraal Service Task System** – A lightweight, priority-driven thread pool that schedules tasks and events dynamically, ensuring efficiency and responsiveness.  
- **Qoraal Service Threading** –  Thread creation and cleanup across multiple architectures. It handles concurrency, letting you focus on building features rather than managing threads.  
- **Qoraal Service Management** – A framework for defining services, starting stopping and keeping services in check, handling dependencies, and ensuring smooth operation.  
- **Qoraal Service Logging** – A flexible logging system designed for embedded environments. It establishes dedicated log channels for debugging, diagnostics, and traceability, delivering the insights you need without sacrificing performance.  
- **Qoraal Shell** Interface is a lightweight, terminal-independent command shell parser designed to execute commands and scripts with built-in support for simple flow control. It’s not tied to any specific terminal environment, making it perfect for embedding into various systems or running in headless mode 
- **Qoraal Service Watchdog Management** – Hardware watchdog management interface to ensure resilience and fault tolerance, so that the system stays alive even when issues arise.  

Qoraal doesn’t impose—it enables. It doesn’t dictate structure—it offers freedom. 
It’s the perfect balance between control and flexibility, letting you build systems that are both **robust and agile**.



### Lets get into it.



---

## Quick Start

The demo application will be compiled using the posix port for evaluation.
Other RTOS suport currently include ChibiOS, FreeRTOS and ThreadX.
1. Open a Codespace or your development environment.
2. Execute the ```build_and_run.bat``` or ```build_and_run.sh``` depending on your environment.
3. in the shell that will open:
   ```bash
   . runme.sh
   ```




## Qoraal OS Abstraction Layer

### Overview

The Qoraal OS Abstraction Layer (`os.h`) provides a unified API for real-time operating systems (RTOS) 
but also runs on standard OS environments. 
It enables seamless execution of Qoraal modules across multiple platforms, including:

- **FreeRTOS** (Popular open-source RTOS)
- **ThreadX** (Azure RTOS, widely used in IoT and embedded systems)
- **ChibiOS** (Lightweight embedded RTOS)
- **Windows** (POSIX-like abstraction)
- **Linux** (Native POSIX threading and synchronization)

By using this module, Qoraal-xxx services can run without modification on different operating systems.

---

### Features

#### 1. **Cross-Platform Threading**
- Supports thread creation, priority management, and thread synchronization.
- Abstracts OS-specific threading mechanisms (e.g., `pthread` on Linux, `xTaskCreate` on FreeRTOS).

#### 2. **Time Management & System Ticks**
- Converts seconds/milliseconds to system ticks with:
  ```c
  #define OS_S2TICKS(sec) ((uint32_t)((sec) * OS_ST_FREQUENCY))
  #define OS_MS2TICKS(msec) ((uint32_t)((msec) * OS_ST_FREQUENCY) / 1000)
  ```
- Provides timestamping utilities for real-time scheduling.

#### 3. **Mutexes and Synchronization Primitives**
- Portable mutex (`os_mutex_create()`), semaphore (`os_sem_create()`), and event handling.
- Binary and counting semaphores for inter-thread communication.

#### 4. **Event Handling**
- Supports both blocking and non-blocking event waiting.
- Works across ISR (Interrupt Service Routine) and thread contexts.
- Example:
  ```c
  p_event_t event;
  os_event_create(&event);
  os_event_wait(&event, 0, EVENT_MASK, 1);
  ```

#### 5. **Timers & Delayed Execution**
- Provides OS-independent software timers (`os_timer_create()`).
- Supports periodic and one-shot timers with precision scheduling.

#### 6. **IRQ Handling & System Locking**
- Identifies if the system is running in an interrupt context (`os_sys_is_irq()`).
- Locks/unlocks critical sections to prevent race conditions (`os_sys_lock()`, `os_sys_unlock()`).

---

### Supported Platforms

| Feature           | Windows | Linux | ChibiOS | FreeRTOS | ThreadX |
|------------------|---------|--------|---------|---------|---------|
| Threads         | ✅ | ✅ | ✅ | ✅ | ✅ |
| Mutexes         | ✅ | ✅  | ✅  | ✅  | ✅  |
| Semaphores      | ✅ | ✅  | ✅  | ✅ | ✅  |
| Events          | ✅  | ✅  | ✅  | ✅  | ✅  |
| Timers          | ✅ | ✅ | ✅  | ✅  | ✅  |
| IRQ Detection   | ❌ No native | ❌ No native | ✅  | ✅  | ✅  |

---

### Example Usage

#### **Creating a Thread**
```c
void my_thread(void *arg) {
    while (1) {
        printf("Running in thread
");
        os_thread_sleep(1000);
    }
}

void start_thread() {
    p_thread_t thread;
    os_thread_create(1024, OS_THREAD_PRIO_5, my_thread, NULL, &thread, "WorkerThread");
}
```

#### **Using Mutexes**
```c
p_mutex_t mutex;

void init_mutex() {
    os_mutex_create(&mutex);
}

void critical_section() {
    os_mutex_lock(&mutex);
    // Perform thread-safe operation
    os_mutex_unlock(&mutex);
}
```

#### **Creating a Timer**
```c
void my_timer_callback(void *arg) {
    printf("Timer expired
");
}

void start_timer() {
    p_timer_t timer;
    os_timer_create(&timer, my_timer_callback, NULL);
    os_timer_set(&timer, OS_S2TICKS(5)); // Fire in 5 seconds
}
```

---

### Why Use This Abstraction Layer?

- **Write Once, Run Anywhere** → The same Qoraal modules work across RTOSes and can be tested on Windows or Linux.
- **Efficient & Lightweight** → Minimal overhead while ensuring compatibility with constrained systems.
- **Scalable** → Ideal for embedded systems and high-performance computing.

This OS abstraction layer is the foundation for all Qoraal-based services, enabling seamless portability and 
real-time performance across platforms.



## Qoraal Service Task System

### Overview

The Qoraal Service Task System provides a structured mechanism for handling events, 
scheduling deferred work, and managing prioritized tasks. It supports execution from interrupt contexts, 
deferring work to scheduled tasks, and leveraging a thread pool for asynchronous operations.

### Use Cases

#### 1. Event-Driven Execution

Events are lightweight signals that notify the system about specific occurrences, 
such as hardware interrupts or software state changes. These events can be used to:

- Trigger immediate actions in response to interrupts or other system events.
- Register multiple event handlers caled in a high priority thread for modular event-based processing.
- Event handlers can defer work to a scheduled task to prevent long execution times in handlers.

##### **Example: Handling an Interrupt**
When an interrupt occurs, the event system (`svc_events.c`) signals an event. The corresponding event handler is executed either immediately or in a deferred context, depending on the OS capabilities.

```c
void my_interrupt_handler(void) {
    svc_events_signal_isr(SVC_EVENTS_TASK);
}
```

#### 2. Deferred Work from Interrupts

Since interrupt handlers must execute quickly, they should avoid long computations or blocking operations. 
The service task system allows scheduling deferred work in a lower priority thread.

- Interrupts can trigger event signals (`svc_events_signal_isr()`).
- Events then schedule tasks (`svc_tasks_schedule()`), which run in a separate execution context.

```c
void my_event_handler(SVC_EVENTS_T id, void *ctx) {
    svc_tasks_schedule(&my_task, my_callback, 0, SERVICE_PRIO_QUEUE2, SVC_TASK_MS2TICKS(100));
}
```

#### 3. Task Scheduling & Thread Pool

Tasks provide a mechanism for executing prioritized work asynchronously. 
The system includes a thread pool where tasks are executed according to their priority.

- **High-priority tasks** execute first.
- **Lower-priority tasks** may be delayed depending on system load.
- Tasks are queued and assigned to worker threads.

##### **Example: Scheduling a Task**
```c
void my_callback(SVC_TASKS_T *task, uintptr_t param, uint32_t reason) {
    // Task execution logic
}

SVC_TASKS_DECL(my_task);

void schedule_task() {
    svc_tasks_schedule(&my_task, my_callback, 0, SERVICE_PRIO_QUEUE1, 0);
}
```

#### 4. Timed Tasks

Tasks can also be scheduled to execute after a delay, making them useful for:

- Periodic execution.
- Delaying non-critical tasks.
- Implementing timeout mechanisms.

##### **Example: Running a Task After a Delay**
```c
svc_tasks_schedule(&delayed_task, my_callback, 0, SERVICE_PRIO_QUEUE3, SVC_TASK_S2TICKS(5));
```
This schedules `my_callback()` to run 5 seconds later.

#### 5. Waitable Tasks

Certain tasks may need to synchronize with other processes or events. 
Waitable tasks block until they are explicitly completed or a timeout occurs.

##### **Example: Waiting for a Task to Complete**
```c
SVC_WAITABLE_TASKS_DECL(my_waitable_task);

svc_tasks_schedule(&my_waitable_task.task, my_callback, 0, SERVICE_PRIO_QUEUE2, SVC_TASK_S2TICKS(5));
svc_tasks_wait(&my_waitable_task.task, SVC_TASK_S2TICKS(10));
```

### Thread Pool Configuration

The system provides a configurable thread pool for executing tasks, with predefined priorities and stack sizes.

```c
#define SVC_TASK_CFG_DEFAULT  \
    {OS_THREAD_PRIO_11, 1024*3, "svc-task0", TIMEOUT_10_SEC},  \
    {OS_THREAD_PRIO_10, 1024*5, "svc-task1", TIMEOUT_10_SEC},  \
    {OS_THREAD_PRIO_5, 1024*5, "svc-task2", TIMEOUT_10_SEC},  \
    {OS_THREAD_PRIO_4, 1024*5, "svc-task3", TIMEOUT_10_SEC},  \
    {OS_THREAD_PRIO_3, 1024*5, "svc-task4", TIMEOUT_30_SEC}
```

### Summary

- **Events** are used to signal and defer work from interrupts.
- **Tasks** provide a prioritized thread pool to schedule tasks for execution on a priority queue.
- **Timed tasks** allow delayed execution of tasks.
- **Waitable tasks** enable synchronization.

This system is designed for real-time applications where efficient task management and prioritization are essential.
