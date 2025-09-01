# `sy_callback.hpp` — Header-only C++11 callback

`sy_callback.hpp` is a **header-only** C++11 library designed to **replace `std::function`** with higher performance and smaller footprint.

---

### Template Code Example

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void compare(int v, int u) {
        if(v == u) {
            std::cout << "match" << std::endl;
            return;
        }
        std::cout << "not match" << std::endl;
        return;
    }

    template<typename V, typename U>
    static auto multi(V v, U u) {
        return v * u;
    }
};

int main() {
    MyClass my_class;

    auto cb_multi = 
        sy_callback::callback<int(int, int)>::make<&MyClass::multi>();
    auto cb_member = 
        sy_callback::callback<void(int, int)>::make<MyClass, &MyClass::compare>(&my_class);
    auto cb_lambda = 
        sy_callback::callback<void()>::make([](){ std::cout << "calling lambda" << std::endl;});

    std::cout << "multiplication value: " << cb_multi(88, 90) << std::endl;
    std::cout << "match? "; cb_member(88, 88);

    cb_lambda();

    return 0;
}
```

---

## 1. Main Component

### `sy_callback::callback<Signature>`

* A class wrapper for callbacks, storing **2 pointers to 2 static functions** and **8 bytes for the object address**.
* Supports:

  * **Static functions**, **member functions**, **global functions**, **capturing lambdas**, **template functors** (non-class templates), or any callable.
  * **Copy** and **move**.
* **Size**:

  * 24 bytes for member and global functions.
  * 24 + sizeof(anything) for arbitrary callables (all objects allocated on the heap, no SBO).

---

## 2. Performance

### 2.1. Call Time (10 million calls)

| Callback type           | N   | `callback` (µs) | `std::function` (µs) | Direct call (µs) | Notes                                              |
| ----------------------- | --- | --------------- | -------------------- | ---------------- | -------------------------------------------------- |
| Small lambda capture    | 10M | 44k–47k         | 82k–83k              | 22k              | Direct lambda call, same object                    |
| Member function pointer | 10M | 41k–42k         | 82k–84k              | 22k              | Template member pointer, inline call               |
| `std::bind`             | 10M | 150k            | 215k                 | 135k–136k        | Lambda wrapping `std::bind` → runtime type-erasure |

### 2.2. Construction & Destruction Time

| Callback type                         | N   | `callback` init+destroy (µs) | `std::function` init+destroy (µs) |
| ------------------------------------- | --- | ---------------------------- | --------------------------------- |
| Small lambda / 1 object               | 10M | 338k                         | 605k                              |
| Large lambda capture (int array 1000) | 10M | 900k                         | 2.49M                             |
| Global function                       | 10M | 280k                         | 1.29M                             |

---

## 3. Machine Code Size

### `sy_callback.hpp`

| Type         | Member same class | Member different class | Global               | Arbitrary callable | Different signature |
| ------------ | ----------------- | ---------------------- | -------------------- | ------------------ | ------------------- |
| Size (bytes) | 1008              | 1312                   | 560 → 32 (with heap) | 654                | 1200                |

### `std::function`

| Type                                      | Size (bytes) |
| ----------------------------------------- | ------------ |
| Member same class (with `std::bind`)      | 160          |
| Member different class (with `std::bind`) | 52,784       |
| Lambda                                    | 39,600       |
| Global                                    | 32           |
| Different signature (with `std::bind`)    | 58,352       |
| Different signature (with lambda)         | 1,136        |
| Different signature (with global)         | 40,272       |

---

## 4. Compile Time (1000 callbacks)

### `sy_callback.hpp`

| Type                                        | Time      | Code size (bytes) |
| ------------------------------------------- | --------- | ----------------- |
| Lambda                                      | \~1.216 s | 1,333,528         |
| Global                                      | \~0.194 s | 907,464           |
| Global (with heap)                          | \~0.157 s | 226,824           |
| Member                                      | \~0.665 s | 1,751,752         |
| Member (with lambda)                        | \~1.070 s | 1,571,576         |
| Member (with std::bind)                     | \~0.725 s | 363,864           |
| Member different class, same signature      | \~0.727 s | 2,106,376         |
| Member different class, different signature | \~2.051 s | 3,893,336         |

### `std::function`

| Type                                        | Time       | Code size (bytes) |
| ------------------------------------------- | ---------- | ----------------- |
| Lambda                                      | \~39.501 s | 30,029,544        |
| Global                                      | \~0.155 s  | 263,384           |
| Member (with `std::bind`)                   | \~0.715 s  | 400,424           |
| Member different class, same signature      | \~87.890 s | 45,620,328        |
| Member different class, different signature | \~92.898 s | 49,862,104        |

---

## 5. Limitations

* Not **thread-safe**.
* Does not support `target<T>()` like `std::function`.
