# `sy_callback.hpp` — Header-only callback library for C++11

---

`sy_callback.hpp` is a **header-only library** that acts as a **drop-in replacement for `std::function`** with **higher performance** and a **smaller footprint**.

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

    sy_callback::callback<int(long, int)> cb_multi = multi; // global function: works because "multi" can be cast

    sy_callback::callback<void(const char*)> cb_anything = 
        [](const char* chars){ std::cout << chars << std::endl; }; // any callable

    if(cb_compare(10, 11)) std::cout << "compare is same" << std::endl;
    else std::cout << "compare is not same" << std::endl;

    std::cout << "multi of 7 and 8: " << cb_multi(7, 8) << std::endl;

    std::cout << "print: "; cb_anything("call lambda\n");

    return 0;
}
```

---

## 1. Architecture

### `sy_callback::callback<Signature>`

A `callback` object consists of three main components:

* **Pointer to object (8 bytes)**: stores the address of the object or `nullptr`.
* **Invoke function (static function pointer)**: calls the function according to the signature.
* **Manager function (static function pointer)**: responsible for **copy/move/destroy** operations.

Internal structure:

```cpp
┌───────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>         │
├───────────────────────────────────────────┤
│ object_ptr : void*                (8 B)   │
│ invoke_fn  : R(*)(void*, Args...) (8 B)   │
│ life_fn    : void*(*)(void*, Op)   (8 B)  │
└───────────────────────────────────────────┘
```

* `invoke_fn` → contains logic to call the function (lambda, global, member, functor).
* `manage_fn` → contains logic to manage the object’s lifetime (copy/destroy).

Base size: **24 bytes** (3 pointers).

For any callable object → memory is allocated on the heap, and `object_ptr` points to it.

---

## 2. Features

* Supports:

  * **Global**, **static**, **member functions**.
  * **Lambdas** (capturing or non-capturing).
  * **Functors** or any callable object.
* Supports **copy / move**.
* Does **not** use SBO (Small Buffer Optimization), keeping the footprint simple.

**Not supported**:

* `target<T>()` like `std::function`.
* Thread-safe moves.

---

## 3. Performance

### 3.1. Call time (10 million calls)

| Callback type              | Direct call (µs) | `sy_callback` (µs) | `std::function` (µs) |
| -------------------------- | ---------------- | ------------------ | -------------------- |
| Small lambda capture       | \~22k            | \~44k–47k          | \~82k–83k            |
| Member function (inline)   | \~22k            | \~41k–42k          | \~82k–84k            |
| Global (inline/non-inline) | \~20k            | \~34k              | \~75k                |
| `std::bind`                | \~135k–136k      | \~150k–151k        | \~215k               |

### 3.2. Construction & destruction time (10 million calls)

| Callback type                     | `sy_callback` (µs) | `std::function` (µs) |
| --------------------------------- | ------------------ | -------------------- |
| Small lambda (1 object)           | \~338k             | \~605k               |
| Large lambda capture (int\[1000]) | \~900k             | \~2.49M              |
| Global function                   | \~280k             | \~1.29M              |

### 3.3. Copy, move, assign (10 million calls)

| Type          | Copy (µs) | Move (µs) | Assign (µs) |
| ------------- | --------- | --------- | ----------- |
| sy\_callback  | \~300K    | \~37K     | \~340K      |
| std::function | \~460K    | \~450K    | \~2M        |

---

## 4. Binary size

### `sy_callback.hpp`

| Type                             | Size (bytes) |
| -------------------------------- | ------------ |
| Member (inline, same class)      | 1008         |
| Member (inline, different class) | 1312         |
| Global (inline)                  | 336          |
| Global (non-inline)              | 32           |
| Any callable                     | 654          |
| std::bind                        | 160          |
| Different signatures             | 1200         |

### `std::function`

| Type                           | Size (bytes) |
| ------------------------------ | ------------ |
| Member (bind, same class)      | 160          |
| Member (bind, different class) | 52,784       |
| Lambda                         | 39,600       |
| Global                         | 32           |
| Different signature (bind)     | 58,352       |
| Different signature (lambda)   | 1,136        |
| Different signature (global)   | 40,272       |

---

## 5. Compile time (1000 callbacks)

### `sy_callback.hpp`

| Type                                                         | Time      | Binary size (bytes) |
| ------------------------------------------------------------ | --------- | ------------------- |
| Lambda                                                       | \~1.216 s | 1,333,528           |
| Global (inline)                                              | \~0.194 s | 683,464             |
| Global (non-inline)                                          | \~0.157 s | 226,824             |
| Member (inline)                                              | \~0.665 s | 1,751,752           |
| Member (lambda)                                              | \~1.070 s | 1,571,576           |
| Member (bind)                                                | \~0.725 s | 363,864             |
| Member (bind) (inline, different class, same signature)      | \~0.727 s | 2,106,376           |
| Member (bind) (inline, different class, different signature) | \~2.051 s | 3,893,336           |

### `std::function`

| Type                                          | Time       | Binary size (bytes) |
| --------------------------------------------- | ---------- | ------------------- |
| Lambda                                        | \~39.501 s | 30,029,544          |
| Global                                        | \~0.155 s  | 263,384             |
| Member (bind)                                 | \~0.715 s  | 400,424             |
| Member (different class, same signature)      | \~87.890 s | 45,620,328          |
| Member (different class, different signature) | \~92.898 s | 49,862,104          |

---

## 6. Example Usage

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void compare(int v, int u) {
        if (v == u) std::cout << "same\n";
        else std::cout << "not same\n";
    }

    template<typename V, typename U>
    static int multi(V v, U u) { return v * u; }
};

template<typename R, typename V, typename U>
static R total(V v, U u) { return v + u; }

void global(int a, int b) {
    std::cout << "global: " << a << "," << b << "\n";
}

struct Functor {
    void operator()(int a, int b) const {
        std::cout << "functor: " << a * b << "\n";
    }
};

int main() {
    MyClass my_class;
    
    sy_callback::callback<void(int,int)> cb_member = 
        sy_callback::callback<void(int,int)>::make<MyClass, &MyClass::compare>(&my_class);
    cb_member(2, 2);

    sy_callback::callback<void(int,int)> cb_global = global;
    cb_global(1, 2);

    sy_callback::callback<int(int,int)> cb_total = total; 
    std::cout << "total: " << cb_total(1, 2) << std::endl;

    sy_callback::callback<int(int,int)> cb_multi_spec = MyClass::multi;
    std::cout << "multi: " << cb_multi_spec(1, 2) << std::endl;

    sy_callback::callback<void(int,int)> cb_lam = [](int a, int b){
        std::cout << "lambda: " << a+b << "\n";
    };
    cb_lam(7, 8);

    int x = 100;
    sy_callback::callback<void(int,int)> cb_cap = [x](int a, int b){
        std::cout << "lambda capture: " << a+b+x << "\n";
    };
    cb_cap(9, 10);

    sy_callback::callback<void(int,int)> cb_functor = Functor{};
    cb_functor(2, 3);

    sy_callback::callback<void(int,int)> cb_copy = cb_functor;
    cb_copy(3, 4);

    sy_callback::callback<void(int,int)> cb_move = std::move(cb_copy);
    cb_move(4, 5);

    cb_move = global;
    cb_move(11, 12);

    cb_move = [](int a, int b){ std::cout << "assign lambda: " << a-b << "\n"; };
    cb_move(20, 10);

    cb_move = Functor{};
    cb_move(6, 7);

    sy_callback::callback<void(int,int)> cb_assign;
    cb_assign = cb_move;
    cb_assign(8, 9);

    cb_assign = std::move(cb_lam);
    cb_assign(10, 11);

    return 0;
}
```
---

Copyright © 2025 ShigamiYune

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this project except in compliance with the License.
You may obtain a copy of the License at:

* [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
