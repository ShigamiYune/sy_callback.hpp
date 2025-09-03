# `sy_callback.hpp` — Thư viện callback header-only cho C++11

---
`sy_callback.hpp` là một **thư viện header-only** thay thế `std::function`, với **hiệu năng cao hơn** và **footprint nhỏ hơn**.

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

## 1. Kiến trúc

### `sy_callback::callback<Signature>`

Một đối tượng `callback` gồm 3 thành phần chính:

- **Pointer object (8 byte)**: lưu trữ địa chỉ object hoặc nullptr.
- **Invoke function (static function pointer)**: gọi hàm tương ứng với signature.
- **Life function (static function pointer)**: chịu trách nhiệm **copy / destroy** đối tượng.

Sơ đồ nội bộ:

```cpp
┌──────────────────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>                        │
├──────────────────────────────────────────────────────────┤
│ object_ptr : std::uinptr_t                       (8 byte)│
│ invoke_fn  : R(*)(std::uinptr_t, Args...)        (8 byte)│
│ life_fn    : std::uinptr_t(*)(Op, std::uinptr_t) (8 byte)│
└──────────────────────────────────────────────────────────┘
```

- `invoke_fn` → nhúng logic gọi hàm (lambda, global, member, functor).
- `life_fn` → nhúng logic quản lý vòng đời (copy / destroy).

Kích thước cơ bản: **24 byte** (3 con trỏ).

Với callable bất kỳ → đối tượng được cấp phát trên heap, `object_ptr` trỏ tới vùng nhớ đó. (nếu là lambda không capture thì sẽ được lưu thành R(*)(Args...) vào object_ptr)

---

## 2. Tính năng

- Hỗ trợ:
    - Hàm **global**, **static**, **member**.
    - **Lambda** (có capture hoặc không).
    - **Functor** hoặc bất kỳ callable object nào.
    - Với template function thì callback sẽ tự động cast template function về kiểu phù hợp
- Cho phép **copy / move**.
- Không dùng SBO (Small Buffer Optimization), giúp đơn giản hóa footprint.
- `target<T>()` như `std::function`.

**Không hỗ trợ**:

- “Move và destroy” multi **thread-safe**. (nếu move và destroy trên đơn luồng thì an toàn, copy trên đa luồng vẫn an toàn)

---

## 3. Hiệu năng

### 3.1. Thời gian gọi (10 triệu lần)

| Loại callback | Gọi trực tiếp (µs) | `sy_callback` (µs) | `std::function` (µs) |
| --- | --- | --- | --- |
| Lambda capture nhỏ | 22k | 44k–47k | 82k–83k |
| Member function (nhúng) | 22k | 41k–42k | 82k–84k |
| global (nhúng và không nhúng) | 20k | 34k | 75k |
| `std::bind` | 135k–136k | 150k–151K | 215k |

### 3.2. Thời gian khởi tạo & hủy (10 triệu lần)

| Loại callback | `sy_callback` (µs) | `std::function` (µs) |
| --- | --- | --- |
| Lambda nhỏ (1 object) | 338k | 605k |
| Lambda capture lớn (mảng int[1000]) | 900k | 2.49M |
| Global function | 280k | 1.29M |

## 3.3 Thời gian copy, move và assign (10 triệu lần)

| Loại | copy (µs) | move (µs) | Assign (µs) |
| --- | --- | --- | --- |
| sy_callback | ~300K | ~37K | ~340K |
| std::function | ~460K | ~450K | ~2M |

---

## 4. Memory usage ( 64 bit )

| Lambda size (byte) | std::function total (byte) | sy_callback total (byte) |
| --- | --- | --- |
| 1 | 32 | 25 |
| 8 | 32 | 32 |
| 16 | 32 | 40 |
| 24 | 64 | 48 |
| 32 | 72 | 56 |
| 48 | 88 | 72 |
| 56 | 96 | 80 |
| 64 | 104 | 88 |
| … | 32 + Lambda size + 8 (vptr) | 24 + Lambda size |

## 5. Kích thước mã máy

### `sy_callback.hpp`

| Loại | Kích thước (byte) |
| --- | --- |
| Member (nhúng) cùng class | 1008 |
| Member (nhúng) khác class | 1312 |
| Global (nhúng) | 336 |
| Global (không nhúng) | 32 |
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

## 6. Thời gian biên dịch (1000 callback)

### `sy_callback.hpp`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~1.216 s | 1,333,528 |
| Global (nhúng) | ~0.194 s | 683,464 |
| Global (không nhúng) | ~0.157 s | 226,824 |
| Member (nhúng) | ~0.665 s | 1,751,752 |
| Member (lambda) | ~1.070 s | 1,571,576 |
| Member (bind) | ~0.725 s | 363,864 |
| Member (nhúng) khác class, cùng signature | ~0.727 s | 2,106,376 |
| Member (nhúng) khác class, khác signature | ~2.051 s | 3,893,336 |

### `std::function`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~39.501 s | 30,029,544 |
| Global | ~0.155 s | 263,384 |
| Member (bind) | ~0.715 s | 400,424 |
| Member khác class, cùng signature | ~87.890 s | 45,620,328 |
| Member khác class, khác signature | ~92.898 s | 49,862,104 |
---

## [Đọc Document và API tại đây:] (https://github.com/ShigamiYune/sy_callback.hpp/blob/main/DOCUMENT.VIE.md)

---
Copyright © 2025 ShigamiYune

Licensed under the Apache License, Version 2.0 (the "License");  
you may not use this project except in compliance with the License.  
You may obtain a copy of the License at:

- http://www.apache.org/licenses/LICENSE-2.0  

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and limitations under the License.
