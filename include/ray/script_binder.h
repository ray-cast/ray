// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_SCRIPT_BINDER_H_
#define _H_SCRIPT_BINDER_H_

#include <ray/script_types.h>

_NAME_BEGIN

namespace spw 
{
	template <typename T> struct Default {
		static T f(void) { return T(); }
	};
	template <typename T> struct Default<T &> {
		static T & f(void) { return *static_cast<T *>(0); }
	};

	template <typename T> class Proxy {
	public:
		T value;
		Proxy(T value) : value(value) {}
	private:
		Proxy(const Proxy &);
		Proxy & operator=(const Proxy &);
	};

	template <typename T, typename U, U T::*member_ptr>
	U getter(const std::shared_ptr<T> & ptr) {
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
			return Default<U>::f();
		}
		return (ptr.get())->*member_ptr;
	}

	template <typename T, typename U, U T::*member_ptr>
	const U & ref_getter(const std::shared_ptr<T> & ptr) {
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
			return Default<U &>::f();
		}
		return (ptr.get())->*member_ptr;
	}

	template <typename T, typename U, U T::*member_ptr>
	void getter_gen(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
		std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			new (gen->GetAddressOfReturnLocation()) Proxy<U>((ptr.get())->*member_ptr);
		}
	}

	template <typename T, typename U, U T::*member_ptr>
	void ref_getter_gen(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
		std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			new (gen->GetAddressOfReturnLocation()) Proxy<U &>((ptr.get())->*member_ptr);
		}
	}

	template <typename T, typename U, U T::*member_ptr>
	void setter(const std::shared_ptr<T> & ptr, U value) {
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			(ptr.get())->*member_ptr = value;
		}
	}

	template <typename T, typename U, U T::*member_ptr>
	void ref_setter(const std::shared_ptr<T> & ptr, const U & value) {
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			(ptr.get())->*member_ptr = value;
		}
	}

	template <typename T, typename U, U T::*member_ptr>
	void setter_gen(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
		std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			(ptr.get())->*member_ptr = static_cast<Proxy<U> *>(gen->GetAddressOfArg(0))->value;
		}
	}

	template <typename T, typename U, U T::*member_ptr>
	void ref_setter_gen(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
		std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
		if (!ptr) {
			AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
			ctx->SetException("Attempting to access member of a null pointer");
		}
		else {
			(ptr.get())->*member_ptr = static_cast<Proxy<U &> *>(gen->GetAddressOfArg(0))->value;
		}
	}

	template <typename T> struct Caller {};
	template <typename T> struct GenCaller {};

	template <typename T, typename R>
	struct Caller<R(T::*)(void)> {
		template <R(T::*fn)(void)>
		static R f(std::shared_ptr<T> & ptr) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)();
			}
		}
	};

	template <typename T, typename R>
	struct Caller<R(T::*)(void) const> {
		template <R(T::*fn)(void) const>
		static R f(std::shared_ptr<T> & ptr) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)();
			}
		}
	};

	template <typename T>
	struct GenCaller<void (T::*)(void)> {
		template <void (T::*fn)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)();
			}
		}
	};

	template <typename T, typename R>
	struct GenCaller<R(T::*)(void)> {
		template <R(T::*fn)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)());
			}
		}
	};

	template <typename T>
	struct GenCaller<void (T::*)(void) const> {
		template <void (T::*fn)(void) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)();
			}
		}
	};

	template <typename T, typename R>
	struct GenCaller<R(T::*)(void) const> {
		template <R(T::*fn)(void) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)());
			}
		}
	};

	template <typename T, typename R, typename A0>
	struct Caller<R(T::*)(A0)> {
		template <R(T::*fn)(A0)>
		static R f(std::shared_ptr<T> & ptr, A0 a0) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0);
			}
		}
	};

	template <typename T, typename R, typename A0>
	struct Caller<R(T::*)(A0) const> {
		template <R(T::*fn)(A0) const>
		static R f(std::shared_ptr<T> & ptr, A0 a0) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0);
			}
		}
	};

	template <typename T, typename A0>
	struct GenCaller<void (T::*)(A0)> {
		template <void (T::*fn)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value);
			}
		}
	};

	template <typename T, typename R, typename A0>
	struct GenCaller<R(T::*)(A0)> {
		template <R(T::*fn)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
			}
		}
	};

	template <typename T, typename A0>
	struct GenCaller<void (T::*)(A0) const> {
		template <void (T::*fn)(A0) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value);
			}
		}
	};

	template <typename T, typename R, typename A0>
	struct GenCaller<R(T::*)(A0) const> {
		template <R(T::*fn)(A0) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct Caller<R(T::*)(A0, A1)> {
		template <R(T::*fn)(A0, A1)>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct Caller<R(T::*)(A0, A1) const> {
		template <R(T::*fn)(A0, A1) const>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1);
			}
		}
	};

	template <typename T, typename A0, typename A1>
	struct GenCaller<void (T::*)(A0, A1)> {
		template <void (T::*fn)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct GenCaller<R(T::*)(A0, A1)> {
		template <R(T::*fn)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
			}
		}
	};

	template <typename T, typename A0, typename A1>
	struct GenCaller<void (T::*)(A0, A1) const> {
		template <void (T::*fn)(A0, A1) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct GenCaller<R(T::*)(A0, A1) const> {
		template <R(T::*fn)(A0, A1) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct Caller<R(T::*)(A0, A1, A2)> {
		template <R(T::*fn)(A0, A1, A2)>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1, A2 a2) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1, a2);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct Caller<R(T::*)(A0, A1, A2) const> {
		template <R(T::*fn)(A0, A1, A2) const>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1, A2 a2) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1, a2);
			}
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct GenCaller<void (T::*)(A0, A1, A2)> {
		template <void (T::*fn)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct GenCaller<R(T::*)(A0, A1, A2)> {
		template <R(T::*fn)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
			}
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct GenCaller<void (T::*)(A0, A1, A2) const> {
		template <void (T::*fn)(A0, A1, A2) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct GenCaller<R(T::*)(A0, A1, A2) const> {
		template <R(T::*fn)(A0, A1, A2) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct Caller<R(T::*)(A0, A1, A2, A3)> {
		template <R(T::*fn)(A0, A1, A2, A3)>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1, A2 a2, A3 a3) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1, a2, a3);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct Caller<R(T::*)(A0, A1, A2, A3) const> {
		template <R(T::*fn)(A0, A1, A2, A3) const>
		static R f(std::shared_ptr<T> & ptr, A0 a0, A1 a1, A2 a2, A3 a3) {
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
				return Default<R>::f();
			}
			else {
				return ((ptr.get())->*fn)(a0, a1, a2, a3);
			}
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct GenCaller<void (T::*)(A0, A1, A2, A3)> {
		template <void (T::*fn)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
					static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct GenCaller<R(T::*)(A0, A1, A2, A3)> {
		template <R(T::*fn)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
					static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
			}
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct GenCaller<void (T::*)(A0, A1, A2, A3) const> {
		template <void (T::*fn)(A0, A1, A2, A3) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
					static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value);
			}
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct GenCaller<R(T::*)(A0, A1, A2, A3) const> {
		template <R(T::*fn)(A0, A1, A2, A3) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			std::shared_ptr<T> & ptr = *static_cast<std::shared_ptr<T> *>(gen->GetObject());
			if (!ptr) {
				AS_NAMESPACE_QUALIFIER asIScriptContext * ctx = AS_NAMESPACE_QUALIFIER asGetActiveContext();
				ctx->SetException("Attempting to call member function of a null pointer");
			}
			else {
				new (gen->GetAddressOfReturnLocation()) Proxy<R>(((ptr.get())->*fn)(
					static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
					static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
					static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
					static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
			}
		}
	};

	template <typename T, typename U>
	struct GetSet {
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr   g(void) { return asFUNCTION((&getter    <T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr  rg(void) { return asFUNCTION((&ref_getter    <T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr  gg(void) { return asFUNCTION((&getter_gen<T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr rgg(void) { return asFUNCTION((&ref_getter_gen<T, U, member_ptr>)); }

		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr   s(void) { return asFUNCTION((&setter        <T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr  rs(void) { return asFUNCTION((&ref_setter    <T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr  sg(void) { return asFUNCTION((&setter_gen<T, U, member_ptr>)); }
		template <U T::* member_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr rsg(void) { return asFUNCTION((&ref_setter_gen<T, U, member_ptr>)); }
	};

	template <typename T, typename U>
	GetSet<T, U> gs_id(U T::*) { return GetSet<T, U>(); }

#define GETTER(ClassName, member_name)         (spw::gs_id(&ClassName::member_name).g<&ClassName::member_name>())
#define REF_GETTER(ClassName, member_name)     (spw::gs_id(&ClassName::member_name).rg<&ClassName::member_name>())
#define GETTER_GEN(ClassName, member_name)     (spw::gs_id(&ClassName::member_name).gg<&ClassName::member_name>())
#define REF_GETTER_GEN(ClassName, member_name) (spw::gs_id(&ClassName::member_name).rgg<&ClassName::member_name>())

#define     SETTER(ClassName, member_name)     (spw::gs_id(&ClassName::member_name).s<&ClassName::member_name>())
#define REF_SETTER(ClassName, member_name)     (spw::gs_id(&ClassName::member_name).rs<&ClassName::member_name>())
#define     SETTER_GEN(ClassName, member_name) (spw::gs_id(&ClassName::member_name).sg<&ClassName::member_name>())
#define REF_SETTER_GEN(ClassName, member_name) (spw::gs_id(&ClassName::member_name).rsg<&ClassName::member_name>())

	template <typename T>
	struct CCaller {
		template <T ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr c(void) { return asFUNCTION(&Caller<T>::template f<ptr>); }
		template <T ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr g(void) { return asFUNCTION(&GenCaller<T>::template f<ptr>); }
	};

	template <typename T>
	CCaller<T> caller_id(T) { return CCaller<T>(); }

#define     CALLER(ClassName, member_name)                  (spw::caller_id(&ClassName::member_name).c< &ClassName::member_name >())
#define GEN_CALLER(ClassName, member_name)                  (spw::caller_id(&ClassName::member_name).g< &ClassName::member_name >())
#define     CALLER_PR(ClassName, member_name, Args, Return) asFUNCTION((spw::Caller<Return (ClassName::*)Args>::f< &ClassName::member_name >))
#define GEN_CALLER_PR(ClassName, member_name, Args, Return) asFUNCTION((spw::GenCaller<Return (ClassName::*)Args>::f< &ClassName::member_name >))
} // end namespace spw


namespace gw {
	template <typename T> class Proxy {
	public:
		T value;
		Proxy(T value) : value(value) {}
		static T cast(void * ptr) {
			return reinterpret_cast<Proxy<T> *>(&ptr)->value;
		}
	private:
		Proxy(const Proxy &);
		Proxy & operator=(const Proxy &);
	};

	template <typename T> struct Wrapper {};
	template <typename T> struct ObjFirst {};
	template <typename T> struct ObjLast {};

	template <>
	struct Wrapper<void(*)(void)> {
		template <void(*fp)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)());
		}
	};

	template <typename R>
	struct Wrapper<R(*)(void)> {
		template <R(*fp)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)());
		}
	};

	template <typename T>
	struct Wrapper<void (T::*)(void)> {
		template <void (T::*fp)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)());
		}
	};

	template <typename T, typename R>
	struct Wrapper<R(T::*)(void)> {
		template <R(T::*fp)(void)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)());
		}
	};

	template <typename T>
	struct Wrapper<void (T::*)(void) const> {
		template <void (T::*fp)(void) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)());
		}
	};

	template <typename T, typename R>
	struct Wrapper<R(T::*)(void) const> {
		template <R(T::*fp)(void) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)());
		}
	};

	template <typename T>
	struct ObjFirst<void(*)(T)> {
		template <void(*fp)(T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R>
	struct ObjFirst<R(*)(T)> {
		template <R(*fp)(T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T>
	struct ObjLast<void(*)(T)> {
		template <void(*fp)(T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R>
	struct ObjLast<R(*)(T)> {
		template <R(*fp)(T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename A0>
	struct Wrapper<void(*)(A0)> {
		template <void(*fp)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename R, typename A0>
	struct Wrapper<R(*)(A0)> {
		template <R(*fp)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename A0>
	struct Wrapper<void (T::*)(A0)> {
		template <void (T::*fp)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename R, typename A0>
	struct Wrapper<R(T::*)(A0)> {
		template <R(T::*fp)(A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename A0>
	struct Wrapper<void (T::*)(A0) const> {
		template <void (T::*fp)(A0) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename R, typename A0>
	struct Wrapper<R(T::*)(A0) const> {
		template <R(T::*fp)(A0) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename A0>
	struct ObjFirst<void(*)(T, A0)> {
		template <void(*fp)(T, A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename R, typename A0>
	struct ObjFirst<R(*)(T, A0)> {
		template <R(*fp)(T, A0)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value));
		}
	};

	template <typename T, typename A0>
	struct ObjLast<void(*)(A0, T)> {
		template <void(*fp)(A0, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R, typename A0>
	struct ObjLast<R(*)(A0, T)> {
		template <R(*fp)(A0, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename A0, typename A1>
	struct Wrapper<void(*)(A0, A1)> {
		template <void(*fp)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename R, typename A0, typename A1>
	struct Wrapper<R(*)(A0, A1)> {
		template <R(*fp)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename A0, typename A1>
	struct Wrapper<void (T::*)(A0, A1)> {
		template <void (T::*fp)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct Wrapper<R(T::*)(A0, A1)> {
		template <R(T::*fp)(A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename A0, typename A1>
	struct Wrapper<void (T::*)(A0, A1) const> {
		template <void (T::*fp)(A0, A1) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct Wrapper<R(T::*)(A0, A1) const> {
		template <R(T::*fp)(A0, A1) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename A0, typename A1>
	struct ObjFirst<void(*)(T, A0, A1)> {
		template <void(*fp)(T, A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct ObjFirst<R(*)(T, A0, A1)> {
		template <R(*fp)(T, A0, A1)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value));
		}
	};

	template <typename T, typename A0, typename A1>
	struct ObjLast<void(*)(A0, A1, T)> {
		template <void(*fp)(A0, A1, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R, typename A0, typename A1>
	struct ObjLast<R(*)(A0, A1, T)> {
		template <R(*fp)(A0, A1, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename A0, typename A1, typename A2>
	struct Wrapper<void(*)(A0, A1, A2)> {
		template <void(*fp)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename R, typename A0, typename A1, typename A2>
	struct Wrapper<R(*)(A0, A1, A2)> {
		template <R(*fp)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct Wrapper<void (T::*)(A0, A1, A2)> {
		template <void (T::*fp)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct Wrapper<R(T::*)(A0, A1, A2)> {
		template <R(T::*fp)(A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct Wrapper<void (T::*)(A0, A1, A2) const> {
		template <void (T::*fp)(A0, A1, A2) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct Wrapper<R(T::*)(A0, A1, A2) const> {
		template <R(T::*fp)(A0, A1, A2) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct ObjFirst<void(*)(T, A0, A1, A2)> {
		template <void(*fp)(T, A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct ObjFirst<R(*)(T, A0, A1, A2)> {
		template <R(*fp)(T, A0, A1, A2)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2>
	struct ObjLast<void(*)(A0, A1, A2, T)> {
		template <void(*fp)(A0, A1, A2, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2>
	struct ObjLast<R(*)(A0, A1, A2, T)> {
		template <R(*fp)(A0, A1, A2, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<void(*)(A0, A1, A2, A3)> {
		template <void(*fp)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename R, typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<R(*)(A0, A1, A2, A3)> {
		template <R(*fp)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<void (T::*)(A0, A1, A2, A3)> {
		template <void (T::*fp)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<R(T::*)(A0, A1, A2, A3)> {
		template <R(T::*fp)(A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<void (T::*)(A0, A1, A2, A3) const> {
		template <void (T::*fp)(A0, A1, A2, A3) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct Wrapper<R(T::*)(A0, A1, A2, A3) const> {
		template <R(T::*fp)(A0, A1, A2, A3) const>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((static_cast<T *>(gen->GetObject())->*fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct ObjFirst<void(*)(T, A0, A1, A2, A3)> {
		template <void(*fp)(T, A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct ObjFirst<R(*)(T, A0, A1, A2, A3)> {
		template <R(*fp)(T, A0, A1, A2, A3)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				Proxy<T>::cast(gen->GetObject()),
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value));
		}
	};

	template <typename T, typename A0, typename A1, typename A2, typename A3>
	struct ObjLast<void(*)(A0, A1, A2, A3, T)> {
		template <void(*fp)(A0, A1, A2, A3, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T, typename R, typename A0, typename A1, typename A2, typename A3>
	struct ObjLast<R(*)(A0, A1, A2, A3, T)> {
		template <R(*fp)(A0, A1, A2, A3, T)>
		static void f(AS_NAMESPACE_QUALIFIER asIScriptGeneric * gen) {
			new (gen->GetAddressOfReturnLocation()) Proxy<R>((fp)(
				static_cast<Proxy <A0> *>(gen->GetAddressOfArg(0))->value,
				static_cast<Proxy <A1> *>(gen->GetAddressOfArg(1))->value,
				static_cast<Proxy <A2> *>(gen->GetAddressOfArg(2))->value,
				static_cast<Proxy <A3> *>(gen->GetAddressOfArg(3))->value,
				Proxy<T>::cast(gen->GetObject())));
		}
	};

	template <typename T>
	struct Id {
		template <T fn_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr  f(void) { return asFUNCTION(&Wrapper<T>::template f<fn_ptr>); }
		template <T fn_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr of(void) { return asFUNCTION(&ObjFirst<T>::template f<fn_ptr>); }
		template <T fn_ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr ol(void) { return asFUNCTION(&ObjLast<T>::template f<fn_ptr>); }
	};

	template <typename T>
	Id<T> id(T fn_ptr) { return Id<T>(); }

#define WRAP_FN(name)             (gw::id(name).f< name >())
#define WRAP_MFN(ClassType, name) (gw::id(&ClassType::name).f< &ClassType::name >())
#define WRAP_OBJ_FIRST(name)      (gw::id(name).of< name >())
#define WRAP_OBJ_LAST(name)       (gw::id(name).ol< name >())

#define WRAP_FN_PR(name, Parameters, ReturnType)             asFUNCTION((gw::Wrapper<ReturnType (*)Parameters>::f< name >))
#define WRAP_MFN_PR(ClassType, name, Parameters, ReturnType) asFUNCTION((gw::Wrapper<ReturnType (ClassType::*)Parameters>::f< &ClassType::name >))
#define WRAP_OBJ_FIRST_PR(name, Parameters, ReturnType)      asFUNCTION((gw::ObjFirst<ReturnType (*)Parameters>::f< name >))
#define WRAP_OBJ_LAST_PR(name, Parameters, ReturnType)       asFUNCTION((gw::ObjLast<ReturnType (*)Parameters>::f< name >))
} // end namespace gw

class ScriptBinder
{
public:
	virtual void setup(asIScriptEngine* engine) noexcept = 0;

	virtual void onFrameBegin() noexcept;
	virtual void onFrame() noexcept;
	virtual void onFrameEnd() noexcept;
};

_NAME_END

#endif
