export module dx3d:core.logger;
import std;

export namespace dx3d
{
	class Logger
	{
	public:
		enum class LogLevel
		{
			Debug,
			Info,
			Warning,
			Error,
			Critical
		};

		explicit Logger(LogLevel level = LogLevel::Error)
			: minimumLogLevel(level)
		{}

		void Debug(std::string_view message)
		{
			if (LogLevel::Debug < minimumLogLevel)
				return;
			std::clog << std::format("[DX3D {}] {}\n", "DEBUG", message);
		}

		void Info(std::string_view message)
		{
			if (LogLevel::Info < minimumLogLevel)
				return;
			std::clog << std::format("[DX3D {}] {}\n", "INFO", message);
		}

		void Warn(std::string_view message)
		{
			if (LogLevel::Warning < minimumLogLevel)
				return;
			std::clog << std::format("[DX3D {}] {}\n", "WARNING", message);
		}

		void Error(std::string_view message)
		{
			if (LogLevel::Error < minimumLogLevel)
				return;
			std::clog << std::format("[DX3D {}] {}\n", "ERROR", message);
		}

		void Critical(std::string_view message)
		{
			if (LogLevel::Critical < minimumLogLevel)
				return;
			std::clog << std::format("[DX3D {}] {}\n", "CRITICAL", message);
		}
	private:
		LogLevel minimumLogLevel;
	};
}