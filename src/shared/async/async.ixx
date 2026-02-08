export module shared:async;
import :win32;
import :error;
import :raii;
import :concepts;

export namespace Async
{
	auto CreateRawEvent(bool manualReset, bool initialState) -> Win32::HANDLE
	{
		if (auto handle = Win32::CreateEventW(nullptr, manualReset, initialState, nullptr); handle)
			return handle;
		throw Error::Win32Error{ Win32::GetLastError(), "Failed to create event" };
	}

	auto CreateEvent(bool manualReset, bool initialState) -> Raii::HandleUniquePtr
	{
		return Raii::HandleUniquePtr{ CreateRawEvent(manualReset, initialState) };
	}

	class Event
	{
	public:
		Event(Win32::HANDLE handle = CreateRawEvent(true, false)) 
			: Handle(handle)
		{ }

		auto Reset(this Event& self) -> void
		{
			Win32::ResetEvent(self.Handle.get());
		}

		auto Set(this Event& self) -> void
		{
			Win32::SetEvent(self.Handle.get());
		}

		auto Wait(this Event& self) -> bool
		{
			return self.Wait(std::chrono::milliseconds{ Win32::Infinite });
		}

		auto Wait(
			this Event& self, 
			Concepts::Duration auto&& timeout
		) -> bool
		{
			auto result = Win32::WaitForSingleObject(
				self.Handle.get(),
				static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count())
			);
			if (result == Win32::WaitResult::Timeout)
				return false;
			if (result == Win32::WaitResult::Signaled)
				return true;
			throw Error::Win32Error{ Win32::GetLastError(), "Failed to wait for event" };
		}

		auto GetPtr(this auto&& self) -> decltype(auto)
		{
			return std::forward_like<decltype(self)>(
				self.Handle
			);
		}

		auto GetHandle(this const Event& self) -> Win32::HANDLE
		{
			return self.Handle.get();
		}

	protected:
		Raii::HandleUniquePtr Handle;
	};
}
