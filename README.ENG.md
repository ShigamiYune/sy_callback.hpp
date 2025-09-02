# `sy_callback.hpp` — Header-only C++11 callback

---

`sy_callback.hpp` is a **header-only library** that replaces `std::function`, with **higher performance** and **smaller footprint**.

---
## Quick start
```
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
        sy_callback::callback<bool(int, int)>::make<MyClass, &MyClass::compare>(&my_class); // membsr

    sy_callback::callback<int(long, int)> cb_multi = multi; // global : that work because "multi" has cast in operator

    sy_callback::callback<void(const char*)> cb_anything = 
        [](const char* chars){ std::cout << chars << std::endl; }; // anything callable

    if(cb_compare(10, 11)) std::cout << "compare is same" << std::endl;
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

* **Pointer object (8 bytes)**: stores the object address or nullptr.
* **Invoke function (static function pointer)**: invokes the function matching the signature.
* **Manager function (static function pointer)**: responsible for **copy / move / destroy** of the object.

Internal layout:

```cpp
┌───────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>         │
├───────────────────────────────────────────┤
│ object_ptr : void*                (8 byte)│
│ invoke_fn  : R(*)(void*, Args...) (8 byte)│
│ life_fn    : void(*)(void*, Op)   (8 byte)│
└───────────────────────────────────────────┘
```

* `invoke_fn` → embeds the logic to invoke the function (lambda, global, member, functor).
* `life_fn` → embeds lifecycle management logic (copy / destroy).

Base size: **24 bytes** (3 pointers).

For arbitrary callables → the object is allocated on the heap, and `object_ptr` points to that memory.

---

## 2. Features

* Supports:

  * **Global**, **static**, and **member** functions.
  * **Lambdas** (with or without captures).
  * **Functors** or any callable object.
* Allows **copy / move**.
* No SBO (Small Buffer Optimization), simplifying the footprint.

**Not supported**:

* `target<T>()` like `std::function`.
* Thread-safe move.

---

## 3. Performance

### 3.1. Call time (10 million calls)

| Callback type             | N   | `sy_callback` (µs) | `std::function` (µs) | Direct call (µs) |
| ------------------------- | --- | ------------------ | -------------------- | ---------------- |
| Small lambda with capture | 10M | 44k–47k            | 82k–83k              | 22k              |
| Member function           | 10M | 41k–42k            | 82k–84k              | 22k              |
| `std::bind`               | 10M | 150k               | 215k                 | 135k–136k        |

### 3.2. Initialization & destruction time (10 million calls)

| Callback type                     | `sy_callback` (µs) | `std::function` (µs) |
| --------------------------------- | ------------------ | -------------------- |
| Small lambda (1 object)           | 338k               | 605k                 |
| Large capture lambda (int\[1000]) | 900k               | 2.49M                |
| Global function                   | 280k               | 1.29M                |

---

## 4. Machine code size

### `sy_callback.hpp`

| Type                    | Size (bytes)    |
| ----------------------- | --------------- |
| Member, same class      | 1008            |
| Member, different class | 1312            |
| Global                  | 560 → 32 (heap) |
| Arbitrary callable      | 654             |
| std::bind               | 160             |
| Different signatures    | 1200            |

### `std::function`

| Type                           | Size (bytes) |
| ------------------------------ | ------------ |
| Member, same class (bind)      | 160          |
| Member, different class (bind) | 52,784       |
| Lambda                         | 39,600       |
| Global                         | 32           |
| Different signatures (bind)    | 58,352       |
| Different signatures (lambda)  | 1,136        |
| Different signatures (global)  | 40,272       |

---

## 5. Compilation time (1000 callbacks)

### `sy_callback.hpp`

| Type                                         | Time      | Code size (bytes) |
| -------------------------------------------- | --------- | ----------------- |
| Lambda                                       | \~1.216 s | 1,333,528         |
| Global                                       | \~0.194 s | 907,464           |
| Global (heap)                                | \~0.157 s | 226,824           |
| Member                                       | \~0.665 s | 1,751,752         |
| Member (lambda)                              | \~1.070 s | 1,571,576         |
| Member (bind)                                | \~0.725 s | 363,864           |
| Member, different class, same signature      | \~0.727 s | 2,106,376         |
| Member, different class, different signature | \~2.051 s | 3,893,336         |

### `std::function`

| Type                                         | Time       | Code size (bytes) |
| -------------------------------------------- | ---------- | ----------------- |
| Lambda                                       | \~39.501 s | 30,029,544        |
| Global                                       | \~0.155 s  | 263,384           |
| Member (bind)                                | \~0.715 s  | 400,424           |
| Member, different class, same signature      | \~87.890 s | 45,620,328        |
| Member, different class, different signature | \~92.898 s | 49,862,104        |

---

## 6. Usage example

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void compare(int v, int u) {
        if (v == u) {
            std::cout << "same\n";
        } else {
            std::cout << "not same\n";
        }
    }

    template<typename V, typename U>
    static int multi(V v, U u) {
        return v * u;
    }
};

template<typename V, typename U>
static auto multi(V v, U u) { return v + u; }

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

    sy_callback::callback<int(int,int)> cb_multi = multi; 
    cb_multi(3, 4);

    sy_callback::callback<int(int,int)> cb_multi_spec = multi;
    cb_multi_spec(5, 6);

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
