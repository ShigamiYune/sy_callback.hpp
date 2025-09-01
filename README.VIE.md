# `sy_callback.hpp` — Header-only C++11 callback

`sy_callback.hpp` là một **thư viện header-only** C++11, được thiết kế **thay thế `std::function`** với hiệu năng cao hơn và footprint nhỏ hơn.

---

Template code

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void compare(int v, int u) {
        if(v == u) {
            std::cout << "giống nhau" << std::endl;
            return;
        }
        std::cout << "không giống nhau" << std::endl;
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
        sy_callback::callback<void()>::make([](){ std::cout << "đang gọi hàm lambda" << std::endl;});

    std::cout << "giá trị nhân : " << cb_multi(88, 90) << std::endl;
    std::cout << "giống nhau không? "; cb_member(88, 88);

    cb_lambda();

    return 0;
}
```

## 1. Thành phần chính

### `sy_callback::callback<Signature>`

- Là class bọc các callback, lưu **2 con trỏ đến 2 hàm tĩnh** và **8 byte để lưu địa chỉ object**.
- Hỗ trợ:
    - Hàm **tĩnh**, **member**, **global**, **lambda có capture**, **template functor** (không phải class template), hoặc bất cứ callable nào.
    - **Copy** và **move**.
- **Kích thước**:
    - 24 byte cho member và global.
    - 24 + sizeof(anything) cho callable bất kỳ (mọi object được cấp phát trên heap, không dùng SBO).

---

## 2. Hiệu năng

### 2.1. Thời gian gọi (10 triệu lần)

| Loại callback | N | `callback` (µs) | `std::function` (µs) | Gọi trực tiếp (µs) | Ghi chú |
| --- | --- | --- | --- | --- | --- |
| Lambda capture nhỏ | 10M | 44k–47k | 82k–83k | 22k | Lambda trực tiếp, cùng object |
| Member function pointer | 10M | 41k–42k | 82k–84k | 22k | Template member pointer, inline call |
| `std::bind` | 10M | 150k | 215k | 135K - 136k | Lambda wrap `std::bind` → type-erasure runtime |

### 2.2. Thời gian khởi tạo và hủy

| Loại callback | N | `callback` init+destroy (µs) | `std::function` init+destroy (µs) |
| --- | --- | --- | --- |
| Lambda nhỏ / 1 object | 10M | 338k | 605k |
| Lambda capture lớn (mảng int 1000) | 10M | 900k | 2.49M |
| Global function | 10M | 280k | 1.29M |

---

## 3. Kích thước mã máy

### `sy_callback.hpp`

| Loại | Member cùng class | Member khác class | Global | Callable bất kỳ | Signature khác nhau |
| --- | --- | --- | --- | --- | --- |
| Kích thước (byte) | 1008  | 1312  | 560 → 32 (với heap) | 654 | 1200 |

### `std::function`

| Loại | Kích thước (byte) |
| --- | --- |
| Member cùng class (với `std::bind`) | 160 |
| Member khác class (với `std::bind`) | 52,784 |
| Lambda | 39,600 |
| Global | 32 |
| Signature khác (với `std::bind`) | 58,352 |
| Signature khác (với lambda) | 1,136 |
| Signature khác (với global) | 40,272 |

---

## 4. Thời gian biên dịch (1000 callback)

### `sy_callback.hpp`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~1.216 s | 1,333,528 |
| Global | ~0.194 s | 907,464 |
| Global (với heap) | ~0.157 s | 226,824 |
| Member | ~0.665 s | 1,751,752 |
| Member (với lambda) | ~1.070 s | 1,571,576 |
| Member (với std::bind) | ~0.725 s | 363,864 |
| Member khác class, cùng signature | ~0.727 s | 2,106,376 |
| Member khác class, khác signature | ~2.051 s | 3,893,336 |

### `std::function`

| Loại | Thời gian | Kích thước mã (byte) |
| --- | --- | --- |
| Lambda | ~39.501 s | 30,029,544 |
| Global | ~0.155 s | 263,384 |
| Member (với `std::bind`) | ~0.715 s | 400,424 |
| Member khác class, cùng signature | ~87.890 s | 45,620,328 |
| Member khác class, khác signature | ~92.898 s | 49,862,104 |

---

## 5. Hạn chế

- Không hỗ trợ **thread safe**.
- Không hỗ trợ `target<T>()` như `std::function`.

---
Copyright © 2025 ShigamiYune

Licensed under the Apache License, Version 2.0 (the "License");  
you may not use this project except in compliance with the License.  
You may obtain a copy of the License at:

- http://www.apache.org/licenses/LICENSE-2.0  

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and limitations under the License.
