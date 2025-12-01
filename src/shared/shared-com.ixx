export module shared:com;
import :win32;
import :error;
import std;

export namespace Com
{
	template<typename T>
	struct Ptr
	{
		static constexpr Win32::GUID Uuid = __uuidof(T);

		using pointer = T*;

		constexpr ~Ptr() noexcept
		{
			reset();
		}

		constexpr Ptr() = default;

		constexpr Ptr(T* typePtr) noexcept
			: ptr(typePtr)
		{
		}

		constexpr Ptr(const Ptr& typePtr) noexcept
			: ptr(typePtr.ptr)
		{
			if (ptr)
				ptr->AddRef();
		}
		constexpr auto operator=(this Ptr& self, const Ptr& other) noexcept -> Ptr&
		{
			self.reset();
			self.ptr = other.ptr;
			self.ptr->AddRef();
			return self;
		}

		constexpr Ptr(Ptr&& other) noexcept
			: ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		constexpr auto operator=(this Ptr& self, Ptr&& other) noexcept -> Ptr&
		{
			self.reset();
			self.swap(other);
			return self;
		}

		constexpr auto operator->(this auto&& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto reset(this Ptr& self) noexcept -> Ptr&
		{
			if (self.ptr)
			{
				self.ptr->Release();
				self.ptr = nullptr;
			}
			return self;
		}

		constexpr auto detach(this Ptr& self) noexcept -> T*
		{
			T* temp = self.ptr;
			self.ptr = nullptr;
			return temp;
		}

		constexpr auto get(this const Ptr& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto swap(this Ptr& self, Ptr& other) noexcept -> void
		{
			std::swap(self.ptr, other.ptr);
		}

		constexpr auto AddressOf(this Ptr& self) noexcept -> void**
		{
			return (void**)&self.ptr;
		}

		T* ptr = nullptr;
	};

	struct HResult final
	{
		constexpr HResult() noexcept = default;

		constexpr HResult(const Win32::HRESULT hr) noexcept
			: Hr(hr) {
		}

		constexpr HResult(long severity, long facility, long code) noexcept
		{
			Hr = Win32::MakeHResult(severity, facility, code);
		}

		constexpr operator Win32::HRESULT() const noexcept { return Hr; }

		constexpr HResult& operator=(const Win32::HRESULT hr) noexcept
		{
			Hr = hr;
			return *this;
		}

		constexpr operator bool() const noexcept { return Succeeded(); }

		constexpr bool operator==(const Win32::HRESULT hr) const noexcept { return Hr == hr; }

		constexpr bool operator==(const HResult& hr) const noexcept { return Hr == hr.Hr; }

		// See https://learn.microsoft.com/en-us/windows/win32/com/using-macros-for-error-handling
		constexpr HRESULT Get() const noexcept { return Hr; }

		constexpr long Facility() const noexcept { return Win32::Facility(Hr); }

		constexpr long Code() const noexcept { return Win32::Code(Hr); }

		constexpr long Severity() const noexcept { return Win32::Severity(Hr); }

		constexpr bool Succeeded() const noexcept { return Win32::HrSuccess(Hr); }

		constexpr bool Failed() const noexcept { return not Succeeded(); }

		void ThrowIfFailed(std::string_view msg, const std::source_location& loc = std::source_location::current()) const
		{
			if (Succeeded())
				return;
			if (not msg.empty())
				throw Error::ComError(Hr, msg.data(), loc);
			throw Error::ComError(Hr, "HRESULT check failed", loc);
		}

		Win32::HRESULT Hr = 0x0;
	};
}
