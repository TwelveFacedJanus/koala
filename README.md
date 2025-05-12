<div align='center'>
  <img src="https://github.com/TwelveFacedJanus/koala/blob/main/images/Dark%20Theme.svg#gh-dark-mode-only">
  <img src="https://github.com/TwelveFacedJanus/koala/blob/main/images/White%20Theme.svg#gh-light-mode-only">
</div>


# Koala

Koala is a lightweight C/C++ library that provides asynchronous event loop functionality with a clean API for both C and C++ developers. The library enables developers to create responsive applications that can handle concurrent operations efficiently.

## Overview

The core of Koala is an asynchronous event loop implementation that:

- Manages a thread pool to execute tasks concurrently
- Provides a future/promise-like mechanism for handling asynchronous operations
- Supports task cancellation and timeout-based operations
- Offers a clean API that works in both C and C++ environments

The implementation follows a simple producer-consumer pattern where:
1. Client code submits functions to be executed asynchronously
2. Worker threads from the thread pool pick up tasks from a queue
3. Results are delivered back through a promise-like mechanism

## Core Components

### Types System (types.h)

The types.h header provides core type definitions:

- Generic types for passing any kind of data safely (similar to `void*` but with type safety)
- Result type (similar to Rust's Result) for handling success/error cases elegantly
- Function pipeline for chaining operations

```c
// Generic type example
Generic g;
int x = 42;
GENERIC_SET(g, &x);
int* px = GENERIC_GET(int*, g);

// Result type example
Result res;
res.is_ok = 1;
GENERIC_SET(res.ok, "Success");
```

### Async System (async.h)

The async.h header provides the event loop implementation:

- `AsyncEventLoop`: Manages worker threads and task distribution
- `AsyncTask`: Represents an asynchronous operation (similar to a Future/Promise)
- Helper functions for running, awaiting, and managing async operations

## Usage Examples

### Basic Usage in C

```c
#include "types.h"
#include "async.h"

Result my_func(FunctionParams* params) {
    Result res;
    res.is_ok = 1;
    GENERIC_SET(res.ok, "Hello from async!");
    return res;
}

int main() {
    // Initialize the event loop
    AsyncEventLoop* loop = async_event_loop_init();
    
    // Create a function pipeline
    FunctionPipeline fp = { my_func, NULL };
    FunctionParams params = { 0, NULL };
    
    // Run the function asynchronously
    AsyncTask* task = async_run(loop, &fp, &params);
    
    // Wait for the result
    Result r = async_await(task);
    
    // Check the result
    if (r.is_ok) printf("%s\n", (char*)GENERIC_GET(char*, r.ok));
    
    // Clean up
    async_event_loop_stop(loop);
    return 0;
}
```

### C++ Usage

```cpp
#include "types.h"
#include "async.h"
#include <iostream>

Result my_func(FunctionParams* params) {
    Result res;
    res.is_ok = true;
    res.ok = Generic<const char*>("Hello from async!");
    return res;
}

int main() {
    // Initialize the event loop
    AsyncEventLoop* loop = async_event_loop_init();
    
    // Create a function pipeline
    FunctionPipeline fp = { my_func, nullptr };
    FunctionParams params = { 0, nullptr };
    
    // Run the function asynchronously
    AsyncTask* task = async_run(loop, &fp, &params);
    
    // Wait for the result
    Result r = async_await(task);
    
    // Check the result
    if (r.is_ok) std::cout << static_cast<const char*>(r.ok.value) << std::endl;
    
    // Clean up
    async_event_loop_stop(loop);
    return 0;
}
```

### Timeout Example

```c
// Run a function with a timeout
AsyncTask* task = async_run(loop, &fp, &params);
Result r = async_await_timeout(task, 1000); // 1 second timeout

if (r.is_ok) {
    // Task completed successfully
} else {
    // Task timed out or failed
}
```

## Building and Installation

To use the Koala library in your project:

1. Copy the core directory to your project
2. Include the necessary headers in your source files
3. Compile the source files with your project

### Compilation Example

```bash
gcc -o myapp myapp.c koala/core/async.c koala/core/types.c -lpthread
```

## License

Koala is released under the BSD License. See the LICENSE file for details.

<div align='center'>
  <img src="https://github.com/TwelveFacedJanus/koala/blob/main/images/Dark%20Theme.png#gh-dark-mode-only" width = "100px">
  <img src="https://github.com/TwelveFacedJanus/koala/blob/main/images/White%20Theme.png#gh-light-mode-only" width = "100px">
</div>

