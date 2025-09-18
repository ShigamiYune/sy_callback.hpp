# 7. DOCUMENT

### 7.1 Tạo callback từ member function

---

```cpp
// Cú pháp:
sy_callback::callback<RETURN(ARGS…)>::make<CLASS, &CLASS::FUNC>(OBJECT* object_ptr);

// Giải thích:
// RETURN   : kiểu trả về của member function
// ARGS…    : kiểu tham số của member function
// CLASS    : lớp chứa member function
// FUNC     : member function bạn muốn đăng ký
// OBJECT   : con trỏ tới instance (có thể const hoặc non-const)
// Trả về   : một sy_callback::callback<RETURN(ARGS…)>
```
### Ví dụ minh họa

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void my_func() {
        std::cout << "Non-const member called\n";
    }

    int my_const_func(int x) const {
        return x * 2;
    }
};

int main() {
    MyClass object;
    const MyClass const_object;

    // Non-const member function
    auto cb1 = sy_callback::callback<void()>::make<MyClass, &MyClass::my_func>(&object);
    cb1(); // Output: Non-const member called

    // Const member function
    auto cb2 = sy_callback::callback<int(int)>::make<MyClass, &MyClass::my_const_func>(&const_object);
    int result = cb2(10); // result = 20
    std::cout << "Const member result: " << result << "\n";
}
```

---

### 7.2 Tạo callback từ hàm global / static

```cpp
// Cú pháp:
sy_callback::callback<RETURN(ARGS…)>::make<&FUNC>();
sy_callback::callback<RETURN(ARGS…)>::make(FUNC);

// Giải thích:
// RETURN   : kiểu trả về của function
// ARGS…    : kiểu tham số của function
// FUNC     : global hoặc static function bạn muốn đăng ký
// Trả về   : một sy_callback::callback<RETURN(ARGS…)>
```

### Ví dụ minh họa

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    static void my_static_func() {
        std::cout << "Static called\n";
    }
};

void my_global_func() {
    std::cout << "Global called\n";
}

int add(int a, int b) {
    return a + b;
}

int main() {
    // Global function
    auto cb1 = sy_callback::callback<void()>::make<&my_global_func>();
    cb1(); // Output: Global called

    // Static function
    auto cb2 = sy_callback::callback<void()>::make(&MyClass::my_static_func);
    cb2(); // Output: Static called

    // Function có tham số và return
    auto cb3 = sy_callback::callback<int(int,int)>::make<&add>();
    std::cout << "add(2,3) = " << cb3(2,3) << "\n"; // Output: 5

    // Thông tin thêm:
    // - Hàm global và static được xử lý như nhau
    // nên không có khác biệt về cách viết, performance hoặc mã máy
    // - Về hiệu năng thì cả hai cách đều như nhau

    // Tip: có thể viết ngắn gọn như sau
    sy_callback::callback<void()> cb4 = &MyClass::my_static_func;
    sy_callback::callback<void()> cb5 = my_global_func;

    // Gợi ý: nên viết theo 1 trong 2 cách
    //     auto cb = sy_callback::callback<void()>::make(&MyClass::my_static_func);
    // hoặc
    //     sy_callback::callback<void()> cb = MyClass::my_static_func;
    //
    // Hai cách này giúp mã máy gọn hơn đáng kể, performance không đổi
}
```

---

### 7.3 Tạo callback từ lambda (không capture và có capture)

```cpp
// Cú pháp:
sy_callback::callback<RETURN(ARGS…)>::make(LAMBDA);

// Giải thích:
// RETURN   : kiểu trả về của lambda
// ARGS…    : kiểu tham số của lambda
// LAMBDA   : lambda bạn muốn đăng ký
// Trả về   : một sy_callback::callback<RETURN(ARGS…)>
```

