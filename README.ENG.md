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

Here’s the English translation of your text, keeping all the details intact:

---

## 1. Architecture

### `sy_callback::callback<Signature>`

A `callback` object consists of 2 main components:

* **Pointer object (8 bytes):** stores the address of the object or `nullptr`.
* **Invoke function (static function pointer):** calls the function corresponding to the object's signature.
* **Life function (static function pointer):** is the function responsible for **copy / destroy**.
* **Thunk function (static function pointer):** is the function that returns the **Invoke function** or **Life function**.

Internal diagram:

```cpp
┌──────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>            │
├──────────────────────────────────────────────┤  ┌─────────────────────────────────────────────────┐
│ object_ptr : std::uintptr_t          (8 byte)│  │invoke_fn :RETURN (*)(std::uintptr_t, ARGS...)   │  
│ thunk_fn   : std::uintptr_t(*)(Op)   (8 byte)│->│life_fn   :std::uintptr_t (*)(Op, std::uintptr_t)│
└──────────────────────────────────────────────┘  └─────────────────────────────────────────────────┘
```

* `object_ptr` → points to the object’s address.
* `invoke_fn` → embeds the logic (invoke).
* `life_fn` → embeds the logic (copy / destroy).
* `thunk_fn` → returns the address of either the invoke function or the life function.

Basic size: 16 **bytes** (2 pointers).

For any callable → the object is allocated on the heap, and `object_ptr` points to that memory. (If it is a non-capturing lambda, it will be stored as `R(*)(Args...)` in `object_ptr`.)

---

If you want, I can also make a **slightly clearer English diagram** that reads more naturally while keeping the technical meaning exactly the same. Do you want me to do that?

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

Here’s a full English translation of your benchmark tables, keeping all details intact:

---

## 3. Performance

> Note: all values marked with \~ are averages and rounded from multiple compilations with clang++ -O2 on MacOS M1.
> Note: Measured numbers below may vary on different architectures, operating systems, or compilers.

### 3.1. Call time (10 million times)

| Callback type                      | Direct call (s) | `sy_callback` (s) | `std::function` (s) |
| ---------------------------------- | --------------- | ----------------- | ------------------- |
| Member function (embedded)         | \~0.02 s        | \~0.048 s         | no similar method   |
| Member function (lambda)           | \~0.026 s       | \~0.05 s          | \~0.085 s           |
| Member function (`std::bind`)      | \~0.70 s        | \~0.71 s          | \~0.135 s           |
| Lambda without capture             | \~0.02 s        | \~0.041 s         | \~0.073 s           |
| Lambda with small capture          | \~0.02 s        | \~0.041 s         | \~0.073 s           |
| Global (embedded and non-embedded) | \~0.016 s       | \~0.041 s         | \~0.075 s           |

---

### 3.2. Initialization + destruction time (10 million times)

| Callback type                          | `sy_callback` (s) | `std::function` (s) |
| -------------------------------------- | ----------------- | ------------------- |
| Lambda without capture                 | \~0.08 s          | \~0.51 s            |
| Lambda with small capture (4 bytes)    | \~0.08 s          | \~0.51 s            |
| Lambda with large capture (1024 bytes) | \~0.1 s           | \~1.5 s             |
| Global function                        | \~0.06 s          | \~0.53 s            |
| Member function                        | \~0.07 s          | \~0.54 s            |

---

### 3.3. Copy, move, and assign time (10 million times)

| Type            | Copy (s) | Move (s) | Assign (s) |
| --------------- | -------- | --------- | ----------- |
| `sy_callback`   | \~0.31 s | \~0.027 s | \~0.26 s    |
| `std::function` | \~0.46 s | \~0.37 s  | \~1.9 s     |

---

## 4. Memory usage (64-bit)

| Size of callable (byte) | `std::function` total (byte)  | `sy_callback` total (byte) |
| ----------------------- | ----------------------------- | -------------------------- |
| 1                       | 32                            | 17                         |
| 8                       | 32                            | 24                         |
| 16                      | 32                            | 32                         |
| 24                      | 64                            | 40                         |
| 32                      | 72                            | 48                         |
| 48                      | 88                            | 56                         |
| 56                      | 96                            | 74                         |
| 64                      | 104                           | 84                         |
| …                       | 32 + callable size + 8 (vptr) | 16 + callable size         |

---

## 5. Compile time (1000 callbacks)

### Unit: total size of generated machine code (bytes) / compile time, `std::function` and `sy_callback` values have subtracted the cost of base

| Type                                                                                         | base               | `std::function`         | `sy_callback`         |
| -------------------------------------------------------------------------------------------- | ------------------ | ----------------------- | --------------------- |
| Global same signature                                                                        | 119,416 / \~0.5 s  | \~125,000 / \~0.15 s    | \~86,000 / \~0.19 s   |
| Global different signature                                                                   | 373,192 / \~0.5 s  | \~32,000,000 / \~48.5 s | \~3,000,000 / \~2 s   |
| Bind member same signature, same class                                                       | 321,496 / \~0.7 s  | \~180,000 / 0.2 s       | \~103,000 / \~0.2 s   |
| Bind member different signature, different class                                             | 6,957,208 / \~12 s | \~41,000,000 / \~248 s  | \~4,700,000 / \~3.5 s |
| Lambda without capture, same signature, different instances                                  | 544,008 / \~0.75 s | \~30,000,000 / \~41 s   | \~770,000 / \~0.5 s   |
| Lambda with capture, different signature, different instances (used to call member function) | 543,368 / \~0.8 s  | \~32,000,000 / \~60 s   | \~2,800,000 / \~2.2 s |
| Embedded member same class, same signature                                                   | 235,912 / 0.5 s    | no similar method       | \~1,400,000 / \~0.7 s |
| Embedded member different class, different signature                                         | 539,992 / \~0.8 s  | no similar method       | \~3,400,000 / \~2.2 s |

---

Mình đã giữ nguyên mọi chi tiết, dấu `~`, chữ “embedded”, và ghi chú về đơn vị cũng như trừ base.

Nếu muốn, mình có thể làm **phiên bản biểu đồ trực quan bằng tiếng Anh** từ bảng này để thấy sự khác biệt rõ ràng giữa `sy_callback` và `std::function`.


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
