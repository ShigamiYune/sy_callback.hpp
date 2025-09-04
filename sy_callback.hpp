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
#include <typeindex>

namespace sy_callback {
    template<typename SIGNATURE> class callback;
    template<typename RETURN, typename... ARGS>
    class callback<RETURN(ARGS...)> {
        template <typename T, typename = void> struct is_functor : std::false_type {};
        template<typename...> 
        using my_void_t = void;
        template <typename T>               struct is_functor<T, my_void_t<decltype(&T::operator())>> : std::true_type {};
        template<typename T>                struct remove_all {
            using type = typename std::remove_cv<
                typename std::remove_pointer<
                    typename std::decay<T>::type
                >::type
            >::type;
        };
        template<typename F>                struct is_invocable_r {
        private:
            template<typename U>
            static auto test(int) -> typename std::is_convertible<
                decltype(std::declval<U>()(std::declval<ARGS>()...)),
                RETURN
            >::type;

            template<typename>
            static std::false_type test(...);

        public:
            static constexpr bool value = decltype(test<F>(0))::value;
        };
        template<typename C, typename O>    struct is_valid_object {
            static constexpr bool value =
                std::is_same<
                    typename remove_all<O>::type,
                    C
                >::value;
        };   
                                            struct invoke_r {
            union {
                RETURN _return;
                std::uintptr_t _object;
            };
        
            invoke_r()                              : _object(0)                {}
            invoke_r(const std::uintptr_t& object)  : _object(object)           {}
            invoke_r(const RETURN& ret)             : _return(ret)              {}
            invoke_r(RETURN&& ret)                  : _return(std::move(ret))   {}
        };
        enum                                struct key_t : std::uint8_t{ 
            copy, invoke, destroy, compare 
        };
        using args_t            = std::tuple<ARGS&...>;
        using invoke_t          = invoke_r(*)(key_t, const std::uintptr_t&, args_t*);

        template<std::size_t... I>                  struct detail_invoke_table {
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) >
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }

            static invoke_r invoke_pointer_not_noexcept(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    return (reinterpret_cast<RETURN(*)(ARGS...)>(object))(std::get<I>(*tuple_ptr)...);
                } else {
                    return object;
                }
                return {};
            }

            template<typename ANY_T>
            static invoke_r invoke_any(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    return (*reinterpret_cast<ANY_T*>(object))(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::copy && std::is_copy_constructible<ANY_T>::value) {
                    ANY_T* orig = reinterpret_cast<ANY_T*>(object);
                    ANY_T* copy_obj = new ANY_T(*orig);
                    return reinterpret_cast<std::uintptr_t>(copy_obj);
                }
                else if (key == key_t::destroy) {
                    delete reinterpret_cast<ANY_T*>(object);
                }
                return {};
            }  
#if __cplusplus >= 201703L
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) & noexcept> 
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }

            static invoke_r invoke_pointer_noexcept(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    return (reinterpret_cast<RETURN(*)(ARGS...) noexcept>(object))(std::get<I>(*tuple_ptr)...);
                } else {
                    return object;
                }
                return {};
            }
#endif       
        };
        template<std::size_t N, std::size_t... I>   struct unpack_size : unpack_size<N-1, N-1, I...> { };
        template<std::size_t... I>                  struct unpack_size<0, I...> {
            using invoke_table = detail_invoke_table<I...>;
        };
        template<typename CLASS>                    struct target_func{
        private:
            std::uintptr_t _object;
            invoke_t _invoke;

            friend class callback;

            target_func(const std::uintptr_t& object, invoke_t invoke) : _object(object), _invoke(invoke) {}
        public:
            operator bool() {
                return _object;
            }
            CLASS* operator->() {
                return reinterpret_cast<CLASS*>(_object);
            }
            inline RETURN operator()(ARGS... args) const { 
                args_t _args{args...};
                return _invoke(key_t::invoke, _object, &_args)._return; 
            }
            target_func& operator*() { return *this; }
            const target_func& operator*() const { return *this; }
        };

        static invoke_r invoke_nothing(key_t key, const std::uintptr_t&, args_t*) { 
            if(key != key_t::destroy) throw std::bad_function_call();
            return {}; 
        }

        using invoke_table      = typename unpack_size<sizeof...(ARGS)>::invoke_table;

        std::uintptr_t  _object = 0;
        invoke_t        _invoke = &invoke_nothing;
    public:
