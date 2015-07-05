#ifndef AS_SMART_POINTER_WRAPPER
#define AS_SMART_POINTER_WRAPPER

#include <new>
#include "angelscript.h"
#include <memory>

namespace spw {
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

#define GETTER(ClassName, member_name)         (::spw::gs_id(&ClassName::member_name).g<&ClassName::member_name>())
#define REF_GETTER(ClassName, member_name)     (::spw::gs_id(&ClassName::member_name).rg<&ClassName::member_name>())
#define GETTER_GEN(ClassName, member_name)     (::spw::gs_id(&ClassName::member_name).gg<&ClassName::member_name>())
#define REF_GETTER_GEN(ClassName, member_name) (::spw::gs_id(&ClassName::member_name).rgg<&ClassName::member_name>())

#define     SETTER(ClassName, member_name)     (::spw::gs_id(&ClassName::member_name).s<&ClassName::member_name>())
#define REF_SETTER(ClassName, member_name)     (::spw::gs_id(&ClassName::member_name).rs<&ClassName::member_name>())
#define     SETTER_GEN(ClassName, member_name) (::spw::gs_id(&ClassName::member_name).sg<&ClassName::member_name>())
#define REF_SETTER_GEN(ClassName, member_name) (::spw::gs_id(&ClassName::member_name).rsg<&ClassName::member_name>())

	template <typename T>
	struct CCaller {
		template <T ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr c(void) { return asFUNCTION(&Caller<T>::template f<ptr>); }
		template <T ptr> AS_NAMESPACE_QUALIFIER asSFuncPtr g(void) { return asFUNCTION(&GenCaller<T>::template f<ptr>); }
	};

	template <typename T>
	CCaller<T> caller_id(T) { return CCaller<T>(); }

#define     CALLER(ClassName, member_name)                  (::spw::caller_id(&ClassName::member_name).c< &ClassName::member_name >())
#define GEN_CALLER(ClassName, member_name)                  (::spw::caller_id(&ClassName::member_name).g< &ClassName::member_name >())
#define     CALLER_PR(ClassName, member_name, Args, Return) asFUNCTION((::spw::Caller<Return (ClassName::*)Args>::f< &ClassName::member_name >))
#define GEN_CALLER_PR(ClassName, member_name, Args, Return) asFUNCTION((::spw::GenCaller<Return (ClassName::*)Args>::f< &ClassName::member_name >))
} // end namespace spw

#endif
