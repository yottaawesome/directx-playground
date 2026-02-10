export module shared:async;
import :win32;
import :error;
import :raii;
import :util;
import :concepts;

export namespace Async
{
	struct EventOptions
	{
		std::wstring_view Name;
		bool InitialState = false;
	};

	auto CreateEventHandle(bool manualReset, const EventOptions& options) -> Win32::HANDLE
	{
		auto handle = Win32::HANDLE{
			Win32::CreateEventW(
				nullptr,
				manualReset,
				options.InitialState,
				options.Name.empty() ? nullptr : options.Name.data()
			) };
		if (handle)
			return handle;
		throw Error::Win32Error{ Win32::GetLastError(), "Failed to create event" };
	}

	// Can define these as a template type with statics, but 
	// MSVC ICEs if using static functions.
	template<typename TPointer>
	class Event
	{
	public:
		constexpr Event(TPointer::element_type* handle)
			: Handle(handle)
		{ }

		auto CreateEvent(this auto& self, const EventOptions& options) -> TPointer
		{
			return Raii::HandleUniquePtr{ self.CreateRawEvent(options) };
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

		auto Wait(this Event& self, Concepts::Duration auto&& timeout) -> bool
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

		constexpr auto GetHandle(this const auto& self) -> auto
		{
			return self.Handle.get();
		}
	protected:
		TPointer Handle;
	};

	struct AutoResetEvent : Event<Raii::HandleUniquePtr>
	{
		constexpr AutoResetEvent()
			: Event(CreateEventHandle(false, { .InitialState = false }))
		{ }
	};

	struct ManualResetEvent : Event<Raii::HandleUniquePtr>
	{
		constexpr ManualResetEvent()
			: Event(CreateEventHandle(true, { .InitialState = false }))
		{ }

		void Reset(this ManualResetEvent& self)
		{
			Win32::ResetEvent(self.Handle.get());
		}
	};
}

namespace
{
	using PtrType = std::unique_ptr<int>;

	struct TestManualResetEvent : Async::Event<std::unique_ptr<int>>
	{
		constexpr TestManualResetEvent()
			: Event(new int{1})
		{ }

		consteval auto IsManualReset(this auto&) noexcept -> bool
		{
			return true;
		}

		constexpr void Reset(this auto& self) { }
	};

	struct TestAutoResetEvent : Async::Event<std::unique_ptr<int>>
	{
		constexpr TestAutoResetEvent()
			: Event(new int{ 1 })
		{ }

		consteval auto IsManualReset(this auto&) noexcept -> bool
		{
			return false;
		}
	};

	constexpr auto Tests = Util::Overloaded{
		[] {
			auto manual = TestManualResetEvent{};
			manual.Reset();
			if (*manual.GetHandle() != 1)
				throw std::exception{ "Handle value was not 1" };
		},
		[] {
			auto manual = TestManualResetEvent{};
			auto ptr = std::move(manual).GetPtr();
			if (*ptr != 1)
				throw std::exception{ "Ptr value was not 1" };
		}
	};
}