#pragma region MAKE
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) , typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        
        template<RETURN(*FUNC)(ARGS...)>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_table::invoke_pointer_not_noexcept;
            return callback;
        } 
        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...)) {
            callback<RETURN(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_table:: invoke_pointer_not_noexcept;
            return callback;
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
            !std::is_same<D_ANY_T, callback>::value &&
            is_invocable_r<ANY_T>::value,
        callback<RETURN(ARGS...)>>::type make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            callback._invoke    = &invoke_table::template invoke_any<D_ANY_T>;
            return callback;
        }
        
        static callback<RETURN(ARGS...)> make(callback<RETURN(ARGS...)>&& func) {
            return std::forward<callback<RETURN(ARGS...)>>(func);
        }
#if __cplusplus >= 201703L
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        
        template<RETURN(*FUNC)(ARGS...) noexcept>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_table::invoke_pointer_noexcept;
            return callback;
        } 
        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...) noexcept) {
            callback<RETURN(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_table:: invoke_pointer_noexcept;
            return callback;
        }
#endif
#pragma endregion 
#pragma region CONSTRUCTOR
        callback() noexcept : _object(0), _invoke(&invoke_nothing){}
        callback(const callback& other) {
            if (other._invoke == &invoke_nothing) {
                _object = 0;
                _invoke = &invoke_nothing;
                return;
            }

            std::uintptr_t object = other._invoke(key_t::copy, other._object, nullptr)._object;

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
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
        }
        callback(RETURN(*func)(ARGS...)) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
        }  
#if __cplusplus >= 201703L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
        }
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
        }
        callback(RETURN(*func)(ARGS...) noexcept) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
        } 
#elif __cplusplus >= 201103L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
        }

#endif
        ~callback() { 
            _invoke(key_t::destroy, _object, nullptr);
            _object = 0;
            _invoke = &invoke_nothing;
        }
#pragma endregion
#pragma region COPY_MOVE_ASSIGN_TARGET
        template<typename ANY_T,
                typename = typename std::enable_if<
                    std::is_pointer<ANY_T>::value &&
                    std::is_function<typename std::remove_pointer<ANY_T>::type>::value &&
                    is_invocable_r<ANY_T>::value
                >::type>
        ANY_T target() {
            if (_invoke == &invoke_table::invoke_pointer_not_noexcept)
                return reinterpret_cast<RETURN(*)(ARGS...)>(_object);
        #if __cplusplus >= 201703L
            else if (_invoke == &invoke_table::invoke_pointer_noexcept)
                return reinterpret_cast<RETURN(*)(ARGS...) noexcept>(_object);
        #endif
            return nullptr;
        }

        template<typename ANY_T,
                typename = typename std::enable_if<
                    !std::is_pointer<ANY_T>::value &&
                    is_invocable_r<ANY_T>::value
                >::type>
        ANY_T* target() {
            if (_invoke == &invoke_table::template invoke_any<ANY_T>)
                return reinterpret_cast<ANY_T*>(_object);
            return nullptr;
        }

        template<
            typename CLASS,
            typename std::enable_if<std::is_class<CLASS>::value && 
            !is_functor<CLASS>::value, int>::type = 0
        >
        target_func<CLASS> target() {
            std::type_index type = typeid(typename remove_all<CLASS>::type);
            if (_invoke(key_t::compare, reinterpret_cast<std::uintptr_t>(&type), nullptr)._object) 
                return target_func<CLASS>(_object, _invoke);
            return target_func<CLASS>(0, &invoke_nothing);
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
            !std::is_same<D_ANY_T, callback>::value &&
            is_invocable_r<ANY_T>::value,
        callback&>::type
        operator=(ANY_T&& func) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
            return *this;
        }

        callback& operator=(RETURN(*func)(ARGS...)) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
            return *this;
        }

