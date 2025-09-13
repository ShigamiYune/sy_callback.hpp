/*
 * Project Name: sy_callback.hpp
 * Author: ShigamiYune
 * Version: 1.6.1
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

#include <functional>
#include <type_traits>
#include <typeindex>

namespace sy_callback {
    template<typename SIGNATURE> class callback;
    template<typename RETURN, typename... ARGS>
    class callback<RETURN(ARGS...)> {
        template <typename T, typename = void>  struct      is_functor : std::false_type {};
        template<typename...>                   using       my_void_t = void;
        template<typename T>                    struct      is_functor<T, my_void_t<decltype(&T::operator())>> : std::true_type {};
        template<typename T>                    struct      remove_all {
            using type = typename std::remove_cv<
                typename std::remove_pointer<
                    typename std::decay<T>::type
                >::type
            >::type;
        };
        template<typename F>                    struct      is_invocable_r {
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
        template<typename C, typename O>        struct      is_valid_object {
            static constexpr bool value =
                std::is_same<
                    typename remove_all<O>::type,
                    C
                >::value;
        };   
        
        enum struct key_t : std::uint8_t{ 
            copy, destroy, get_name 
        };
        
        using func_invoke_t = RETURN(*)(const std::uintptr_t&, ARGS...);
        using func_life_t = std::uintptr_t(*)(key_t, const std::uintptr_t&);
        using func_thunk_t = std::uintptr_t(*)(bool);

        template<typename CLASS>                struct      target_func{
        private:
            std::uintptr_t _object;
            func_thunk_t _thunk;

            friend class callback;

            target_func(const std::uintptr_t& object, func_thunk_t thunk) : _object(object), _thunk(thunk) {}
        public:
            operator bool() {
                return _object;
            }
            CLASS* operator->() {
                return reinterpret_cast<CLASS*>(_object);
            }
            inline RETURN operator()(ARGS... args) const { 
                return (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...); 
            }
            target_func& operator*() { return *this; }
            const target_func& operator*() const { return *this; }
        };

        std::uintptr_t _object;
        func_thunk_t _thunk;

#pragma region INVOKE TABLE
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

        static RETURN invoke_pointer_not_noexcept(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<RETURN(*)(ARGS...)>(object))(args...);
        }
        
        template<typename ANY_T>
        static RETURN invoke_any(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<ANY_T*>(object))(args...);
        }

        static RETURN invoke_nothing(const std::uintptr_t&, ARGS...) { throw std::bad_function_call(); }

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

        static RETURN invoke_pointer_noexcept(const std::uintptr_t& object, ARGS... args) {
            return (*reinterpret_cast<RETURN(*)(ARGS...) noexcept>(object))(args...);
        }

#endif
#pragma endregion
#pragma region LIFE TABLE
        template<typename CLASS> 
        static std::uintptr_t life_member(key_t type, const std::uintptr_t& object) {
            return object;
        }     
        template<typename ANY_T>
        static std::uintptr_t life_any(key_t type, const std::uintptr_t& object) {    
            if (type == key_t::copy) {
                if (!std::is_copy_constructible<ANY_T>::value) return 0;

                ANY_T* orig = reinterpret_cast<ANY_T*>(object);
                ANY_T* copy_obj = new ANY_T(*orig);
                return reinterpret_cast<std::uintptr_t>(copy_obj);
            }
            else if (type == key_t::destroy ) delete reinterpret_cast<ANY_T*>(object);
            return 0;
        }
        static std::uintptr_t life_global(key_t type, const std::uintptr_t& object) {
            return object;
        }
        static std::uintptr_t life_nothing(key_t type, const std::uintptr_t&) { return 0; }
#pragma endregion
#pragma region THUNK TABLE
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) > 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &&> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &&> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &&> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &&> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        
        static std::uintptr_t thunk_pointer_not_noexcept(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_pointer_not_noexcept) 
                            : reinterpret_cast<std::uintptr_t>(&life_global);
        }

#if __cplusplus >= 201703L
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) & noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const & noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile & noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile & noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) && noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const && noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile && noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile && noexcept> 
        static std::uintptr_t thunk_member(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_member<CLASS, FUNC>) 
                            : reinterpret_cast<std::uintptr_t>(&life_member<typename remove_all<CLASS>::type>);
        } 

        static std::uintptr_t thunk_pointer_noexcept(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_pointer_noexcept) 
                            : reinterpret_cast<std::uintptr_t>(&life_global);
        }

#endif
        template<typename ANY_T>
        static std::uintptr_t thunk_any(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_any<ANY_T>) 
                            : reinterpret_cast<std::uintptr_t>(&life_any<ANY_T>);
        }

        static std::uintptr_t thunk_nothing(bool invoke) {
            return invoke   ? reinterpret_cast<std::uintptr_t>(&invoke_nothing) 
                            : reinterpret_cast<std::uintptr_t>(&life_nothing);
        }
#pragma endregion
    public:
#pragma region MAKE
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) , typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile &&, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        
        template<RETURN(*FUNC)(ARGS...)>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._thunk     = &thunk_pointer_not_noexcept;
            return callback;
        } 
        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...)) {
            callback<RETURN(ARGS...)> callback;
            callback._object = reinterpret_cast<std::uintptr_t>(func);
            callback._thunk = &thunk_pointer_not_noexcept;
            return callback;
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value,
        callback<RETURN(ARGS...)>>::type make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(+func);
            callback._thunk     = &thunk_pointer_not_noexcept;
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
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile & noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        template<typename CLASS, RETURN(CLASS::*FUNC)(ARGS...) const volatile && noexcept, typename OBJ>
        static typename std::enable_if<is_valid_object<CLASS, OBJ>::value, callback<RETURN(ARGS...)>>::type
        make(OBJ*&& object) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(std::forward<OBJ*>(object));
            callback._thunk     = &thunk_member<OBJ, FUNC>;;
            return callback;
        }
        
        template<RETURN(*FUNC)(ARGS...) noexcept>
        static callback<RETURN(ARGS...)> make() {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(FUNC);
            callback._thunk     = &thunk_pointer_noexcept;
            return callback;
        } 
        static callback<RETURN(ARGS...)> make(RETURN(*func)(ARGS...) noexcept) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(func);
            callback._thunk     = &thunk_pointer_noexcept;
            return callback;
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
        callback<RETURN(ARGS...)>>::type make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            callback._thunk     = &thunk_any<D_ANY_T>;
            return callback;
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
        callback<RETURN(ARGS...)>>::type make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(+func);
            callback._thunk     = &thunk_pointer_noexcept;
            return callback;
        }
#elif __cplusplus >= 201103L
        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        static typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value,
        callback<RETURN(ARGS...)>>::type make(ANY_T&& func) {
            callback<RETURN(ARGS...)> callback;
            callback._object    = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            callback._thunk     = &thunk_any<D_ANY_T>;
            return callback;
        }
#endif
#pragma endregion 
#pragma region CONSTRUCTOR
        callback() noexcept : _object(0), _thunk(&thunk_nothing){}
        callback(const callback& other) {
            if (other._thunk == &thunk_nothing) {
                _object = 0;
                _thunk = &thunk_nothing;
                return;
            }

            std::uintptr_t object = (*reinterpret_cast<func_life_t>(other._thunk(false)))(key_t::copy, other._object);

            if(!object) {
                _object = 0;
                _thunk = &thunk_nothing;
                return;
            }

            _object = object;
            _thunk = other._thunk;
        }
        callback(callback&& other) noexcept {
            _object = other._object;
            _thunk = other._thunk;

            other._object = 0;
            other._thunk = &thunk_nothing;
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
            _thunk = &thunk_pointer_not_noexcept;
        }
        callback(RETURN(*func)(ARGS...)) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _thunk = &thunk_pointer_not_noexcept;
        }  
#if __cplusplus >= 201703L
        template<
            typename ANY_T,
            typename D_ANY_T = typename std::decay<ANY_T>::type,
            typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value,
                int
            >::type = 0
        >
        callback(ANY_T&& func) {
            _object = reinterpret_cast<std::uintptr_t>(+func);
            _thunk = &thunk_pointer_noexcept;
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
            _thunk = &thunk_any<D_ANY_T>;
        }
        callback(RETURN(*func)(ARGS...) noexcept) {
            _object = reinterpret_cast<std::uintptr_t>(func);
            _thunk = &thunk_pointer_noexcept;
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
            _thunk = &thunk_any<D_ANY_T>;
        }

#endif
        ~callback() { 
            if(_thunk == &thunk_nothing) return;

            (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
            _object = 0;
            _thunk = &thunk_nothing;
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
            if (_thunk == &thunk_pointer_not_noexcept)
                return reinterpret_cast<RETURN(*)(ARGS...)>(_object);
        #if __cplusplus >= 201703L
            else if (_thunk == &thunk_pointer_noexcept)
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
            if (_thunk == &thunk_any<ANY_T>)
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
            if (&life_member<typename remove_all<CLASS>::type> == reinterpret_cast<func_life_t>(_thunk(false))) 
                return target_func<CLASS>(_object, _thunk);
            return target_func<CLASS>(0, &thunk_nothing);
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
            !std::is_same<D_ANY_T, callback>::value &&
            is_invocable_r<ANY_T>::value,
        callback&>::type
        operator=(ANY_T&& func) {
            if(_thunk != &thunk_nothing){
                (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
            }

            _object = reinterpret_cast<std::uintptr_t>(new D_ANY_T(std::forward<ANY_T>(func)));
            _thunk = &thunk_any<D_ANY_T>;
            return *this;
        }

        callback& operator=(RETURN(*func)(ARGS...)) {
            if(_thunk != &thunk_nothing){
                (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
            }

            _object = reinterpret_cast<std::uintptr_t>(func);
            _thunk = &thunk_pointer_not_noexcept;
            return *this;
        }

#if __cplusplus >= 201703L
        callback& operator=(RETURN(*func)(ARGS...) noexcept) {
            if(_thunk != &thunk_nothing){
                (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
            }

            _object = reinterpret_cast<std::uintptr_t>(func);
            _thunk = &thunk_pointer_noexcept;
            return *this;
        }
#endif

        callback& operator=(const callback& other) {
            if (this == &other || other._thunk == &thunk_nothing) return *this;

            std::uintptr_t object = (*reinterpret_cast<func_life_t>(other._thunk(false)))(key_t::copy, other._object);

            if(!object) {
                _object = 0;
                _thunk = &thunk_nothing;
                return *this;
            }
            
            _object = object;
            _thunk = other._thunk;

            return *this;
        }

        callback& operator=(callback&& other) noexcept {
            if (this != &other) {
                if(_thunk != &thunk_nothing){
                    (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
                }

                _object = other._object;
                _thunk = other._thunk;

                other._object = 0;
                other._thunk = &thunk_nothing;
            }
            return *this;
        }
#pragma endregion
#pragma region INVOKE PREDICTION
                template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) > 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) &&> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const &&> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile &&> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile &&> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>)
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }

        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value, RETURN>::type
        invoke_prediction(ARGS... args){
            return _thunk == &thunk_pointer_not_noexcept
                ? (*reinterpret_cast<RETURN(*)(ARGS...)>(_object))(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }

#if __cplusplus >= 201703L
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) & noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const & noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile & noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile & noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) && noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const && noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) volatile && noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename CLASS, RETURN(remove_all<CLASS>::type::*FUNC)(ARGS...) const volatile && noexcept> 
        RETURN invoke_prediction(ARGS... args){
            return (_thunk == &thunk_member<CLASS, FUNC>) 
                ? (reinterpret_cast<CLASS*>(_object)->*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        
        RETURN invoke_prediction(RETURN(*FUNC)(ARGS...) noexcept, ARGS... args){
            return _thunk == &thunk_pointer_noexcept 
                ? (*FUNC)(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        
        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value, RETURN>::type
        invoke_prediction(ARGS... args){
            return _thunk == &thunk_pointer_noexcept
                ? (*reinterpret_cast<RETURN(*)(ARGS...) noexcept>(_object))(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...) noexcept>::value &&
                is_invocable_r<ANY_T>::value, RETURN>::type
        invoke_prediction(ARGS... args){
            return (_thunk == &thunk_any<ANY_T>)
                ? (*reinterpret_cast<ANY_T*>(_object))(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
#elif __cplusplus >= 201103L
        template<typename ANY_T, typename D_ANY_T = typename std::decay<ANY_T>::type>
        typename std::enable_if<
                !std::is_same<D_ANY_T, callback>::value &&
                !std::is_convertible<D_ANY_T, RETURN(*)(ARGS...)>::value &&
                is_invocable_r<ANY_T>::value, RETURN>::type
        invoke_prediction(ARGS... args){
            return (_thunk == &thunk_any<ANY_T>)
                ? (*reinterpret_cast<ANY_T*>(_object))(args...)
                : (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
#endif
#pragma endregion      
        inline bool isCallable() const { return _thunk != &thunk_nothing; }
        inline operator bool() const { return _thunk != &thunk_nothing; }

        inline RETURN invoke(ARGS... args) const { 
            return (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        inline RETURN operator()(ARGS... args) const {
            return (*reinterpret_cast<func_invoke_t>(_thunk(true)))(_object, args...);
        }
        
        void swap(callback& other) {
            std::swap(_object, other._object);
            std::swap(_thunk, other._thunk);
        }
        void reset() {
            if(_thunk == &thunk_nothing) return;

            (*reinterpret_cast<func_life_t>(_thunk(false)))(key_t::destroy, _object);
            _object = 0;
            _thunk = &thunk_nothing;
        }
    };
}
#endif
