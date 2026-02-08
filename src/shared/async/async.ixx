export module shared:async;
import :win32;
import :error;
import :raii;
import :concepts;

export namespace Async
{
	struct EventOptions
	{
		std::wstring_view Name;
		bool ManualReset = false;
		bool InitialState = false;
	};

	auto CreateRawEvent(const EventOptions& options) -> Win32::HANDLE
	{
		auto handle = Win32::HANDLE{
			Win32::CreateEventW(
				nullptr,
				options.ManualReset,
				options.InitialState,
				options.Name.empty() ? nullptr : options.Name.data()
			)};
		if (handle)
			return handle;
		throw Error::Win32Error{ Win32::GetLastError(), "Failed to create event" };
	}

	auto CreateEvent(const EventOptions& options) -> Raii::HandleUniquePtr
	{
		return Raii::HandleUniquePtr{ CreateRawEvent(options) };
	}

	template<bool VManualReset>
	class Event
	{
	public:
		constexpr Event(Win32::HANDLE handle = CreateRawEvent({ .ManualReset = VManualReset, .InitialState = false }))
			: Handle(handle)
		{ }

		void Reset(this Event& self) requires VManualReset
		{
			Win32::ResetEvent(self.Handle.get());
		}

		void Set(this Event& self)
		{
			Win32::SetEvent(self.Handle.get());
		}

		auto Wait(this Event& self) -> bool
		{
			return self.Wait(std::chrono::milliseconds{ Win32::Infinite });
		}

		void WaitForSuccess(this Event& self, Concepts::Duration auto&& timeout)
		{
			if (not self.Wait(timeout))
				throw Error::RuntimeError{std::format("Failed to wait for event in under {}", timeout)};
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

		constexpr auto GetPtr(this auto&& self) -> decltype(auto)
		{
			return std::forward_like<decltype(self)>(self.Handle);
		}

		constexpr auto GetHandle(this const Event& self) -> Win32::HANDLE
		{
			return self.Handle.get();
		}

	protected:
		Raii::HandleUniquePtr Handle;
	};

	using AutoResetEvent = Event<false>;
	using ManualResetEvent = Event<true>;
}
