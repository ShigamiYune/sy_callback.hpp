# `sy_callback.hpp` — Header-only callback library for C++11

---

`sy_callback.hpp` is a **header-only library** that replaces `std::function` with **higher performance** and a **smaller footprint**.

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
        sy_callback::callback<bool(int, int)>::make<MyClass, &MyClass::compare>(&my_class); // member

    sy_callback::callback<int(long, int)> cb_multi = multi; // global: works because "multi" has a cast in operator

    sy_callback::callback<void(const char*)> cb_anything = 
        [](const char* chars){ std::cout << chars << std::endl; }; // any callable

    if (cb_compare(10, 11)) std::cout << "compare is same" << std::endl;
    else std::cout << "compare not same" << std::endl;

    std::cout << "multi of 7 and 8: " << cb_multi(7, 8) << std::endl;

    std::cout << "print: "; cb_anything("call a lambda\n");

    return 0;
}
```

---

## 1. Architecture

### `sy_callback::callback<Signature>`

A `callback` object consists of 3 main components:

* **Pointer object (8 bytes)**: stores the address of the object or `nullptr`.
* **Invoke function (static function pointer)**: invokes the callable with the corresponding signature.
* **Life function (static function pointer)**: responsible for **copy / destroy** operations.

Internal layout:

```cpp
┌──────────────────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>                        │
├──────────────────────────────────────────────────────────┤
│ object_ptr : std::uinptr_t                       (8 byte)│
│ invoke_fn  : R(*)(std::uinptr_t, Args...)        (8 byte)│
│ life_fn    : std::uinptr_t(*)(Op, std::uinptr_t) (8 byte)│
└──────────────────────────────────────────────────────────┘
```

* `invoke_fn` → embeds the invocation logic (lambda, global, member, functor).
* `life_fn` → embeds lifecycle management logic (copy / destroy).

Basic size: **24 bytes** (3 pointers).

For any callable → the object is allocated on the heap, with `object_ptr` pointing to that memory.
(If it’s a captureless lambda, it is stored as `R(*)(Args...)` in `object_ptr`).

---

## 2. Features

* Supports:

  * **Global**, **static**, and **member** functions.
  * **Lambdas** (with or without captures).
  * **Functors** or any callable object.
  * Template functions → callback will automatically cast the template function to the proper type.
* Allows **copy / move**.
* No SBO (Small Buffer Optimization) → simplifies the footprint.
* Provides `target<T>()` similar to `std::function`.

**Not supported**:

* Multi-thread-safe **move and destroy**.
  (Single-threaded move/destroy is safe; multi-threaded copy is still safe).

---

## 3. Performance

### 3.1 Invocation time (10 million calls)

| Callback type                    | Direct call (µs) | `sy_callback` (µs) | `std::function` (µs) |
| -------------------------------- | ---------------- | ------------------ | -------------------- |
| Small capture lambda             | 22k              | 44k–47k            | 82k–83k              |
| Member function (embedded)       | 22k              | 41k–42k            | 82k–84k              |
| Global (embedded / non-embedded) | 20k              | 34k                | 75k                  |
| `std::bind`                      | 135k–136k        | 150k–151k          | 215k                 |

---

### 3.2 Construction & destruction time (10 million ops)

| Callback type                     | `sy_callback` (µs) | `std::function` (µs) |
| --------------------------------- | ------------------ | -------------------- |
| Small lambda (1 object)           | 338k               | 605k                 |
| Large capture lambda (int\[1000]) | 900k               | 2.49M                |
| Global function                   | 280k               | 1.29M                |

---

### 3.3 Copy, move, and assign time (10 million ops)

| Type          | Copy (µs) | Move (µs) | Assign (µs) |
| ------------- | --------- | --------- | ----------- |
| sy\_callback  | \~300k    | \~37k     | \~340k      |
| std::function | \~460k    | \~450k    | \~2M        |

---

## 4. Memory usage (64-bit)

| Lambda size (bytes) | std::function total (bytes) | sy\_callback total (bytes) |
| ------------------- | --------------------------- | -------------------------- |
| 1                   | 32                          | 25                         |
| 8                   | 32                          | 32                         |
| 16                  | 32                          | 40                         |
| 24                  | 64                          | 48                         |
| 32                  | 72                          | 56                         |
| 48                  | 88                          | 72                         |
| 56                  | 96                          | 80                         |
| 64                  | 104                         | 88                         |
| …                   | 32 + Lambda size + 8 (vptr) | 24 + Lambda size           |

---

## 5. Binary size

### `sy_callback.hpp`

| Type                              | Size (bytes) |
| --------------------------------- | ------------ |
| Embedded member (same class)      | 1008         |
| Embedded member (different class) | 1312         |
| Global (embedded)                 | 336          |
| Global (non-embedded)             | 32           |
| Any callable                      | 654          |
| std::bind                         | 160          |
| Different signatures              | 1200         |

### `std::function`

| Type                           | Size (bytes) |
| ------------------------------ | ------------ |
| Member (same class, bind)      | 160          |
| Member (different class, bind) | 52,784       |
| Lambda                         | 39,600       |
| Global                         | 32           |
| Different signatures (bind)    | 58,352       |
| Different signatures (lambda)  | 1,136        |
| Different signatures (global)  | 40,272       |

---

## 6. Compilation time (1000 callbacks)

### `sy_callback.hpp`

| Type                                          | Time      | Binary size (bytes) |
| --------------------------------------------- | --------- | ------------------- |
| Lambda                                        | \~1.216 s | 1,333,528           |
| Global (embedded)                             | \~0.194 s | 683,464             |
| Global (non-embedded)                         | \~0.157 s | 226,824             |
| Member (embedded)                             | \~0.665 s | 1,751,752           |
| Member (lambda)                               | \~1.070 s | 1,571,576           |
| Member (bind)                                 | \~0.725 s | 363,864             |
| Member (different class, same signature)      | \~0.727 s | 2,106,376           |
| Member (different class, different signature) | \~2.051 s | 3,893,336           |

### `std::function`

| Type                                          | Time       | Binary size (bytes) |
| --------------------------------------------- | ---------- | ------------------- |
| Lambda                                        | \~39.501 s | 30,029,544          |
| Global                                        | \~0.155 s  | 263,384             |
| Member (bind)                                 | \~0.715 s  | 400,424             |
| Member (different class, same signature)      | \~87.890 s | 45,620,328          |
| Member (different class, different signature) | \~92.898 s | 49,862,104          |

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