#if __cplusplus >= 201703L
        callback& operator=(RETURN(*func)(ARGS...) noexcept) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
            return *this;
        }
#endif

        callback& operator=(const callback& other) {
            if (this == &other || other._invoke == &invoke_nothing) return *this;

            std::uintptr_t object = other._invoke(key_t::copy, other._object, nullptr)._object;

            if(!object) {
                _object = 0;
                _invoke = &invoke_nothing;
                return *this;
            }
            
            _object = object;
            _invoke = other._invoke;

            return *this;
        }

        callback& operator=(callback&& other) noexcept {
            if (this != &other) {
                _invoke(key_t::destroy, _object, nullptr);

                _object = other._object;
                _invoke = other._invoke;

                other._object = 0;
                other._invoke = &invoke_nothing;
            }
            return *this;
        }
#pragma endregion

        inline bool isCallable() const { return _invoke != &invoke_nothing; }
        inline operator bool() const { return _invoke != &invoke_nothing; }

        inline RETURN invoke(ARGS... args) const { 
            args_t _args{args...};
            return _invoke(key_t::invoke, _object, &_args)._return;
        }
        inline RETURN operator()(ARGS... args) const {
            args_t _args{args...};
            return _invoke(key_t::invoke, _object, &_args)._return;
        }
        
        void swap(callback& other) {
            std::swap(_object, other._object);
            std::swap(_invoke, other._invoke);
        }
        void reset() {
            _invoke(key_t::destroy, _object, nullptr);
            _object = 0;
            _invoke = &invoke_nothing;
        }
    };
    
    template<typename... ARGS>
    class callback<void(ARGS...)> {
        template <typename T, typename = void> struct is_functor : std::false_type {};
        template<typename...> 
        using my_void_t = void;
        template <typename T>               struct is_functor<T, my_void_t<decltype(&T::operator())>> : std::true_type {};
        template<typename T>                struct remove_all {
            using type = typename std::remove_cv<
                typename std::remove_pointer<
                    typename std::decay<T>::type
                >::type
            >::type;
        };
        template<typename F>                struct is_invocable_r {
        private:
            template<typename U>
            static auto test(int) -> typename std::is_convertible<
                decltype(std::declval<U>()(std::declval<ARGS>()...)),
                void
            >::type;

            template<typename>
            static std::false_type test(...);

        public:
            static constexpr bool value = decltype(test<F>(0))::value;
        };
        template<typename C, typename O>    struct is_valid_object {
            static constexpr bool value =
                std::is_same<
                    typename remove_all<O>::type,
                    C
                >::value;
        };   
                                            struct invoke_r {
            union {
                std::uintptr_t _object;
            };
        
            invoke_r()                              : _object(0)                {}
            invoke_r(const std::uintptr_t& object)  : _object(object)           {}
        };
        enum                                struct key_t : std::uint8_t{ 
            copy, invoke, destroy, compare 
        };
        using args_t            = std::tuple<ARGS&...>;
        using invoke_t          = invoke_r(*)(key_t, const std::uintptr_t&, args_t*);

        template<std::size_t... I>                  struct detail_invoke_table {
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) >
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &&>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }

            static invoke_r invoke_pointer_not_noexcept(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<void(*)(ARGS...)>(object))(std::get<I>(*tuple_ptr)...);
                } else {
                    return object;
                }
                return {};
            }

            template<typename ANY_T>
            static invoke_r invoke_any(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (*reinterpret_cast<ANY_T*>(object))(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::copy && std::is_copy_constructible<ANY_T>::value) {
                    ANY_T* orig = reinterpret_cast<ANY_T*>(object);
                    ANY_T* copy_obj = new ANY_T(*orig);
                    return reinterpret_cast<std::uintptr_t>(copy_obj);
                }
                else if (key == key_t::destroy) {
                    delete reinterpret_cast<ANY_T*>(object);
                }
                return {};
            }  
