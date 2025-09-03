# 7. DOCUMENTATION

### 7.1 Create callback from member function

---

```cpp
// Syntax:
sy_callback::callback<RETURN(ARGS…)>::make<CLASS, &CLASS::FUNC>(OBJECT* object_ptr);

// Explanation:
// RETURN   : return type of the member function
// ARGS…    : parameter types of the member function
// CLASS    : class containing the member function
// FUNC     : the member function you want to register
// OBJECT   : pointer to the instance (can be const or non-const)
// Returns  : a sy_callback::callback<RETURN(ARGS…)>
```

### Example

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

### 7.2 Create callback from global / static function

```cpp
// Syntax:
sy_callback::callback<RETURN(ARGS…)>::make<&FUNC>();
sy_callback::callback<RETURN(ARGS…)>::make(FUNC);

// Explanation:
// RETURN   : return type of the function
// ARGS…    : parameter types of the function
// FUNC     : global or static function you want to register
// Returns  : a sy_callback::callback<RETURN(ARGS…)>
```

### Example

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

    // Function with parameters and return value
    auto cb3 = sy_callback::callback<int(int,int)>::make<&add>();
    std::cout << "add(2,3) = " << cb3(2,3) << "\n"; // Output: 5

    // Extra information:
    // - Global and static functions are treated the same
    //   so there’s no difference in syntax, performance, or machine code
    // - Performance is identical in both cases

    // Tip: you can also write in shorter form
    sy_callback::callback<void()> cb4 = &MyClass::my_static_func;
    sy_callback::callback<void()> cb5 = my_global_func;

    // Recommended style:
    //     auto cb = sy_callback::callback<void()>::make(&MyClass::my_static_func);
    // or
    //     sy_callback::callback<void()> cb = MyClass::my_static_func;
    //
    // These produce leaner machine code without performance loss
}
```

---

### 7.3 Create callback from lambda (with and without capture)

```cpp
// Syntax:
sy_callback::callback<RETURN(ARGS…)>::make(LAMBDA);

// Explanation:
// RETURN   : return type of the lambda
// ARGS…    : parameter types of the lambda
// LAMBDA   : lambda you want to register
// Returns  : a sy_callback::callback<RETURN(ARGS…)>
```

### Example

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    // Lambda without capture
    auto lambda_without_capture = [](){ std::cout << "Lambda without capture called\n"; };
    auto cb1 = sy_callback::callback<void()>::make(lambda_without_capture);
    cb1(); // Output: Lambda without capture called

    // Lambda with capture
    int x = 10;
    auto lambda_with_capture = [x](){ std::cout << "Lambda with capture called, x = " << x << "\n"; };
    auto cb2 = sy_callback::callback<void()>::make(lambda_with_capture);
    cb2(); // Output: Lambda with capture called, x = 10

    // Extra information:
    // - Lambda without capture is treated like global/static function
    // - Lambda with capture is always allocated on heap
    //   because sy_callback does not yet support SBO

    // Tip: you can also write shorter
    sy_callback::callback<void()> cb3 = lambda_without_capture;
    sy_callback::callback<void()> cb4 = lambda_with_capture;
    cb3();
    cb4();
    // or
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

### 7.4 Create callback from Functor

```cpp
// Syntax:
sy_callback::callback<RETURN(ARGS…)>::make(FUNCTOR);

// Explanation:
// RETURN   : return type of the functor
// ARGS…    : parameter types of the functor
// FUNCTOR  : functor you want to register
// Returns  : a sy_callback::callback<RETURN(ARGS…)>
```

### Example

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

    // Extra information:
    // A functor is treated like a lambda with capture 
    // unless it defines an operator cast to RETURN(*)(ARGS...)

    // Tip: you can also write shorter
    sy_callback::callback<void()> cb2 = Functor{};
    cb2();
}
```

---

### 7.5 Create callback from generator / template function

```cpp
// Syntax: same as introduced in 7.1 7.2 7.3 7.4
```

### Example

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
        std::cout << "functor template called with: " << k << "\n";
    }
};

template<typename R, typename V, typename U>
R total(V v, U u) {
    return v + u;
}

