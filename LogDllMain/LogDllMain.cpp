// LogDllMain.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../LogDll.h"

#include <boost/thread/thread.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/support/date_time.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/exception/all.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

//enum severity_level
//{
//	trace,
//	debug,
//	info,
//	warn,
//	error,
//	fatal
//};

BOOST_LOG_ATTRIBUTE_KEYWORD(_severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(_timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(_uptime, "Uptime", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(_scope, "Scope", attrs::named_scope::value_type)

logging::formatting_ostream& operator<<
(
	logging::formatting_ostream& strm,
	logging::to_log_manip< severity_level, tag::_severity > const& manip
)
{
	static const char* strings[] =
	{
		"TRACE",
		"DEBUG",
		"INFO ",
		"WARN ",
		"ERROR",
		"FATAL"
	};

	severity_level level = manip.get();
	if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
		strm << strings[level];
	else
		strm << static_cast< int >(level);

	return strm;
}
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt<severity_level>)

void bar(int x);

int main()
{
	logging::add_console_log(std::clog, keywords::format = "%TimeStamp%	%Message%");
	logging::add_file_log(
		keywords::file_name = "out1.log",
		//keywords::open_mode = std::ios::app,
		keywords::auto_flush = true,
		keywords::rotation_size = 1 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::filter = _severity == trace,
		keywords::format = expr::stream
							<< expr::format_date_time(_timestamp, "%Y-%m-%d %H:%M:%S.%f")
							<< "	" << expr::attr< attrs::current_thread_id::value_type >("ThreadID")
							<< "	" << _severity
							<< "	" << expr::message
							<< "			" << expr::format_named_scope(_scope, keywords::format = "%c", keywords::iteration = expr::reverse, keywords::depth = 3));
	logging::add_file_log(
		keywords::file_name = "out2.log",
		//keywords::open_mode = std::ios::app,
		keywords::auto_flush = true,
		keywords::rotation_size = 1 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::filter = _severity.or_default(debug) >= debug,
		keywords::format = expr::stream
							<< expr::format_date_time(_timestamp, "%Y-%m-%d %H:%M:%S.%f")
							<< "	" << expr::attr< attrs::current_thread_id::value_type >("ThreadID")
							<< "	" << _severity
							<< "		" << expr::message
							//<< "		" << expr::format_named_scope(_scope, keywords::format = "[%C	%F:%l]", keywords::iteration = expr::reverse, keywords::depth = 3));
							<< "			" << expr::format_named_scope(_scope, keywords::format = "%c", keywords::iteration = expr::reverse, keywords::depth = 3));

	logging::add_common_attributes();
	logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());

	{	// no level 输出，因为level不是severity_level类型
		src::logger lg;
		BOOST_LOG(lg) << "BOOST_LOG";

		BOOST_LOG_TRIVIAL(info) << "BOOST_LOG_TRIVIAL";
		BOOST_LOG_TRIVIAL(severity_level::info) << "BOOST_LOG_TRIVIAL";
	}
	fnLogDll();

	BOOST_LOG_FUNCTION();
	auto& slg = my_logger::get();
	BOOST_LOG_SEV(slg, trace) << "An trace severity message";
	BOOST_LOG_SEV(slg, debug) << "An debug severity message";
	BOOST_LOG_SEV(slg, info) << "An info severity message";
	BOOST_LOG_SEV(slg, warn) << "An warn severity message";
	BOOST_LOG_SEV(slg, error) << "An error severity message";
	BOOST_LOG_SEV(slg, fatal) << "An fatal severity message";


	Connection conn;
	conn.on_connected("1.2.3.4");
	conn.on_data_received(123);
	conn.on_data_sent(321);
	conn.on_disconnected();

	CLogDll conn2;
	conn2.on_connected("5.6.7.8");
	conn2.on_data_received(123);
	conn2.on_data_sent(321);
	conn2.on_disconnected();

	try
	{
		bar(-1);
	}
	catch (std::range_error& e)
	{
		auto scope = boost::get_error_info< logging::current_scope_info >(e);

		src::logger lg;
		BOOST_LOG(lg) << "bar call failed: " << e.what() << ", scopes stack:"
			<< *boost::get_error_info< logging::current_scope_info >(e);
		// bar call failed: x must not be negative, scopes stack:int __cdecl main(void)->void __cdecl bar(int)
	}
	return 0;
}

void bar(int x)
{
	BOOST_LOG_FUNCTION();
	if (x < 0)
	{
		// Attach a copy of the current scope stack to the exception
		throw boost::enable_error_info(std::range_error("x must not be negative"))
			<< logging::current_scope();
	}
}
