#include <iostream>
#include <functional>
#include <chrono>
#include "sy_callback.hpp"

struct MyClass {
    void member_func() {}
};

struct Functor {
    void operator()() {}
};

void global_func() {}

int main() {
    const int N = 10'000'000;

    MyClass obj;
    Functor f;
    int x = 42;

    // ===== Direct call =====
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) obj.member_func();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Direct member_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) global_func();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Direct global_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    auto lambda_no_capture = [](){};
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) lambda_no_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Direct lambda no capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    auto lambda_with_capture = [x](){ (void)x; };
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) lambda_with_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Direct lambda with capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) f();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Direct functor: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    // ===== sy_callback =====
    auto cb_member_inline = sy_callback::callback<void()>::make<MyClass, &MyClass::member_func>(&obj);
    auto cb_member_bind = std::bind(&MyClass::member_func, &obj);
    auto cb_global = sy_callback::callback<void()>::make(global_func);
    auto cb_lambda_no_capture = sy_callback::callback<void()>::make(lambda_no_capture);
    auto cb_lambda_with_capture = sy_callback::callback<void()>::make(lambda_with_capture);
    auto cb_functor = sy_callback::callback<void()>::make(f);

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_member_inline();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback inline_member_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_member_bind();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback bind_member_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_global();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback global_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_lambda_no_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback lambda no capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_lambda_with_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback lambda with capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) cb_functor();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "sy_callback functor: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    // ===== std::function =====
    std::function<void()> std_member = std::bind(&MyClass::member_func, &obj);
    std::function<void()> std_global = global_func;
    std::function<void()> std_lambda_no_capture = lambda_no_capture;
    std::function<void()> std_lambda_with_capture = lambda_with_capture;
    std::function<void()> std_functor = f;

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) std_member();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::function bind_member_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) std_global();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::function global_func: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) std_lambda_no_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::function lambda no capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) std_lambda_with_capture();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::function lambda with capture: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; ++i) std_functor();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::function functor: "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms\n";

    return 0;
}
