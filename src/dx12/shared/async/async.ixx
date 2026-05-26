export module shared:async;
import :win32;
import :error;
import :raii;
import :util;
import :concepts;
import :strings;

export namespace Async
{
	// Can define these as a template type with statics, but 
	// MSVC ICEs if using static functions.
	struct Event
	{
		void Set(this auto& self)
		{
			Win32::SetEvent(self.Handle.get());
		}

		auto Wait(this auto& self) -> bool
		{
			return self.Wait(std::chrono::milliseconds{ Win32::Infinite });
		}

		void WaitForSuccess(this auto& self, Concepts::Duration auto&& timeout)
		{
			if (not self.Wait(timeout))
				throw Error::RuntimeError{std::format("Failed to wait for event in under {}", timeout)};
		}

		auto Wait(this auto& self, Concepts::Duration auto&& timeout) -> bool
		{
			auto result = Win32::DWORD{
				Win32::WaitForSingleObject(
					self.GetHandle(),
					static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count())
				)};
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

		constexpr auto Empty(this const auto& self) -> bool 
		{ 
			return not self.Handle; 
		}
	};

	struct EventOpener
	{
		std::wstring_view Name = L"";
		Win32::DWORD DesiredAccess = Win32::EventAccess::All;
		bool InheritHandle = false;

		[[nodiscard]]
		auto Open(this const EventOpener& self) -> Raii::HandleUniquePtr
		{
			auto handle = Win32::HANDLE{
				Win32::OpenEventW(
					self.DesiredAccess,
					self.InheritHandle,
					self.Name.data()
				)};
			if (not handle)
				throw Error::Win32Error{ Win32::GetLastError(), std::format("Failed to open event with name '{}'", Strings::ConvertString(self.Name)) };
			return Raii::HandleUniquePtr{ handle };
		}
		
		[[nodiscard]]
		operator Raii::HandleUniquePtr(this const EventOpener& self)
		{
			return self.Open();
		}

		[[nodiscard]]
		auto operator()(this const EventOpener& self) -> Raii::HandleUniquePtr
		{
			return self.Open();
		}
	};

	struct EventBuilder
	{
		std::wstring Name = L"";
		bool ManualReset = false;
		bool InitialState = false;

		[[nodiscard]]
		auto Create(this const EventBuilder& self) -> Raii::HandleUniquePtr
		{
			auto handle = Win32::HANDLE{
				Win32::CreateEventW(
					nullptr,
					self.ManualReset,
					self.InitialState,
					self.Name.empty() ? nullptr : self.Name.data()
				)};
			if (not handle)
				throw Error::Win32Error{ Win32::GetLastError(), "Failed to create event" };
			return Raii::HandleUniquePtr{ handle };
		}

		[[nodiscard]]
		operator Raii::HandleUniquePtr(this const EventBuilder& self)
		{
			return self.Create();
		}

		[[nodiscard]]
		auto operator()(this const EventBuilder& self) -> Raii::HandleUniquePtr
		{
			return self.Create();
		}
	};

	struct AutoResetEvent : Event
	{
		Raii::HandleUniquePtr Handle = EventBuilder{};
	};

	struct ManualResetEvent : Event
	{
		void Reset(this ManualResetEvent& self)
		{
			Win32::ResetEvent(self.Handle.get());
		}

		Raii::HandleUniquePtr Handle = EventBuilder{ .ManualReset = true };
	};
}

namespace
{
	using PtrType = std::unique_ptr<int>;

	struct TestManualResetEvent : Async::Event
	{
		constexpr TestManualResetEvent()
			: Handle(new int{1})
		{ }

		constexpr void Reset(this auto& self) { }
		PtrType Handle;
	};

	struct TestAutoResetEvent : Async::Event
	{
		constexpr TestAutoResetEvent()
			: Handle(new int{ 1 })
		{ }

		PtrType Handle;
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
			if (not manual.Empty())
				throw std::exception{ "Expected object to be empty after move" };
		}
	};
}