#if __cplusplus >= 201703L
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) & noexcept> 
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile & noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }
            template<typename CLASS, void(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile && noexcept>
            static invoke_r invoke_member(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<CLASS*>(object)->*FUNC)(std::get<I>(*tuple_ptr)...);
                } 
                else if (key == key_t::compare){
                    std::type_index& type = *reinterpret_cast<std::type_index*>(object);
                    return std::uintptr_t(type == typeid(typename remove_all<CLASS>::type));
                }
                else {
                    return object;
                }
                return {};
            }

            static invoke_r invoke_pointer_noexcept(key_t key, const std::uintptr_t& object, args_t* tuple_ptr) {
                if (key == key_t::invoke) {
                    (reinterpret_cast<void(*)(ARGS...) noexcept>(object))(std::get<I>(*tuple_ptr)...);
                } else {
                    return object;
                }
                return {};
            }
#endif       
        };
        template<std::size_t N, std::size_t... I>   struct unpack_size : unpack_size<N-1, N-1, I...> { };
        template<std::size_t... I>                  struct unpack_size<0, I...> {
            using invoke_table = detail_invoke_table<I...>;
        };
        template<typename CLASS>                    struct target_func{
        private:
            std::uintptr_t _object;
            invoke_t _invoke;

            friend class callback;

            target_func(const std::uintptr_t& object, invoke_t invoke) : _object(object), _invoke(invoke) {}
        public:
            operator bool() {
                return _object;
            }
            CLASS* operator->() {
                return reinterpret_cast<CLASS*>(_object);
            }
            inline void operator()(ARGS... args) const { 
                args_t _args{args...};
                _invoke(key_t::invoke, _object, &_args); 
            }
            target_func& operator*() { return *this; }
            const target_func& operator*() const { return *this; }
        };

        static invoke_r invoke_nothing(key_t key, const std::uintptr_t&, args_t*) { 
            if(key != key_t::destroy) throw std::bad_function_call();
            return {}; 
        }

        using invoke_table      = typename unpack_size<sizeof...(ARGS)>::invoke_table;

        std::uintptr_t  _object = 0;
        invoke_t        _invoke = &invoke_nothing;
    public:
#pragma region MAKE
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) , typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        
        template<void(*FUNC)(ARGS...)>
        static callback<void(ARGS...)> make() {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_table::invoke_pointer_not_noexcept;
            return callback;
        } 
        static callback<void(ARGS...)> make(void(*func)(ARGS...)) {
            callback<void(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_table:: invoke_pointer_not_noexcept;
            return callback;
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
            !std::is_same<D_ANY_T, callback>::value &&
            is_invocable_r<ANY_T>::value,
        callback<void(ARGS...)>>::type make(ANY_T&& func) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            callback._invoke    = &invoke_table::template invoke_any<D_ANY_T>;
            return callback;
        }
        
        static callback<void(ARGS...)> make(callback<void(ARGS...)>&& func) {
            return std::forward<callback<void(ARGS...)>>(func);
        }
#if __cplusplus >= 201703L
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, void(CLASS::*FUNC)(ARGS...) const volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<void(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._invoke    = &invoke_table::template invoke_member<OBJ, FUNC>;
            return callback;
        }
        
        template<void(*FUNC)(ARGS...) noexcept>
        static callback<void(ARGS...)> make() {
            callback<void(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._invoke    = &invoke_table::invoke_pointer_noexcept;
            return callback;
        } 
        static callback<void(ARGS...)> make(void(*func)(ARGS...) noexcept) {
            callback<void(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._invoke = &invoke_table:: invoke_pointer_noexcept;
            return callback;
        }
#endif
#pragma endregion 
#pragma region CONSTRUCTOR
        callback() noexcept : _object(0), _invoke(&invoke_nothing){}
        callback(const callback& other) {
            if (other._invoke == &invoke_nothing) {
                _object = 0;
                _invoke = &invoke_nothing;
                return;
            }

            std::uintptr_t object = other._invoke(key_t::copy, other._object, nullptr)._object;

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
                std::is_convertible<D_ANY_T, void(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
        }
        callback(void(*func)(ARGS...)) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
        }  
#if __cplusplus >= 201703L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, void(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
        }
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, void(*)(ARGS...)>::value &&
                !std::is_convertible<D_ANY_T, void(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
        }
        callback(void(*func)(ARGS...) noexcept) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
        } 
#elif __cplusplus >= 201103L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, void(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(
                new D_ANY_T(std::forward<ANY_T>(func))
            );
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
        }

