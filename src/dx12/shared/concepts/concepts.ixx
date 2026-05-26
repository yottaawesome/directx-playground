export module shared:concepts;
import std;

export namespace Concepts
{
	// Doesn't work with the default parameter of std::chrono::milliseconds{ Win32::Infinite }, 
	// for some reason.
	//template<typename T>
	//concept Duration = std::is_convertible_v<T, std::chrono::duration<typename T::rep, typename T::period>>;

	template<class T>
	struct IsDuration : std::false_type {};

	template<class Rep, class Period>
	struct IsDuration<std::chrono::duration<Rep, Period>> : std::true_type {};

	template<typename T>
	constexpr bool IsDurationV = IsDuration<T>::value;

	template<typename T>
	concept Duration = IsDurationV<T>;
}
