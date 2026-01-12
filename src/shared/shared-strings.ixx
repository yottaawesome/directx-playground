export module shared:strings;
import std;

export namespace Strings
{
	template<size_t N, typename TChar>	
	struct FixedString
	{
		TChar Buffer[N]{};
		using TStringView = std::basic_string_view<TChar, std::char_traits<TChar>>;
		using TString = std::basic_string<TChar, std::char_traits<TChar>>;

		constexpr FixedString() noexcept = default;

		constexpr FixedString(const TChar (&str)[N]) noexcept
		{
			std::copy_n(str, N, Buffer);
		}
		
		constexpr auto ToView() const noexcept -> TStringView
		{
			return TStringView{ Buffer };
		}

		constexpr auto ToString() const noexcept -> TString
		{
			return TString{ Buffer };
		}

		constexpr auto Size() const noexcept -> size_t
		{
			return N - 1; // exclude null terminator
		}

		template<size_t M>
		constexpr auto operator+(const FixedString<M, TChar>& other) const noexcept -> FixedString<N + M - 1, TChar>
		{
			FixedString<N + M - 1, TChar> result{};
			std::copy_n(Buffer, N - 1, result.Buffer);
			std::copy_n(other.Buffer, M, result.Buffer + N - 1); // include null terminator from other
			return result;
		}

		constexpr auto operator==(const FixedString<N, TChar>& other) const noexcept -> bool
		{
			for (size_t i = 0; i < Size(); ++i)
			{
				if (Buffer[i] != other.Buffer[i])
					return false;
			}
			return true;
		}

		template<size_t M>
		constexpr auto operator==(const FixedString<M, TChar>& other) const noexcept -> bool
		{
			return false;
		}
	};

	template<size_t N>
	FixedString(const char(&str)[N]) -> FixedString<N, char>;
	template<size_t N>
	FixedString(const wchar_t(&str)[N]) -> FixedString<N, wchar_t>;

	static_assert(FixedString{"Hello, "} + FixedString{"world!"} == "Hello, world!");
}