### Ví dụ minh họa

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    // Lambda không capture
    auto lambda_without_capture = [](){ std::cout << "Lambda without capture called\n"; };
    auto cb1 = sy_callback::callback<void()>::make(lambda_without_capture);
    cb1(); // Output: Lambda without capture called

    // Lambda có capture
    int x = 10;
    auto lambda_with_capture = [x](){ std::cout << "Lambda with capture called, x = " << x << "\n"; };
    auto cb2 = sy_callback::callback<void()>::make(lambda_with_capture);
    cb2(); // Output: Lambda with capture called, x = 10

    // Thông tin thêm:
    // - Lambda không capture được xử lý như global/static function
    // - Lambda có capture luôn được cấp phát trên heap (dù capture nhỏ hay lớn)
    //   vì sy_callback chưa hỗ trợ SBO

    // Tip: cũng có thể viết ngắn gọn
    sy_callback::callback<void()> cb3 = lambda_without_capture;
    sy_callback::callback<void()> cb4 = lambda_with_capture;
    cb3();
    cb4();
    // hoặc
    sy_callback::callback<void()> cb5 = 
		    [](){ std::cout << "short callback for lambda without capture\n"; };
		    
    float y = 10101.0f;
    sy_callback::callback<void()> cb6 = 
		    [y](){ std::cout << "short callback for lambda with capture y: " << y << "\n"; };
    
    cb5();
    cb6();
}
```

---

### 7.4 Tạo callback từ Functor

```cpp
// Cú pháp:
sy_callback::callback<RETURN(ARGS…)>::make(FUNCTOR);

// Giải thích:
// RETURN   : kiểu trả về của lambda
// ARGS…    : kiểu tham số của lambda
// FUNCTOR  : functor bạn muốn đăng ký
// Trả về   : một sy_callback::callback<RETURN(ARGS…)>
```

### Ví dụ minh họa

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct Functor {
    void operator()() {
        std::cout << "Functor called \n";
    }
};

int main() {
    sy_callback::callback<void()> cb1 = sy_callback::callback<void()>::make(Functor{});
    cb1();

    // Thông tin thêm:
    // Functor sẽ được xử lý như lambda có capture nếu không có operator cast sang RETURN(*)(ARGS...)

    // Tip: cũng có thể viết ngắn gọn
    sy_callback::callback<void()> cb2 = Functor{};
    cb2();
}
```

---

### 7.5 Tạo callback từ hàm generator / template

```cpp
// Cú pháp: như đã giới thiệu từ 7.1 7.2 7.3 7.4
```

### Ví dụ minh họa

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct Math {
    template<typename R, typename V, typename U>
    static R multi(V v, U u) {
        return v * u;
    }
};

struct Character {
    float velocity = 5.0f;

    template<typename K>
    void faster(K k) {
        velocity *= k;
    }
};

struct Funtor {
    template<typename K>
    void operator()(K k) {
        std::cout << "functor template called width:" << k << "\n";
    }
};

template<typename R, typename V, typename U>
R total(V v, U u) {
    return v + u;
}

