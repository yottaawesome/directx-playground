export module dx3d:core.base;
import std;
import :core.common;
import :core.logger;

export namespace dx3d
{
	constexpr auto IsDebug =
#ifdef _DEBUG
		true;
#else
		false;
#endif
	constexpr auto IsRelease = not IsDebug;
	
	struct BaseDesc
	{
		Logger& Logger;
	};
	class Base
	{
	public:
		virtual ~Base() = default;
		Base(const Base&) = delete;
		auto operator=(const Base&) -> Base & = delete;

		explicit Base(const BaseDesc& desc) 
			: logger{desc.Logger} 
		{ }
		
		virtual auto GetLogger() noexcept -> Logger& final
		{ 
			return logger; 
		}

	protected:
		Logger& logger;
	};
}
