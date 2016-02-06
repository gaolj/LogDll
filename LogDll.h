// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LOGDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LOGDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef LOGDLL_EXPORTS
#define LOGDLL_API __declspec(dllexport)
#else
#define LOGDLL_API __declspec(dllimport)
#endif

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(remote_address, "RemoteAddress", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(received_size, "ReceivedSize", std::size_t)
BOOST_LOG_ATTRIBUTE_KEYWORD(sent_size, "SentSize", std::size_t)

enum severity_level
{
	trace,
	debug,
	info,
	warn,
	error,
	fatal
};

class FuncTracer
{
public:
	src::severity_logger<severity_level> _logger;
	FuncTracer()
	{
		BOOST_LOG_SEV(_logger, trace) << "BGN";
	}

	~FuncTracer(void)
	{
		BOOST_LOG_SEV(_logger, trace) << "END";
	}
};

class LOGDLL_API Connection
{
	src::severity_channel_logger<severity_level> m_net, m_stat;
	logging::attribute_set::iterator m_net_remote_addr, m_stat_remote_addr;

public:
	Connection();

	void on_connected(std::string const& remote_addr);

	void on_disconnected();

	void on_data_received(std::size_t size);

	void on_data_sent(std::size_t size);
};

// 此类是从 LogDll.dll 导出的
class LOGDLL_API CLogDll {
public:
	CLogDll(void);

	void on_connected(std::string const& remote_addr);

	void on_disconnected();

	void on_data_received(std::size_t size);

	void on_data_sent(std::size_t size);
	src::channel_logger< > m_net{ keywords::channel = "net" }, m_stat{ keywords::channel = "stat" };
	logging::attribute_set::iterator m_net_remote_addr, m_stat_remote_addr;
};

extern LOGDLL_API int nLogDll;

LOGDLL_API int fnLogDll(void);
