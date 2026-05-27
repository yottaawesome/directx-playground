export module dx3d:core;

export namespace dx3d
{
	constexpr auto IsDebug =
#ifdef _DEBUG
		true;
#else
		false;
#endif
	constexpr auto IsRelease = not IsDebug;

	class Base
	{
	public:
		virtual ~Base() = default;
		Base() = default;
	protected:
		Base(const Base&) = delete;
		auto operator=(const Base&) -> Base& = delete;
	};
}