int main() {
    Character character;

    // member template function
    sy_callback::callback<void(float)> cb1 = 
        sy_callback::callback<void(float)>::make<Character, &Character::faster>(&character);
    std::cout << "Current velocity: " << character.velocity << "\n";
    cb1(5.0f);
    std::cout << "Current velocity (after called cb1): " << character.velocity << "\n";

    // static template function
    sy_callback::callback<int(float, int)> cb2 = 
        sy_callback::callback<int(float, int)>::make<&Math::multi>();
    std::cout << "Multi of 5 and 10: " << cb2(5.0f, 10) << "\n";

    // global template function
    sy_callback::callback<int(float, int)> cb3 = 
        sy_callback::callback<int(float, int)>::make(&total);
    std::cout << "Total of 8 and 19: " << cb3(8.0f, 19) << "\n";

    // Lambda generation without capture
    auto lambda_generation_without_capture = [](auto v, auto u){
            return v / u;
        };
    sy_callback::callback<float(float, float)> cb4 = 
        sy_callback::callback<float(float, float)>::make(lambda_generation_without_capture);
    std::cout << "Divide 5 by 7: " << cb4(5.0f, 7.0f) << "\n";

    // Lambda generation with capture
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

    // How it works:
    // All template or generic functions are automatically cast 
    // to a function compatible with sy_callback::callback
    // You can also manually cast if needed
    sy_callback::callback<int(float, int)> cb7 = 
        sy_callback::callback<int(float, int)>::make(static_cast<int(*)(float, int)>(&total));
    std::cout << "Manual cast for Total of 8 and 19: " << cb7(8.0f, 19) << "\n";

    // Extra information:
    //
    // - Global and static functions are still treated the same
    // - Performance remains identical
    //
    // - Lambda without capture is still treated like global/static function
    // - Lambda with capture is always allocated on heap
    //   because sy_callback does not yet support SBO
    //
    // - Functor is still treated like lambda with capture 
    //   unless it defines operator cast to RETURN(*)(ARGS...)

    // Tip: can also be written shorter as introduced earlier
    // Note: template classes are not supported directly,
    // you must instantiate them before use
}
```

---

## 8. API of sy\_callback::callback

### 8.1 Copy

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() { 
    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "Base callback\n"; };
		    
    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "Copy callback\n"; };
		
    std::cout << "\nBefore Copy callback:\n";
    cb1();
    cb2();

    cb1 = cb2;

    std::cout << "\nAfter Copy callback:\n";
    cb1();
    cb2();
    
    // Note: 
    // Copying callback across multiple threads is safe
}
```

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
    // Note:
    // you should not call a moved-from callback like cb2
    // if called, it throws bad_functional_call()
    //
    // you can check using callback.isCallable()
    //
    // Another note on multithreading:
    // Moving callbacks across threads may cause race conditions
}
```

### 8.3 isCallable() and operator bool

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    sy_callback::callback<void()> cb1;

    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "I don’t know what to write here\n"; };
		
    if(!cb1.isCallable()) {
        std::cout << "cb1 does not have any callback to call\n";
    }
    else {
        std::cout << "cb1 has something and we call it: ";
        cb1();
    }

    // or

    if(!cb2) {
        std::cout << "cb2 does not have any callback to call\n";
    }
    else {
        std::cout << "cb2 has something and we call it: ";
        cb2();
    }
}
```

### 8.4 swap

```cpp
#include <iostream>
#include "sy_callback.hpp"

int main() {
    sy_callback::callback<void()> cb1 = 
        [](){ std::cout << "This text will be printed first \n"; };
		    
    sy_callback::callback<void()> cb2 = 
        [](){ std::cout << "This text will be printed second\n"; };

    std::cout << "\nBefore swap callback:\n";
    cb1();
    cb2();

    cb1.swap(cb2);

    std::cout << "\nAfter swap callback:\n";
    cb1();
    cb2();

    // Note:
    // Swapping callbacks across threads may cause race conditions
}
```

### 8.5 Assign

```cpp
#include <iostream>
#include "sy_callback.hpp"

struct Functor {
    void operator()() {
        std::cout << "I WANT YOU TO CALL ME\n";
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

    // Note:
    // Assigning callback across threads may also cause race conditions
}
```

### 8.6 target<T>()

```cpp
// Syntax

callback.target<T>()

// T : type you want to target
// target<T>() returns the original object if T matches 
// the type of the owned callback
```

### Example

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

    // Note: target always returns the original object, not a copy
}
```
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
