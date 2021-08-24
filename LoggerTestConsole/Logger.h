#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <mutex>
#include <ctime>
#include <fstream>
#include <tchar.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace aricanli {
	namespace general {
		class Timer
		{
		public:
			//initialize class with a test name and by starting timer
			Timer() : m_Stop(false)
			{
				m_BeginTime = std::chrono::high_resolution_clock::now();
			}

			//destructor if timer not stopped stop timer
			~Timer() { if (!m_Stop) Stop(); }

			//calculate beginning and finishing times of timer and also get thread id
			//by using these calculations adds new profile to json file and stop timer
			void Stop()
			{
				auto endTime = std::chrono::high_resolution_clock::now();
				begin = std::chrono::time_point_cast<std::chrono::microseconds>(m_BeginTime).time_since_epoch().count();
				end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

				threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
				m_Stop = true;
			}
		public:
			long long begin;
			long long end;
			uint32_t threadID;

		private:
			std::chrono::time_point<std::chrono::steady_clock> m_BeginTime;
			bool m_Stop;
		};

		enum class Severity
		{
			Quiet = 0, Fatal = 8, Error = 16, Warning = 24, Info = 32, Verbose = 40, Debug = 48, Trace = 56
		};

		class Logger
		{
		private:
			Severity severity = Severity::Quiet;
			std::mutex log_mutex;
			std::string file_path = "";
			std::ofstream file;

		protected:
			// template function converts arguments(wchar_t) to std::wstring type
			//input any type args but can't take char type because wostringstream for wchar_t
			template<typename ... Args>
			std::wstring wstringer(const Args& ... args)
			{
				std::wostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			// template function converts arguments(char_t, other args) to std::string type
			//input any type args but can't take wchar_t type because ostringstream for char_t
			template<typename ... Args>
			std::string stringer(const Args& ... args)
			{
				std::ostringstream oss;
				int a[] = { 0, ((void)(oss << args), 0) ... };
				return oss.str();
			}

			/*****************************************************************************************
			* Def : template function take any type input and convert it to std::string to write file
			* Args : value => specify any type input to write file
			* Output : writes given input to file
			******************************************************************************************/
			template <typename T>
			void log_writefile(const T& value) {
				file.close();
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				if (typeid(value) == typeid(wchar_t const* __ptr64)) {
					std::wstring temp = wstringer(value);
					std::string res(temp.begin(), temp.end());
					file << res.c_str() << " ";
					std::cout << res << " ";
					return;
				}
				file << value << " ";
				std::cout << value << " ";
			}

		public:
			//severity has default value but developer can set desired severity value manually
			static void SetPriority(Severity new_severity)
			{
				instance().severity = new_severity;
			}

			/***********************************************************************************************
			 * Def : Template function developer can write any severity type log in one function by changing
			 * not constant parameter Severity type and severity msg
			 * Args : line => lineNumber, source_file => source File path, msg_priorty_str => Severity as string
			 * msg_severity => severity level to check whether severity bigger or not,
			 * message => first user message, args => any type args (wchar,char, int ...)
			 * Output : Log text file which includes all logs in chosen path from GUI
			 ***********************************************************************************************/
			template<typename T, typename... Args>
			static void Any(int line, const std::string source_file, const std::string& msg_priorty_str,
				Severity msg_severity, const T message, Args... args)
			{
				instance().log(line, source_file, msg_priorty_str, msg_severity, message, args...);
			}

		private:
			Logger()
			{
				file_path = "C:\\Users\\hp\\source\\repos\\log.txt";
				enable_file_output(file_path);
			}

			Logger(const Logger&) = delete;
			Logger& operator= (const Logger&) = delete;

			~Logger()
			{
				if (file.is_open())
					file.close();
			}

			static Logger& instance()
			{
				static Logger logger;
				return logger;
			}

			/********************************************************************************************
			 * Def : Takes arguments which will be written to file and format them to write file properly
			 * like log. (Log format : Date, Severity type, Args, line, File)
			 * Args : line => lineNumber, source => source File path, msg_priorty_str => Severity as string
			 * msg_severity => severity to check whether severity bigger or not, msg => first user message
			 * args => any type args (wchar,char, int ...)
			 * Output : Log text file which includes all logs in chosen path from GUI
			 *********************************************************************************************/
			template<typename T, typename... Args>
			void log(int line, const std::string& source, const std::string& msg_priorty_str,
				Severity msg_severity, const T& msg, Args... args)
			{
				if (severity <= msg_severity)	//check severity
				{
					//Date operation take current time : Day, Month, Hour:Minute:Sec, Year
					time_t current_time = time(0);
					tm* timestamp = localtime(&current_time);
					char buffer[80];
					//convert time to char array then char array to std::string
					strftime(buffer, 80, "%c", timestamp);
					std::string s(buffer);
					//define std::lock_guard to support multithreading
					typename std::lock_guard<std::mutex> lock(log_mutex);
					std::string m_msg;
					if (file.is_open())
					{
						file.close();
						file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
						//check type wchar or char convert std::string to write file
						if (typeid(msg) == typeid(wchar_t const* __ptr64)) {
							std::wstring tmp = wstringer(msg);
							std::string resT(tmp.begin(), tmp.end());
							m_msg = resT;
						}
						else {
							std::string resT = stringer(msg);
							m_msg = resT;
						}
						//write file given inputs in a proper format for logging
						//(time, severity, message, args, line and source file path)
						file << s.c_str() << '\t' << msg_priorty_str.c_str() << " " << m_msg.c_str() << " ";
						std::cout << s << '\t' << msg_priorty_str << " " << m_msg;
						//write all type args to file
						int dummy[] = { 0, ((void)log_writefile(std::forward<Args>(args)),0)... };
						file << " on line " << line << " in " << source.c_str() << "\n";
						std::cout << " on line " << line << " in " << source << "\n";
					}
					else
						std::cout << "Logger: Failed to open file ";
				}
			}

			//close file if open if don't close, don't write file
			void close_file()
			{
				if (file.is_open())
					file.close();
			}

			//create or open file in a give path
			void enable_file_output(const std::string& new_file_path)
			{
				file.close();
				file_path = new_file_path;
				file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
				if (!file.is_open()) {
					std::cout << "File creation failed " << file_path;
					return;
				}
			}
		};

		/******************************************************************************************
		* Define macros to provide usefulness to developer Line and File default no need to write
		* again and again so file and line are given developers just write their messages by using
		* macros which type developer need according to severity
		*******************************************************************************************/
		#define LOG_QUIET(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Quiet]\t", Severity::Quiet, msg, __VA_ARGS__))
		#define LOG_FATAL(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Fatal]\t", Severity::Fatal, msg, __VA_ARGS__))
		#define LOG_ERROR(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Error]\t", Severity::Error, msg, __VA_ARGS__))
		#define LOG_INFO(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Info]\t", Severity::Info, msg, __VA_ARGS__))
		#define LOG_WARN(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Warn]\t", Severity::Warning, msg, __VA_ARGS__))
		#define LOG_DEBUG(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Debug]\t", Severity::Debug, msg, __VA_ARGS__))
		#define LOG_VERB(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Verbose]\t", Severity::Verbose, msg, __VA_ARGS__))
		#define LOG_TRACE(msg, ...) (Logger::Any(__LINE__, __FILE__,"[Trace]\t", Severity::Trace, msg, __VA_ARGS__))
	}
}