#endif
        ~callback() { 
            _invoke(key_t::destroy, _object, nullptr);
            _object = 0;
            _invoke = &invoke_nothing;
        }
#pragma endregion
#pragma region COPY_MOVE_ASSIGN_TARGET
        template<typename ANY_T,
                typename = typename std::enable_if<
                    std::is_pointer<ANY_T>::value &&
                    std::is_function<typename std::remove_pointer<ANY_T>::type>::value &&
                    is_invocable_r<ANY_T>::value
                >::type>
        ANY_T target() {
            if (_invoke == &invoke_table::invoke_pointer_not_noexcept)
                return reinterpret_cast<void(*)(ARGS...)>(_object);
        #if __cplusplus >= 201703L
            else if (_invoke == &invoke_table::invoke_pointer_noexcept)
                return reinterpret_cast<void(*)(ARGS...) noexcept>(_object);
        #endif
            return nullptr;
        }

        template<typename ANY_T,
                typename = typename std::enable_if<
                    !std::is_pointer<ANY_T>::value &&
                    is_invocable_r<ANY_T>::value
                >::type>
        ANY_T* target() {
            if (_invoke == &invoke_table::template invoke_any<ANY_T>)
                return reinterpret_cast<ANY_T*>(_object);
            return nullptr;
        }

        template<
            typename CLASS,
            typename std::enable_if<std::is_class<CLASS>::value && 
            !is_functor<CLASS>::value, int>::type = 0
        >
        target_func<CLASS> target() {
            std::type_index type = typeid(typename remove_all<CLASS>::type);
            if (_invoke(key_t::compare, reinterpret_cast<std::uintptr_t>(&type), nullptr)._object) 
                return target_func<CLASS>(_object, _invoke);
            return target_func<CLASS>(0, &invoke_nothing);
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
            !std::is_same<D_ANY_T, callback>::value &&
            is_invocable_r<ANY_T>::value,
        callback&>::type
        operator=(ANY_T&& func) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            _invoke = &invoke_table::template invoke_any<D_ANY_T>;
            return *this;
        }

        callback& operator=(void(*func)(ARGS...)) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_not_noexcept;
            return *this;
        }

#if __cplusplus >= 201703L
        callback& operator=(void(*func)(ARGS...) noexcept) {
            _invoke(key_t::destroy, _object, nullptr);

            _object = reinterpret_cast<std::uintptr_t>(func);
            _invoke = &invoke_table::invoke_pointer_noexcept;
            return *this;
        }
#endif

        callback& operator=(const callback& other) {
            if (this == &other || other._invoke == &invoke_nothing) return *this;

            std::uintptr_t object = other._invoke(key_t::copy, other._object, nullptr)._object;

            if(!object) {
                _object = 0;
                _invoke = &invoke_nothing;
                return *this;
            }
            
            _object = object;
            _invoke = other._invoke;

            return *this;
        }

        callback& operator=(callback&& other) noexcept {
            if (this != &other) {
                _invoke(key_t::destroy, _object, nullptr);

                _object = other._object;
                _invoke = other._invoke;

                other._object = 0;
                other._invoke = &invoke_nothing;
            }
            return *this;
        }
#pragma endregion

        inline bool isCallable() const { return _invoke != &invoke_nothing; }
        inline operator bool() const { return _invoke != &invoke_nothing; }

        inline void invoke(ARGS... args) const { 
            args_t _args{args...};
            _invoke(key_t::invoke, _object, &_args);
        }
        inline void operator()(ARGS... args) const {
            args_t _args{args...};
            _invoke(key_t::invoke, _object, &_args);
        }
        
        void swap(callback& other) {
            std::swap(_object, other._object);
            std::swap(_invoke, other._invoke);
        }
        void reset() {
            _invoke(key_t::destroy, _object, nullptr);
            _object = 0;
            _invoke = &invoke_nothing;
        }
    };

}
#endif
