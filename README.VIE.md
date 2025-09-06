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
- **Invoke function (static function pointer)**: gọi hàm tương ứng với signature đối tượng.
- **Life function** **(static function pointer):** là hàm chịu trách nhiệm **copy / destroy;**
- **Thunk function (static function pointer):** là hàm sẽ trả về **Invoke function** hoặc **Life function**

Sơ đồ nội bộ:

```cpp
┌─────────────────────────────────────────────┐
│ sy_callback::callback<R(Args...)>           │
├─────────────────────────────────────────────┤   ┌───────────────────────────────────────────────┐
│ object_ptr : std::uinptr_t          (8 byte)│   │invoke_fn :RETURN (*)(std::uinptr_t, ARGS...)  │  
│ thunk_fn   : std::uinptr_t(*)(Op)   (8 byte)│ ->│life_fn   :std::uinptr_t (*)(Op, std::uinptr_t)│
└─────────────────────────────────────────────┘   └───────────────────────────────────────────────┘
```

- `object_ptr` → địa chỉ đến object
- `invoke_fn` → nhúng logic (invoke)
- `life_fn` → nhúng logic (copy / destroy)
- `thunk_fn` → trả về địa chỉ hàm invoke hoặc hàm life

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
> Lưu ý: Các số liệu đo được dưới đây có thể khác trên kiến trúc khác nhau, hệ điều hành khác nhau, compile khác nhau

### 3.1. Thời gian gọi (10 triệu lần)

| Loại callback | Gọi trực tiếp (s) | `sy_callback` (s) | `std::function` (s) |
| --- | --- | --- | --- |
| Member function (nhúng) | ~0.02 s | ~0.048 s | không có cách tương tự |
| Member function (lambda) | ~0.026 s | ~0.05 s | ~0.085 s |
| Member function (std::bind) | ~0.70 s | ~0.71 s | ~0.135 s |
| Lambda không capture | ~0.02 s | ~0.041 s | ~0.073 s |
| Lambda capture nhỏ | ~0.02 s | ~0.041 s | ~0.073 s |
| global (nhúng và không nhúng) | ~0.016 s | ~0.041 s | ~0.075 s |

### 3.2. Thời gian khởi tạo + hủy (10 triệu lần)

| Loại callback | `sy_callback` (s) | `std::function` (s) |
| --- | --- | --- |
| Lambda không capture | ~0.08 s | ~0.51 s |
| Lambda capture nhỏ (4 byte) | ~0.08 s | ~0.51 s |
| Lambda capture lớn (1024 byte) | ~0.1 s | ~1.5 s |
| Global function | ~0.06 s | ~0.53 s |
| member func | ~0.07 s | ~0.54 s |

## 3.3 Thời gian copy, move và assign (10 triệu lần)

| Loại | copy (s) | move (µs) | Assign (µs) |
| --- | --- | --- | --- |
| sy_callback | ~0.31 s | ~0.027 s | ~0.26 s |
| std::function | ~0.46 s | ~0.37 s | ~1.9 s |

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

## 5. Thời gian biên dịch (1000 callback)

### đơn vị : tổng size mã máy sinh ra (byte) / thời gian biên dịch, các giá trị của std::function và sy_callback đều đã trừ đi chi phí của base

| Loại | base | std::function | sy_callback |
| --- | --- | --- | --- |
| Global cùng signature | 119,416 / ~0.5 s | ~125,000 / ~0.15 s | ~86,000 / ~0.19 s |
| Global khác signature | 373,192 / ~0.5 s | ~32,000,000 / ~48,5 s | ~3,000,000 / ~2 s |
| Bind member cùng signature, cùng class  | 321,496 / ~0.7 s | ~180.000 / 0.2 s | ~103,000 / ~0.2 s |
| Bind member khác signature, khác class  | 6,957,208 / ~12 s | ~41,000,000 / ~248 s | ~4,700,000 / ~3.5 s |
| lambda không capture, cùng signature, khác nhau | 544,008 / ~0.75 s | ~30,000,000 / ~41 s | ~770,000 / ~0.5s |
| lambda có capture, khác signature, khác nhau (dùng để gọi hàm member) | 543,368 / ~0.8 s | ~32,000,000 / ~60 s | ~2,800,000 / ~2.2 s |
| Member nhúng cùng class cùng signature | 235,912  / 0.5 s | không có cách tương tự | ~1,400,000 / ~0.7 s |
| Member nhúng khác class khác signature | 539,992 / ~0.8 s | không có cách tương tự | ~3,400,00 / ~2.2 s |
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
