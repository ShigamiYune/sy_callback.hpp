/*
 * Project Name: sy_callback.hpp
 * Author: ShigamiYune
 * Copyright 2025 ShigamiYune
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#ifndef SY_CALLBACK_HPP
#define SY_CALLBACK_HPP

#include <cassert>
#include <functional>
#include <type_traits>

namespace sy_callback {
    template<typename SIGNATURE> class callback;
    template<typename RETURN, typename... ARGS>
    class callback<RETURN(ARGS...)> {
        template<typename F, typename R, typename... Args>
        struct is_invocable_r {
        private:
            template<typename U>
            static auto test(int) -> typename std::is_convertible<
                decltype(std::declval<U>()(std::declval<Args>()...)),
                R
            >::type;

            template<typename>
            static std::false_type test(...);

        public:
            static constexpr bool value = decltype(test<F>(0))::value;
        };

        template<typename C, typename O>
        struct is_valid_object {            
            static constexpr bool value = 
            std::is_same<
                typename std::remove_cv<
                    typename std::remove_pointer<
                        typename std::decay<O>::type
                    >::type
                >::type, C
            >::value;
        };
        
        template<typename C>
        struct remove_all {
            using type = 
            typename std::remove_cv<
                typename std::remove_pointer<
                    typename std::decay<C>::type
                >::type
            >::type;
        };

        enum class type_key{
            destroy, copy
        };
        template <typename T, typename = void>
        struct is_functor : std::false_type {};

        template<typename...>
        using my_void_t = void;

        template <typename T>
        struct is_functor<T, my_void_t<decltype(&T::operator())>> : std::true_type {};

        using func_invoke_t = RETURN(*)(const std::uintptr_t&, ARGS...);
        using func_life_t = std::uintptr_t(*)(type_key, const std::uintptr_t&);

        std::uintptr_t _object;
        func_invoke_t _invoke;    
        func_life_t _life;

        template<typename CLASS>
        struct target_f{
        private:
            std::uintptr_t _object;
            func_invoke_t _invoke;

            friend class callback;

            target_f(const std::uintptr_t& object, func_invoke_t invoke) : _object(object), _invoke(invoke) {}
        public:
            operator bool() {
                return _object;
            }
            CLASS* operator->() {
                return reinterpret_cast<CLASS*>(_object);
            }
            inline RETURN operator()(ARGS... args) const { return _invoke(_object, args...); }
            target_f& operator*() { return *this; }
            const target_f& operator*() const { return *this; }
        };
        
        // INVOKE MEMBER ----------------------------------------------------------------
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) > 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &&> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &&> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &&> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &&> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        
        // LIFE MEMBER ------------------------------------------------------------------
        template<typename CLASS>
        static std::uintptr_t life_member(type_key type, const std::uintptr_t& object) {
            return object;
        }        
                
        // INVOKE GLOBAL ----------------------------------------------------------------
        static RETURN invoke_global_not_noexcept(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<RETURN(*)(ARGS...)>(object))(args...);
        }
        // INVOKE LIFE ------------------------------------------------------------------
        static std::uintptr_t life_global(type_key type, const std::uintptr_t& object) {
            return object;
        }     
#if __cplusplus >= 201703L
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) & noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const & noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile & noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile & noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) && noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const && noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile && noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile && noexcept> 
        static RETURN invoke_member(const std::uintptr_t& object, ARGS... args) {
            return (reinterpret_cast<CLASS*>(object)->*FUNC)(args...);
        }

        static RETURN invoke_global_noexcept(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<RETURN(*)(ARGS...) noexcept>(object))(args...);
        }   
#endif 
        template<typename ANY_T>
        static RETURN invoke_any(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<ANY_T*>(object))(args...);
        }
        template<typename ANY_T>
        static std::uintptr_t life_any(type_key type, const std::uintptr_t& object) {    
            if (type == type_key::copy) {
                if (!std::is_copy_constructible<ANY_T>::value) return 0;

                ANY_T* orig = reinterpret_cast<ANY_T*>(object);
                ANY_T* copy_obj = new ANY_T(*orig);
                return reinterpret_cast<std::uintptr_t>(copy_obj);
            }
            else if (type == type_key::destroy ) delete reinterpret_cast<ANY_T*>(object);
            return 0;
        }

        static RETURN invoke_nothing(const std::uintptr_t&, ARGS...) { throw std::bad_function_call(); }

        static std::uintptr_t life_nothing(const type_key, const std::uintptr_t&) { return 0; }
        
    public:
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) , typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }

#if __cplusplus >= 201703L
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(object);
            callback._invoke    = &invoke_member<OBJ, FUNC>;
            callback._life      = &life_member<CLASS>;
            return callback;
        }
#endif
        template<RETURN(*FUNC)(ARGS...)>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_global_not_noexcept;
            callback._life      = &life_global;
            return callback;
        } 
#if __cplusplus >= 201703L
        template<RETURN(*FUNC)(ARGS...) noexcept>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_global_noexcept;
            callback._life      = &life_global;
            return callback;
        }
#endif
        template<typename ANY_T>
        static typename std::enable_if<
            !std::is_same<typename std::decay<ANY_T>::type, callback>::value &&
            is_invocable_r<ANY_T, RETURN, ARGS...>::value,
        callback<RETURN(ARGS...)>>::type 
        make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(new typename std::decay<ANY_T>::type(std::forward<ANY_T>(func)));
            callback._invoke    = &invoke_any<typename std::decay<ANY_T>::type>;
            callback._life      = &life_any<typename std::decay<ANY_T>::type>;
            return callback;
        }

        static callback<RETURN(ARGS...)> make(callback<RETURN(ARGS...)>&& func) {
            return std::forward<callback<RETURN(ARGS...)>>(func);
        }

        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...)) {
            callback<RETURN(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_global_not_noexcept;
            callback._life   = &life_global;
            return callback;
        }
#if __cplusplus >= 201703L
        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...) noexcept) {
            callback<RETURN(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_global_noexcept;
            callback._life   = &life_global;
            return callback;
        }  
#endif
        callback() noexcept : _object(0), _invoke(&invoke_nothing){}
        callback(const callback& other) {
            if (other._invoke == &invoke_nothing) {
                _object = 0;
                _invoke = &invoke_nothing;
                return;
            }

            std::uintptr_t object = other._life(type_key::copy, other._object);

            if(!object) {
                _object = 0;
                _invoke = &invoke_nothing;
                return;
            }

            _object = object;
            _invoke = other._invoke;
        }
        callback(callback&& other) noexcept {
            _object = other._object;
            _invoke = other._invoke;

            other._object = 0;
            other._invoke = &invoke_nothing;
        }
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T, RETURN, ARGS...>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_global_not_noexcept;
            _life   = &life_global;
        }
        callback(RETURN(*func)(ARGS...)) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_global_not_noexcept;
            _life   = &life_global;
        }  
#if __cplusplus >= 201703L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T, RETURN, ARGS...>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_global_noexcept;
            _life   = &life_global;
        }
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T, RETURN, ARGS...>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_any<D_ANY_T>;
            _life   = &life_any<D_ANY_T>;
        }
        callback(RETURN(*func)(ARGS...) noexcept) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_global_noexcept;
            _life   = &life_global;
        } 
#elif __cplusplus >= 201103L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T, RETURN, ARGS...>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_any<D_ANY_T>;
            _life   = &life_any<D_ANY_T>;
        }

#endif
        ~callback() { 
            _life(type_key::destroy, _object);
            _object = 0;
            _invoke = &invoke_nothing;
            _life   = &life_nothing;
        }

        inline bool isCallable() const { return _invoke != &invoke_nothing; }
        inline operator bool() const { return _invoke != &invoke_nothing; }

        inline RETURN invoke(ARGS... args) const { return _invoke(_object, args...); }
        inline RETURN operator()(ARGS... args) const { return _invoke(_object, args...); }

        void swap(callback& other) {
            std::swap(_object, other._object);
            std::swap(_invoke, other._invoke);
            std::swap(_life, other._life);
        }
        void reset() {
            _life(type_key::destroy, _object);
            _object = 0;
            _invoke = &invoke_nothing;
            _life = &life_nothing;
        }

        template<typename ANY_T,
                typename = typename std::enable_if<
                    std::is_pointer<ANY_T>::value &&
                    std::is_function<typename std::remove_pointer<ANY_T>::type>::value &&
                    is_invocable_r<ANY_T, RETURN, ARGS...>::value
                >::type>
        ANY_T target() {
            if (_invoke == &invoke_global_not_noexcept)
                return reinterpret_cast<RETURN(*)(ARGS...)>(_object);
        #if __cplusplus >= 201703L
            else if (_invoke == &invoke_global_noexcept)
                return reinterpret_cast<RETURN(*)(ARGS...) noexcept>(_object);
        #endif
            return nullptr;
        }

        template<typename ANY_T,
                typename = typename std::enable_if<
                    !std::is_pointer<ANY_T>::value &&
                    is_invocable_r<ANY_T, RETURN, ARGS...>::value
                >::type>
        ANY_T* target() {
            if (_invoke == &invoke_any<ANY_T>)
                return reinterpret_cast<ANY_T*>(_object);
            return nullptr;
        }

        template<
            typename CLASS,
            typename std::enable_if<std::is_class<CLASS>::value && !is_functor<CLASS>::value, int>::type = 0
        >
        target_f<typename remove_all<CLASS>::type> target() {
            if (_life == &life_member<typename remove_all<CLASS>::type>) 
                return target_f<typename remove_all<CLASS>::type>(_object, _invoke);
            return target_f<typename remove_all<CLASS>::type>(0, &invoke_nothing);
        }

        template<typename ANY_T>
        typename std::enable_if<
            !std::is_same<typename std::decay<ANY_T>::type, callback>::value &&
            is_invocable_r<ANY_T, RETURN, ARGS...>::value,
        callback&>::type
        operator=(ANY_T&& func) {
            if (_life != &life_nothing) _life(type_key::destroy, _object);

            using T = typename std::decay<ANY_T>::type;
            _object = reinterpret_cast<std::uintptr_t>(new T(std::forward<ANY_T>(func)));
            _invoke = &invoke_any<T>;
            _life   = &life_any<T>;
            return *this;
        }

        callback& operator=(RETURN(*func)(ARGS...)) {
            if (_life != &life_nothing) _life(type_key::destroy, _object);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_global_not_noexcept;
            _life   = &life_global;
            return *this;
        }

#if __cplusplus >= 201703L
        callback& operator=(RETURN(*func)(ARGS...) noexcept) {
            if (_life != &life_nothing) _life(type_key::destroy, _object);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_global_noexcept;
            _life   = &life_global;
            return *this;
        }
#endif

        callback& operator=(const callback& other) {
            if (this == &other || other._life == &life_nothing) return *this;

            std::uintptr_t object = other._life(type_key::copy, other._object);
            assert(object != 0 && "Callback object is not copyable!");

            if(_life != &life_nothing) _life(type_key::destroy, _object);

            _object = object;
            _invoke = other._invoke;
            _life   = other._life;

            return *this;
        }

        callback& operator=(callback&& other) noexcept {
            if (this != &other) {
                _life(type_key::destroy, _object);

                _object = other._object;
                _invoke = other._invoke;
                _life   = other._life;

                other._object = 0;
                other._invoke = &invoke_nothing;
                other._life   = &life_nothing;
            }
            return *this;
        }   
    };
}
#endif
