export module dx3d:game;
import std;
import :core;
import :win32;
import :window;
import :graphics;

export namespace dx3d
{
	struct GameDesc
	{
		Logger::LogLevel LogLevel = Logger::LogLevel::Info;
	};
	class Game : public Base
	{
	public:
		virtual ~Game() override
		{
			GetLogger().Info("Game deallocation started.");
		}

		Game(const GameDesc& desc)
			: Base({ 
				.Logger = *(new Logger{desc.LogLevel})
			})
			, loggerPtr{ &logger }
		{
			loggerPtr->Info("Game initialised.");
			window = std::make_unique<Window>(
				WindowDesc{
					.Base = { .Logger = *loggerPtr }
				}
			);
			graphicsEngine = std::make_unique<GraphicsEngine>(
				GraphicsEngineDesc{
					.Base = { .Logger = *loggerPtr }
				}
			);
			loggerPtr->Info("Game initialisation completed.");
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
		std::unique_ptr<Logger> loggerPtr;
		std::unique_ptr<GraphicsEngine> graphicsEngine;
		std::unique_ptr<Window> window;
		bool isRunning = true;
	};
}