int main() {
    Character character;

    // hàm member template
    sy_callback::callback<void(float)> cb1 = 
        sy_callback::callback<void(float)>::make<Character, &Character::faster>(&character);
    std::cout << "Current velocity: " << character.velocity << "\n";
    cb1(5.0f);
    std::cout << "Current velocity (after called cb1): " << character.velocity << "\n";

    // hàm static template
    sy_callback::callback<int(float, int)> cb2 = 
        sy_callback::callback<int(float, int)>::make<&Math::multi>();
    std::cout << "Multi of 5 and 10: " << cb2(5.0f, 10) << "\n";

    // hàm global template
    sy_callback::callback<int(float, int)> cb3 = 
        sy_callback::callback<int(float, int)>::make(&total);
    std::cout << "Total of 8 and 19: " << cb3(8.0f, 19) << "\n";

    // Lambda generation không capture
    auto lambda_generation_without_capture = [](auto v, auto u){
            return v / u;
        };
    sy_callback::callback<float(float, float)> cb4 = 
        sy_callback::callback<float(float, float)>::make(lambda_generation_without_capture);
    std::cout << "Device of 5 and 7: " << cb4(5.0f, 7.0f) << "\n";

    // Lambda generation có capture
    int x = 10;
    auto lambda_generation_with_capture = [x](auto y){
            return x - y;
        };
    sy_callback::callback<float(double)> cb5 = 
        sy_callback::callback<float(double)>::make(lambda_generation_with_capture);
    std::cout << "Delta of x and 7: " << cb5(7.0) << "\n";

    // functor template
    sy_callback::callback<void(const char*)> cb6 = 
        sy_callback::callback<void(const char*)>::make(Funtor{});
    cb6("print in print");

    // Cách thức hoạt động:
    // Mọi hàm template hoặc generation đều được tự động cast thành hàm phù hợp với sy_callback::callback
    // Bạn cũng có thể tự cast nếu muốn
    sy_callback::callback<int(float, int)> cb7 = 
        sy_callback::callback<int(float, int)>::make(static_cast<int(*)(float, int)>(&total));
    std::cout << "Hand cast for Total of 8 and 19: " << cb7(8.0f, 19) << "\n";

    
    // Thông tin thêm:
    //
    // - Hàm global và static vẫn được xử lý như nhau
    // nên vẫn không có khác biệt về cách viết, performance hoặc mã máy
    // - Về hiệu năng thì cả hai cách vẫn như nhau
    //
    // - Lambda không capture vẫn được xử lý như global/static function
    // - Lambda có capture vẫn luôn được cấp phát trên heap (dù capture nhỏ hay lớn)
    //   vì sy_callback chưa hỗ trợ SBO
    //
    // Functor vẫn sẽ được xử lý như lambda có captrure nếu không có operator cast sang RETURN(*)(ARGS...)

    // Tip: cũng có thể viết ngắn gọn như đã giới thiệu trước đó ở 1. 2. 3. 4.

    // Lưu ý: vẫn như đã giới thiệu trước đó ở 1. 2. 3. 4.
    // Tuy nhiên ở đây
    // sy_callback::callback chỉ hỗ trợ các hàm template / generation chứ không hỗ trợ template class
    // nếu muốn dùng template class bạn phải instance nó
}
```

---

## 8. API của sy_callback::callback

### 8.1 Copy

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() { 
    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "Base callback\n"; };
		    
    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "Copy callback \n"; };
		
    std::cout << "\nBefore Copy callback:\n";
    cb1();
    cb2();

    cb1 = cb2;

    std::cout << "\nAfter Copy callback:\n";
    cb1();
    cb2();
    
    // Lưu ý: 
    // Nếu bạn copy callback trên đa luồng, callback sẽ không gặp vấn đề gì cả
}
```
--
### 8.2 Move

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "Base callback\n"; };
		    
    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "Move callback\n"; };
		
    std::cout << "\nBefore Move callback:\n";
    cb1();
    cb2();

    cb1 = std::move(cb2);

    std::cout << "\nAfter Move callback:\n";
    cb1();
    // Lưu ý : 
    // bạn không nên gọi callback đã được move như cb2
    // nếu bạn gọi, nó sẽ ném 1 throw bad_functional_call()
    //
    // bạn có thể kiểm tra bằng callback.isCallable()
    // 
    // 1 lưu ý khác về đa luồng: 
    // Nếu bạn move callback trên đa luồng thì sẽ có khả năng bị race coddition
}
```
---
### 8.3 isCallable() và operator bool

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    sy_callback::callback<void()> cb1;

    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "I don't known write what in here\n"; };
		
    if(!cb1.isCallable()) {
        std::cout << "cb1 not have any a callback for call\n";
    }
    else {
        std::cout << "cb2 have a somthing callback for call and we call it: ";
        cb1();
    }

    // hoặc

    if(!cb2) {
        std::cout << "cb2 not have any a callback for call\n";
    }
    else {
        std::cout << "cb2 have a somthing callback for call and we call it: ";
        cb2();
    }
}
```
---
### 8.4 swap

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "That context will be print in first line \n"; };
		    
    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "That context will be print in seccond line\n"; };

    std::cout << "\nBefore swap callback:\n";
    cb1();
    cb2();

    cb1.swap(cb2);

    std::cout << "\nAfter swap callback:\n";
    cb1();
    cb2();

    // Lưu ý
    // Nếu bạn swap callback trên đa luồng thì sẽ có khả năng bị race coddition
}
```
---
### 8.5 Assign

```
#include <iostream>
#include "sy_callback.hpp"

struct Functor {
    void operator()() {
        std::cout << "I WANT YOU CALL ME\n";
    }
};

void global() {
    std::cout << "Wh-Wh-What ... ... ... OK? OK! What - What do you want from me?\n";
}

int main() {

    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "knock, knock, knock!\n"; }; 
    cb1();

    auto lambda_without_capture = [](){ std::cout << "Who is there?\n"; };
    cb1 = lambda_without_capture;
    cb1(); // assign

    const char* voice = "I AM THE CALLBACK\n";
    auto lambda_with_capture = [voice](){ std::cout << voice; };
    cb1 = lambda_with_capture; // assign
    cb1();

    cb1 = global; // assign
    cb1();

    cb1 = Functor{}; // assign
    cb1();

    // Lưu ý
    // Nếu bạn asiggn callback trên đa luồng thì cũng có khả năng bị race coddition
}
```
---
### 8.6 target<T>()

```cpp
// Cú pháp

