export module dx3d:com.ptr;
import std;
import :win32;
import :com.hresult;
import :com.error;

export namespace dx3d
{
	template<typename T, bool Copyable = true>
	struct Ptr
	{
		constexpr ~Ptr() noexcept
		{
			reset();
		}

		constexpr Ptr() = default;

		constexpr explicit Ptr(T* typePtr) noexcept
			: ptr(typePtr)
		{}

		// Optionally copyable
		constexpr Ptr(const Ptr& other) noexcept
			requires Copyable
		: ptr(other.ptr)
		{
			if (ptr)
				ptr->AddRef();
		}
		constexpr Ptr(const Ptr& other) noexcept
			requires (not Copyable) = delete;
		constexpr auto operator=(const Ptr& other) noexcept -> Ptr&
			requires Copyable
		{
			if (this == &other)
				return *this;
			reset();
			ptr = other.ptr;
			if (ptr)
				ptr->AddRef();
			return *this;
		}
		constexpr auto operator=(const Ptr& other) noexcept -> Ptr&
			requires (not Copyable) = delete;


		// Movable
		constexpr Ptr(Ptr&& other) noexcept
			: ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		constexpr auto operator=(Ptr&& other) noexcept -> Ptr&
		{
			if (this == &other)
				return *this;
			reset();
			ptr = std::exchange(other.ptr, nullptr);
			return *this;
		}

		constexpr explicit operator bool(this const Ptr& self) noexcept
		{
			return self.ptr != nullptr;
		}

		constexpr auto operator==(this const Ptr& self, const Ptr& other) noexcept -> bool
		{
			return self.ptr == other.ptr;
		}

		constexpr auto operator==(this const Ptr& self, std::nullptr_t) noexcept -> bool
		{
			return self.ptr == nullptr;
		}

		constexpr auto operator*(this auto&& self) noexcept -> T&
		{
			return *self.ptr;
		}

		constexpr auto operator->(this auto&& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto reset(this Ptr& self) noexcept -> void
		{
			if (self.ptr)
			{
				self.ptr->Release();
				self.ptr = nullptr;
			}
		}

		constexpr auto detach(this Ptr& self) noexcept -> T*
		{
			T* temp = self.ptr;
			self.ptr = nullptr;
			return temp;
		}

		constexpr auto Get(this const Ptr& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto swap(this Ptr& self, Ptr& other) noexcept -> void
		{
			auto temp = self.ptr;
			self.ptr = other.ptr;
			other.ptr = temp;
		}

		// Releases the held interface and returns the address of the internal
		// pointer for use as an out-parameter (the typical COM idiom).
		constexpr auto ReleaseAndGetAddressOf(this Ptr& self) noexcept -> T**
		{
			self.reset();
			return &self.ptr;
		}

		constexpr auto Release() -> std::uint32_t
		{
			if (not ptr)
				return 0;
			auto count = ptr->Release();
			ptr = nullptr;
			return count;
		}

		constexpr auto AddRef(this Ptr& self) -> std::uint32_t
		{
			if (not self.ptr)
				return 0;
			return self.ptr->AddRef();
		}

		// Convenience aliases for ReleaseAndGetAddressOf, kept because the
		// existing call sites use them. Both release first to avoid leaks.
		constexpr auto AddressOf(this Ptr& self) noexcept -> void**
		{
			self.reset();
			return reinterpret_cast<void**>(&self.ptr);
		}

		constexpr auto AddressOfTyped(this Ptr& self) noexcept -> T**
		{
			return self.ReleaseAndGetAddressOf();
		}

		constexpr auto GetUuid(this const Ptr&) noexcept -> Win32::GUID
		{
			return __uuidof(T);
		}

		template <typename U>
		auto As(this const Ptr& self) -> Ptr<U>
		{
			if (not self.ptr)
				return {};

			auto rawPtr = static_cast<U*>(nullptr);
			auto hr = HResult{
				self.ptr->QueryInterface(__uuidof(U), reinterpret_cast<void**>(&rawPtr))
			};
			if (not hr)
				throw ComError{ hr, "QueryInterface() failed in As<>()" };
			return Ptr<U>{ rawPtr };
		}

		T* ptr = nullptr;
	};
}