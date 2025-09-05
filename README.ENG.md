# `sy_callback.hpp` — Header-only callback library for C++11

---

`sy_callback.hpp` is a **header-only library** designed to replace `std::function` in **high-performance, embedded environments**.

---

## Quick Start

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    bool compare(int v, int u) {
        return v == u;
    }
};

template<typename R, typename V, typename U>
static R multi(V v, U u) { return v * u; }

int main() {
    MyClass my_class;

    sy_callback::callback<bool(int, int)> cb_compare = 
        sy_callback::callback<bool(int, int)>::make<MyClass, &MyClass::compare>(&my_class); // member function

    sy_callback::callback<int(long, int)> cb_multi = multi; // global function

    sy_callback::callback<void(const char*)> cb_anything = 
        [](const char* chars){ std::cout << chars << std::endl; }; // any callable

    if(cb_compare(10, 11)) std::cout << "compare is same" << std::endl;
    else std::cout << "compare not same" << std::endl;

    std::cout << "multi of 7 and 8: " << cb_multi(7, 8) << std::endl;

    std::cout << "print: "; cb_anything("call a lambda\n");

    return 0;
}
```

---

Sure! Dưới đây là bản dịch tiếng Anh giữ nguyên ý nghĩa kỹ thuật:

---

## 1. Architecture

### `sy_callback::callback<Signature>`

A `callback` object consists of **two main components**:

* **Object pointer (8 bytes):** stores the address of the object or `nullptr`.
* **Invoke function (static function pointer):** calls the function matching the signature and is responsible for **copy / destroy / compare** operations on the object.

Internal layout:

```cpp
┌───────────────────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>                         │
├───────────────────────────────────────────────────────────┤
│ object_ptr : std::uintptr_t                      (8 bytes)│
│ invoke_fn  : union(*)(Op, std::uintptr_t, args*) (8 bytes)│
└───────────────────────────────────────────────────────────┘
```

* `invoke_fn` → contains logic for **invoke, copy, destroy, compare**.
* `life_fn` → contains logic for **lifecycle management** (copy / destroy / compare).

**Base size:** 16 **bytes** (2 pointers).

For any callable object → the object is **heap-allocated**, and `object_ptr` points to that memory. (If it is a **lambda without capture**, it is stored as `R(*)(Args...)` directly in `object_ptr`).

---

## 2. Features

* Supports:

  * **Global**, **static**, **member** functions.
  * **Lambda** (capturing or non-capturing).
  * **Functor** or any callable object.
  * Template functions are automatically cast to the appropriate callback type.
* Supports **copy / move**.
* No SBO (Small Buffer Optimization), simplifying footprint.
* Provides `target<T>()` similar to `std::function`.

**Not supported**:

* Multi-thread safe "move and destroy" (safe to move/destroy on a single thread, copy on multiple threads is safe).

---

## 3. Performance

> Note: All values marked with \~ are averages and rounded from multiple compilations using clang++ -O2 on MacOS M1.
> Note: Measured numbers may vary on different architectures, operating systems, or compilers.

### 3.1. Call Time (10 million calls)

| Callback Type                         | Direct Call (s) | `sy_callback` (s) | `std::function` (s) |
| ------------------------------------- | --------------- | ----------------- | ------------------- |
| Member function (inline)              | \~0.02 s        | \~0.048 s         | not applicable      |
| Member function (lambda)              | \~0.026 s       | \~0.05 s          | \~0.085 s           |
| Member function (`std::bind`)         | \~0.70 s        | \~0.101 s         | \~0.135 s           |
| Lambda without capture                | \~0.02 s        | \~0.041 s         | \~0.073 s           |
| Lambda with small capture             | \~0.02 s        | \~0.041 s         | \~0.073 s           |
| Global function (inline & non-inline) | \~0.016 s       | \~0.041 s         | \~0.075 s           |

### 3.2. Construction & Destruction Time (10 million times)

| Callback Type                          | `sy_callback` (s) | `std::function` (s) |
| -------------------------------------- | ----------------- | ------------------- |
| Lambda without capture                 | \~0.08 s          | \~0.51 s            |
| Lambda with small capture (4 bytes)    | \~0.08 s          | \~0.51 s            |
| Lambda with large capture (1024 bytes) | \~0.1 s           | \~1.5 s             |
| Global function                        | \~0.06 s          | \~0.53 s            |
| Member function                        | \~0.07 s          | \~0.54 s            |

---

### 3.3 Copy, move, assign time (10 million)

| Type          | copy (s) | move (µs) | assign (µs) |
| ------------- | -------- | --------- | ----------- |
| sy\_callback  | \~0.3    | \~0.037   | \~0.340     |
| std::function | \~0.46   | \~0.45    | \~2         |

---

## 4. Memory usage ( 64 bit )

| size of callable (byte) | std::function total (byte) | sy_callback total (byte) |
| --- | --- | --- |
| 1 | 32 | 17 |
| 8 | 32 | 24 |
| 16 | 32 | 32 |
| 24 | 64 | 40 |
| 32 | 72 | 48 |
| 48 | 88 | 56 |
| 56 | 96 | 74 |
| 64 | 104 | 84 |
| … | 32 + callable size + 8 (vptr) | 16 + callable size |

---

## 5. Machine code size (Codegen)

### 5.1 Base + per-signature

| Library           | Base overhead (bytes) | Per-signature overhead (bytes) | Total codegen = Base + body × n |
| ----------------- | --------------------- | ------------------------------ | ------------------------------- |
| **sy\_callback**  | \~1,700               | \~1,000                        | 1,700 + (body × n)              |
| **std::function** | \~140                 | \~1,000                        | 140 + (body × n)                |

### 5.2 Callback type-specific (body only)

#### 5.2.1 Embedded

| Callback type                                  | sy\_callback (bytes) | std::function (bytes) |
| ---------------------------------------------- | -------------------- | --------------------- |
| Same class, same signature, embedded           | \~900                | N/A                   |
| Different class, same signature, embedded      | \~900                | N/A                   |
| Different class, different signature, embedded | \~2,300              | N/A                   |
| Same class, different signature, embedded      | \~2,300              | N/A                   |
| Global/Static same signature, embedded         | \~300                | N/A                   |
| Global/Static different signature, embedded    | \~1,800              | N/A                   |

#### 5.2.2 Bind

| Callback type                              | sy\_callback (bytes) | std::function (bytes) |
| ------------------------------------------ | -------------------- | --------------------- |
| Same class, same signature, bind           | 0                    | 0                     |
| Different class, same signature, bind      | \~1,000              | \~28,000              |
| Same class, different signature, bind      | \~7,000              | \~42,000              |
| Different class, different signature, bind | \~7,000              | \~42,000              |

#### 5.2.3 Lambda

| Callback type                                | sy\_callback (bytes) | std::function (bytes) |
| -------------------------------------------- | -------------------- | --------------------- |
| Non-capture lambda, same signature           | \~500                | \~30,000              |
| Non-capture lambda, different signature      | \~1,800              | \~32,000              |
| Capture lambda 8 bytes, same signature       | \~600                | \~30,000              |
| Capture lambda 24 bytes, same signature      | \~600                | \~32,000              |
| Capture lambda 8 bytes, different signature  | \~1,400              | \~32,000              |
| Capture lambda 24 bytes, different signature | \~1,400              | \~34,000              |

#### 5.2.4 Global / Static

| Callback type       | sy\_callback (bytes)              | std::function (bytes)             |
| ------------------- | --------------------------------- | --------------------------------- |
| Same signature      | 96 `first time, 0 for subsequent` | 32 `first time, 0 for subsequent` |
| Different signature | \~1,600                           | \~31,000                          |

---

## 6. Compile time (1,000 callbacks)

### `sy_callback.hpp`

| Type                                        | Time      | Code size (bytes) |
| ------------------------------------------- | --------- | ----------------- |
| Lambda                                      | \~1.216 s | \~1,334,000       |
| Global (embedded)                           | \~0.194 s | \~683,000         |
| Global (non-embedded)                       | \~0.157 s | \~227,000         |
| Member (embedded)                           | \~0.665 s | \~1,752,000       |
| Member (lambda)                             | \~1.070 s | \~1,572,000       |
| Member (bind)                               | \~0.725 s | \~364,000         |
| Member different class, same signature      | \~0.727 s | \~2,106,000       |
| Member different class, different signature | \~2.051 s | \~3,893,000       |

### `std::function`

| Type                                        | Time       | Code size (bytes) |
| ------------------------------------------- | ---------- | ----------------- |
| Lambda                                      | \~39.501 s | \~30,023,000      |
| Global                                      | \~0.155 s  | 263,000           |
| Member (bind)                               | \~0.715 s  | 400,000           |
| Member different class, same signature      | \~87.890 s | \~45,620,000      |
| Member different class, different signature | \~92.898 s | \~49,862,000      |

---

## [Read Document and API in here:](https://github.com/ShigamiYune/sy_callback.hpp/blob/main/DOCUMENT.ENG.md)

---

Copyright © 2025 ShigamiYune

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this project except in compliance with the License.
You may obtain a copy of the License at:

* [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
