export module shared:raii;
import std;
import :win32;

export namespace Raii
{
	template<auto VDeleteFn>
	struct Deleter
	{
		static constexpr void operator()(auto ptr) noexcept
		{
			VDeleteFn(ptr);
		}
	};
	template<typename T, auto VDeleteFn>
	using IndirectUniquePtr = std::unique_ptr<std::remove_pointer_t<T>, Deleter<VDeleteFn>>;
	template<typename T, auto VDeleteFn>
	using DirectUniquePtr = std::unique_ptr<T, Deleter<VDeleteFn>>;

	using HandleUniquePtr = IndirectUniquePtr<Win32::HANDLE, Win32::CloseHandle>;
}
