#pragma once

namespace spd {
	template <typename T>
	struct remove_reference {
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&> {
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&&> {
		using type = T;
	};

	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template <typename T>
	struct remove_const {
		using type = T;
	};

	template <typename T>
	struct remove_const<const T> {
		using type = T;
	};

	template <typename T>
	using remove_const_t = typename remove_const<T>::type;

	struct false_type {
		static constexpr bool value = false;
	};

	struct true_type {
		static constexpr bool value = true;
	};

	template <typename T>
	struct is_lvalue_ref : false_type {};

	template <typename T>
	struct is_lvalue_ref<T&> : true_type {};

	template <typename T>
	inline constexpr bool is_lvalue_ref_v = is_lvalue_ref<T>::value;

	template <typename T>
	struct is_rvalue_ref : false_type {};

	template <typename T>
	struct is_rvalue_ref<T&&> : true_type {};

	template <typename T>
	inline constexpr bool is_rvalue_ref_v = is_rvalue_ref<T>::value;

	template<bool B, typename T, typename F>
	struct conditional {
		using type = T;
	};

	// partial specialization for false case
	template<typename T, typename F>
	struct conditional<false, T, F> {
		using type = F;
	};

	template<bool B, typename T, typename F>
	using conditional_t = typename conditional<B, T, F>::type;

	// ---------- functions below ----------

	template <typename T>
	inline constexpr remove_reference_t<T>&& move(T&& elem) noexcept {
		return static_cast<remove_reference_t<T>&&>(elem);
	}

	template <typename T>
	inline constexpr remove_reference_t<T>&& move_if_noexcept(T&& elem) noexcept {
		return static_cast<remove_reference_t<T>&&>(elem);
	}

	template <typename T>
	inline constexpr T&& forward(remove_reference_t<T>& arg) noexcept {
		return static_cast<T&&>(arg);
	}

	// for rvalues, ensure rvalues aren't accidentally forwarded as lvalues
	template <typename T>
	constexpr T&& forward(remove_reference_t<T>&& arg) noexcept {
		static_assert(!is_lvalue_ref_v<T>,
					  "template argument substituting T is an lvalue reference type, "
					  "but forward received an rvalue");
		return static_cast<T&&>(arg);
	}
}