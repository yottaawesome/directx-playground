export module shared:util;
import std;

export namespace Util
{
	struct InitT {} constexpr Init;
	struct NoInitT {} constexpr NoInit;

	template<typename...T>
	struct Overloaded : T...
	{
		constexpr Overloaded(T... t)
			: T(std::move(t))...
		{
			Run();
		}

		using T::operator()...;

		constexpr void Run(this Overloaded& self)
		{
			static_assert(((static_cast<T&>(self)(), ...), true));
		}
	};
}
