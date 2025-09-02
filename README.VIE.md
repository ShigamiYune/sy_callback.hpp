# `sy_callback.hpp` — Thư viện callback header-only cho C++11

---

`sy_callback.hpp` là một **thư viện header-only** dùng như **thay thế trực tiếp cho `std::function`** với **hiệu năng cao hơn** và **footprint nhỏ hơn**.

---

## Bắt đầu nhanh

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
        sy_callback::callback<bool(int, int)>::make<MyClass, &MyClass::compare>(&my_class); // hàm thành viên

    sy_callback::callback<int(long, int)> cb_multi = multi; // hàm global: hoạt động vì "multi" có thể cast

    sy_callback::callback<void(const char*)> cb_anything = 
        [](const char* chars){ std::cout << chars << std::endl; }; // bất kỳ callable nào

    if(cb_compare(10, 11)) std::cout << "compare giống nhau" << std::endl;
    else std::cout << "compare không giống nhau" << std::endl;

    std::cout << "multi của 7 và 8: " << cb_multi(7, 8) << std::endl;

    std::cout << "print: "; cb_anything("gọi lambda\n");

    return 0;
}
```

---

## 1. Kiến trúc

### `sy_callback::callback<Signature>`

Một đối tượng `callback` gồm ba thành phần chính:

* **Con trỏ đến đối tượng (8 bytes)**: lưu địa chỉ của đối tượng hoặc `nullptr`.
* **Hàm invoke (con trỏ hàm tĩnh)**: gọi hàm theo signature.
* **Hàm quản lý (con trỏ hàm tĩnh)**: chịu trách nhiệm **copy/move/destroy**.

Cấu trúc bên trong:

```cpp
┌───────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>         │
├───────────────────────────────────────────┤
│ object_ptr : void*                (8 B)   │
│ invoke_fn  : R(*)(void*, Args...) (8 B)   │
│ life_fn    : void*(*)(void*, Op)   (8 B)  │
└───────────────────────────────────────────┘
```

* `invoke_fn` → chứa logic gọi hàm (lambda, global, member, functor).
* `manage_fn` → chứa logic quản lý vòng đời đối tượng (copy/destroy).

Kích thước cơ bản: **24 bytes** (3 con trỏ).

Với bất kỳ callable object nào → bộ nhớ được cấp phát trên heap, và `object_ptr` trỏ tới nó.

---

## 2. Tính năng

* Hỗ trợ:

  * **Hàm global**, **static**, **hàm thành viên**.
  * **Lambda** (có capture hoặc không capture).
  * **Functor** hoặc bất kỳ callable object nào.
* Hỗ trợ **copy / move**.
* Không sử dụng SBO (Small Buffer Optimization), giúp footprint đơn giản hơn.

**Không hỗ trợ**:

* `target<T>()` giống như `std::function`.
* Move **thread-safe**.

---

## 3. Hiệu năng

### 3.1. Thời gian gọi (10 triệu lần)

| Loại callback              | Gọi trực tiếp (µs)  | `sy_callback` (µs)  | `std::function` (µs)  |
| -------------------------- | ------------------  | ------------------  | --------------------  |
| Lambda nhỏ capture         | ~22k                | ~44k–47k            | ~82k–83k              |
| Hàm thành viên (inline)    | ~22k                | ~41k–42k            | ~82k–84k              |
| Global (inline/non-inline) | ~20k                | ~34k                | ~75k                  |
| `std::bind`                | ~135k–136k          | ~150k–151k          | ~215k                 |

### 3.2. Thời gian khởi tạo & hủy (10 triệu lần)

| Loại callback                   | `sy_callback` (µs) | `std::function` (µs) |
| ------------------------------- | ------------------ | -------------------- |
| Lambda nhỏ (1 object)           | ~338k              | ~605k                |
| Lambda capture lớn (int\[1000]) | ~900k              | ~2.49M               |
| Hàm global                      | ~280k              | ~1.29M               |

### 3.3. Copy, move, assign (10 triệu lần)

| Loại          | Copy (µs) | Move (µs) | Assign (µs) |
| ------------- | --------- | --------- | ----------- |
| sy\_callback  | \~300K    | \~37K     | \~340K      |
| std::function | \~460K    | \~450K    | \~2M        |

---

## 4. Kích thước mã máy

### `sy_callback.hpp`

| Loại                        | Kích thước (bytes) |
| --------------------------- | ------------------ |
| Member (inline, cùng class) | 1008               |
| Member (inline, khác class) | 1312               |
| Global (inline)             | 336                |
| Global (non-inline)         | 32                 |
| Bất kỳ callable nào         | 654                |
| std::bind                   | 160                |
| Signature khác nhau         | 1200               |

### `std::function`

| Loại                      | Kích thước (bytes) |
| ------------------------- | ------------------ |
| Member (bind, cùng class) | 160                |
| Member (bind, khác class) | 52,784             |
| Lambda                    | 39,600             |
| Global                    | 32                 |
| Signature khác (bind)     | 58,352             |
| Signature khác (lambda)   | 1,136              |
| Signature khác (global)   | 40,272             |

---

## 5. Thời gian biên dịch (1000 callback)

### `sy_callback.hpp`

| Loại                                        | Thời gian | Kích thước mã (bytes) |
| ------------------------------------------- | --------- | --------------------- |
| Lambda                                      | \~1.216 s | 1,333,528             |
| Global (inline)                             | \~0.194 s | 683,464               |
| Global (non-inline)                         | \~0.157 s | 226,824               |
| Member (inline)                             | \~0.665 s | 1,751,752             |
| Member (lambda)                             | \~1.070 s | 1,571,576             |
| Member (bind)                               | \~0.725 s | 363,864               |
| Member (bind) (inline, khác class, cùng signature) | \~0.727 s | 2,106,376      |
| Member (bind) (inline, khác class, khác signature) | \~2.051 s | 3,893,336      |

### `std::function`

| Loại                                | Thời gian  | Kích thước mã (bytes) |
| ----------------------------------- | ---------- | --------------------- |
| Lambda                              | \~39.501 s | 30,029,544            |
| Global                              | \~0.155 s  | 263,384               |
| Member (bind)                       | \~0.715 s  | 400,424               |
| Member (khác class, cùng signature) | \~87.890 s | 45,620,328            |
| Member (khác class, khác signature) | \~92.898 s | 49,862,104            |

---

## 6. Ví dụ sử dụng

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

- http://www.apache.org/licenses/LICENSE-2.0  

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and limitations under the License.
