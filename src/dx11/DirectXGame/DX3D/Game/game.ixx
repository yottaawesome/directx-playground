export module dx3d:game;
import std;
import :core;
import :win32;
import :window;

export namespace dx3d
{
	class Game : public Base
	{
	public:
		virtual ~Game() override = default;

		Game()
		{
			window = std::make_unique<Window>();
		}

		virtual void Run() final
		{
			auto msg = Win32::MSG{};
			while (isRunning)
			{
				while (Win32::PeekMessageW(&msg, nullptr, 0, 0, Win32::PeekMessageFlags::Remove))
				{
					if (msg.message == Win32::Messages::Quit)
					{
						isRunning = false;
						break;
					}

					Win32::TranslateMessage(&msg);
					Win32::DispatchMessageW(&msg);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	private:
		std::unique_ptr<Window> window;
		bool isRunning = true;
	};
}
