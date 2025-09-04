# `sy_callback.hpp` — Thư viện callback header-only cho C++11

---

`sy_callback.hpp` là một **thư viện header-only** nhằm thay thế `std::function` trong môi trường yêu cầu hiệu suất cao, nhúng

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

Một đối tượng `callback` gồm 2 thành phần chính:

- **Pointer object (8 byte)**: lưu trữ địa chỉ object hoặc nullptr.
- **Invoke function (static function pointer)**: gọi hàm tương ứng với signature và chịu trách nhiệm **copy / destroy / compare** đối tượng.

Sơ đồ nội bộ:

```cpp
┌──────────────────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>                        │
├──────────────────────────────────────────────────────────┤
│ object_ptr : std::uinptr_t                       (8 byte)│
│ invoke_fn  : union(*)(Op std::uinptr_t, args*)   (8 byte)│
└──────────────────────────────────────────────────────────┘
```

- `invoke_fn` → nhúng logic (invoke, copy, destroy, compare)
- `life_fn` → nhúng logic quản lý vòng đời (copy / destroy).

Kích thước cơ bản: 16 **byte** (2 con trỏ).

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

> Lưu ý: tất cả giá trị đánh dấu ~ là trung bình và làm tròn từ nhiều lần biên dịch với clang++ -O2 trên MacOS M1.
Lưu ý: Các số liệu đo được dưới đây có thể khác trên kiến trúc khác nhau, hệ điều hành khác nhau, compile khác nhau
> 

### 3.1. Thời gian gọi (10 triệu lần)

| Loại callback | Gọi trực tiếp (s) | `sy_callback` (µs) | `std::function` (µs) |
| --- | --- | --- | --- |
| Lambda capture nhỏ | ~0.022 s | ~0.044 s | ~0.082 s |
| Member function (nhúng) | ~0.022 s | ~0.042 s | ~0.082 s |
| global (nhúng và không nhúng) | ~0.02 s | ~0.034 s | ~0.075 s |
| `std::bind` | ~0.135 s | ~0.15 s | ~0.215 s |

### 3.2. Thời gian khởi tạo & hủy (10 triệu lần)

| Loại callback | `sy_callback` (s) | `std::function` (s) |
| --- | --- | --- |
| Lambda nhỏ (1 object) | ~0.338 s | ~0.605 s |
| Lambda capture lớn (mảng int[1000]) | ~0.9 s | ~2.49 s |
| Global function | ~0.28 s | ~1.29 s |

## 3.3 Thời gian copy, move và assign (10 triệu lần)

| Loại | copy (s) | move (µs) | Assign (µs) |
| --- | --- | --- | --- |
| sy_callback | ~0.3 s | ~0.037 s | ~0.340 s |
| std::function | ~0.460 | ~0.45 s | ~2 s |

---

## 4. Memory usage ( 64 bit )

| size của callable (byte) | std::function total (byte) | sy_callback total (byte) |
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

### 5. Kích thước mã máy (Codegen size)

---

### 5.1. Kích thước cá nhân (base + per-signature)

| Thư viện | Base overhead (byte) | Per-signature overhead (byte) | **Tổng codegen** =
Base + body của callback × n |
| --- | --- | --- | --- |
| **sy_callback** | ~1,700 | ~1,000 | ~1,700 + (body × n) |
| **std::function** | ~140 | ~1,000 | ~140 + (body × n) |

---

### 5.2. Kích thước wrap theo loại callback (body) (đã trừ chi phí Base)

5.2.1 Nhúng

| Loại callback | sy_callback (byte) | std::function (byte) |
| --- | --- | --- |
| khác Member - cùng class - cùng signature - nhúng | ~900 | không có cách tương tự |
| Member khác class, cùng signature, nhúng | ~900 | Không có cách tương tự |
| Member khác class, khác signature, nhúng | ~2,300 | Không có cách tương tự |
| Member cùng class, khác signature, nhúng | ~2,300 | Không có cách tương tự |
| Global / Static cùng signature, nhúng | ~300 | Không có cách tương tự |
| Global / Static Khác signature, nhúng | ~1,800 | Không có cách tương tự |

5.2.2 Bind

| Loại callback | sy_callback (byte) | std::function (byte) |
| --- | --- | --- |
| Member cùng class, cùng signature, bind | 0  | 0  |
| Member khác class, cùng signature, bind | ~1,000 | ~28,000 |
| Member cùng class, khác signature, bind | ~7,000 | ~42,000 |
| Member khác class, khác signature, bind | ~7,000 | ~42,000 |

5.2.3 Lambda

| Loại callback | sy_callback (byte) | std::function (byte) |
| --- | --- | --- |
| Lambda không capture, cùng signature | ~500 | ~30,000 |
| Lambda không capture, khác signature | ~1,800 | ~32,000 |
| Lambda có capture 8 byte, cùng signature | ~600 | ~30,000  |
| Lambda có capture 24 byte, cùng signature | ~600 | ~32,000  |
| Lambda có capture 8 byte, khác signature | ~1,400 | ~32,000 |
| Lambda có capture 24 byte, khác signature | ~1,400 | ~34,000 |

5.2.4 Global / Static

| Loại callback | sy_callback (byte) | std::function (byte) |
| --- | --- | --- |
| Global / Static cùng signature | 96 `cho lần đầu tiên và 0 cho những lần tiếp theo` | 32 `cho lần đầu tiên và 0 cho những lần tiếp theo` |
| Global / Static khác signature | ~1,600 | ~31,000 |

## 6. Thời gian biên dịch (1000 callback)

### `sy_callback.hpp`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~1.216 s | ~1,334,000 |
| Global (nhúng) | ~0.194 s | ~683,000 |
| Global (không nhúng) | ~0.157 s | ~227,000 |
| Member (nhúng) | ~0.665 s | ~1,752,000 |
| Member (lambda) | ~1.070 s | ~1,572,000 |
| Member (bind) | ~0.725 s | ~364,000 |
| Member (nhúng) khác class, cùng signature | ~0.727 s | ~2,106,000 |
| Member (nhúng) khác class, khác signature | ~2.051 s | ~3,893,000 |

### `std::function`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~39.501 s | ~30,023,000 |
| Global | ~0.155 s | 263,000 |
| Member (bind) | ~0.715 s | ~400,000 |
| Member khác class, cùng signature | ~87.890 s | ~45,620,000 |
| Member khác class, khác signature | ~92.898 s | ~49,862,000 |
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