callback.target<T>()

// T : là kiểu bạn muốn target
// target<T>() trả về bản gốc nếu T cùng kiểu với callback nó đang sở hữu
```

### Ví dụ minh hoạ

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct MyClass {
    void member () {
        std::cout << "Member called\n";
    }
};

struct Functor {
    void operator()() {
        std::cout << "Functor called\n";
    }
};

void global() {
    std::cout << "Global called\n";
}

int main() {
    MyClass my_class;
    sy_callback::callback<void()> cb1 = 
        sy_callback::callback<void()>::make<MyClass, &MyClass::member>(&my_class);

    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "lambda called\n"; };

    sy_callback::callback<void()> cb3 = global;
    sy_callback::callback<void()> cb4 = Functor{};

    if(auto member = cb1.target<MyClass>()) {
        (*member)();
        member->member();
    }

    if(auto lambda = cb2.target<void(*)()>()) {
        (*lambda)();
    }

    if(auto global = cb3.target<void(*)()>()) {
        (*global)();
    }

    if(auto functor = cb4.target<Functor>()) {
        (*functor)();
    }

    // Lưu ý target sẽ luôn trả về object gốc chứ không copy
}
```
---
### 8.7 reset()

```cpp
#include <iostream>
#include "sy_callback.hpp"

void global() {
    std::cout << "Global called\n";
};

int main() {
    sy_callback::callback<void()> cb1 = global;
   
    if(cb1) {
        std::cout << "callback cb1 is callable and we call it: "; cb1();
    }
    else{
        std::cout << "callback cb1 can't callable \n";
    }

    cb1.reset();

    if(cb1) {
        std::cout << "callback cb1 is callable and we call it: "; cb1();
    }
    else{
        std::cout << "callback cb1 can't callable \n";
    }
    
    // Lưu ý
    // Nếu bạn reset callback trên đa luồng thì sẽ có khả năng bị race coddition

    return 0;
}
```
### 8.8 invoke_prediction<T>(arg…)

Là một hàm `invoke` nhưng sẽ **thực thi nhanh hơn** nếu người dùng dự đoán đúng **kiểu (`T`) callback mà `sy_callback` đang giữ**.

- Khi kiểu đúng (`T` trùng), `invoke_cast` sẽ đi theo **fast path**, gần như trực tiếp gọi hàm, tương tự **CPU dự đoán nhánh đúng (branch prediction hit)**.
- Khi kiểu sai, `invoke_cast` phải thực hiện kiểm tra runtime và tìm đúng hàm, tương tự **CPU dự đoán nhánh sai (branch misprediction)**, nên chậm hơn.

### Ví dụ minh hoạ

```cpp
#include <iostream>
#include "sy_callback.hpp"

using namespace sy_callback;

struct MyClass {
    void member1() { std::cout << "member1 has call \n"; }
    void member2() const { std::cout << "member2 has call \n"; }
};

void global() {
    std::cout << "global has call \n";
}

int main() {
    MyClass object;

    auto lambda = [](){ std::cout << "lambda has call \n"; };

    callback<void()> cb1 = lambda;
    cb1.invoke_prediction<decltype(lambda)>();

    callback<void()> cb2 = global;
    cb2.invoke_prediction<void(*)()>();

    callback<void()> cb3 = callback<void()>::make<MyClass, &MyClass::member1>(&object);
    cb3.invoke_prediction<MyClass, &MyClass::member1>();

    // Lưu ý:
    // đối với callback member như cb3 nếu instance object là const 
    // thì tại invoke_prediction bạn cũng cần truyền vào class là const
    // Ví dụ
    const MyClass const_object;
    callback<void()> cb4 = callback<void()>::make<MyClass, &MyClass::member2>(&const_object);
    cb4.invoke_prediction<const MyClass, &MyClass::member2>();

    // Thông tin hiệu suất
    // với invoke_prediction dù bạn đoán đúng kiểu thì nó chỉ nhanh hơn từ 10 - 20% 
    // và có thể tăng 1 chút mã máy
    return 0;
}
```
