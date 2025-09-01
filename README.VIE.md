# `sy_callback.hpp` — Header-only C++11 callback

---

`sy_callback.hpp` là một **thư viện header-only** thay thế `std::function`, với **hiệu năng cao hơn** và **footprint nhỏ hơn**.

---

## 1. Kiến trúc

### `sy_callback::callback<Signature>`

Một đối tượng `callback` gồm 3 thành phần chính:

- **Pointer object (8 byte)**: lưu trữ địa chỉ object hoặc nullptr.
- **Invoke function (static function pointer)**: gọi hàm tương ứng với signature.
- **Manager function (static function pointer)**: chịu trách nhiệm **copy / move / destroy** đối tượng.

Sơ đồ nội bộ:

```cpp
┌───────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>         │
├───────────────────────────────────────────┤
│ object_ptr : void*                (8 byte)│
│ invoke_fn  : R(*)(void*, Args...) (8 byte)│
│ life_fn    : void(*)(void*, Op)   (8 byte)│
└───────────────────────────────────────────┘
```

- `invoke_fn` → nhúng logic gọi hàm (lambda, global, member, functor).
- `manage_fn` → nhúng logic quản lý vòng đời (copy / destroy).

Kích thước cơ bản: **24 byte** (3 con trỏ).

Với callable bất kỳ → đối tượng được cấp phát trên heap, `object_ptr` trỏ tới vùng nhớ đó.

---

## 2. Tính năng

- Hỗ trợ:
    - Hàm **global**, **static**, **member**.
    - **Lambda** (có capture hoặc không).
    - **Functor** hoặc bất kỳ callable object nào.
- Cho phép **copy / move**.
- Không dùng SBO (Small Buffer Optimization), giúp đơn giản hóa footprint.

**Không hỗ trợ**:

- `target<T>()` như `std::function`.
- Move **thread-safe**.

---

## 3. Hiệu năng

### 3.1. Thời gian gọi (10 triệu lần)

| Loại callback | N | `sy_callback` (µs) | `std::function` (µs) | Gọi trực tiếp (µs) |
| --- | --- | --- | --- | --- |
| Lambda capture nhỏ | 10M | 44k–47k | 82k–83k | 22k |
| Member function | 10M | 41k–42k | 82k–84k | 22k |
| `std::bind` | 10M | 150k | 215k | 135k–136k |

### 3.2. Thời gian khởi tạo & hủy (10 triệu lần)

| Loại callback | `sy_callback` (µs) | `std::function` (µs) |
| --- | --- | --- |
| Lambda nhỏ (1 object) | 338k | 605k |
| Lambda capture lớn (mảng int[1000]) | 900k | 2.49M |
| Global function | 280k | 1.29M |

---

## 4. Kích thước mã máy

### `sy_callback.hpp`

| Loại | Kích thước (byte) |
| --- | --- |
| Member cùng class | 1008 |
| Member khác class | 1312 |
| Global | 560 → 32 (heap) |
| Callable bất kỳ | 654 |
| std::bind | 160 |
| Signature khác nhau | 1200 |

### `std::function`

| Loại | Kích thước (byte) |
| --- | --- |
| Member cùng class (bind) | 160 |
| Member khác class (bind) | 52,784 |
| Lambda | 39,600 |
| Global | 32 |
| Signature khác nhau (bind) | 58,352 |
| Signature khác nhau (lambda) | 1,136 |
| Signature khác nhau (global) | 40,272 |

---

## 5. Thời gian biên dịch (1000 callback)

### `sy_callback.hpp`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~1.216 s | 1,333,528 |
| Global | ~0.194 s | 907,464 |
| Global (heap) | ~0.157 s | 226,824 |
| Member | ~0.665 s | 1,751,752 |
| Member (lambda) | ~1.070 s | 1,571,576 |
| Member (bind) | ~0.725 s | 363,864 |
| Member khác class, cùng signature | ~0.727 s | 2,106,376 |
| Member khác class, khác signature | ~2.051 s | 3,893,336 |

### `std::function`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~39.501 s | 30,029,544 |
| Global | ~0.155 s | 263,384 |
| Member (bind) | ~0.715 s | 400,424 |
| Member khác class, cùng signature | ~87.890 s | 45,620,328 |
| Member khác class, khác signature | ~92.898 s | 49,862,104 |

---

## 6. Ví dụ sử dụng

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

- http://www.apache.org/licenses/LICENSE-2.0  

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and limitations under the License.
