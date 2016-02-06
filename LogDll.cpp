// LogDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LogDll.h"
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt<severity_level >)
#define LOG_FUNC_TRACER()	BOOST_LOG_FUNCTION();FuncTracer tracer;


// 这是导出变量的一个示例
LOGDLL_API int nLogDll=0;

// 这是导出函数的一个示例。
LOGDLL_API int fnLogDll(void)
{
	LOG_FUNC_TRACER();

	auto& log = my_logger::get();
	BOOST_LOG_SEV(log, info) << "BOOST_LOG_SEV";

	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 LogDll.h
CLogDll::CLogDll()
{
    return;
}

void CLogDll::on_connected(std::string const& remote_addr)
{
	LOG_FUNC_TRACER();
	attrs::constant< std::string > addr(remote_addr);
	m_net_remote_addr = m_net.add_attribute("RemoteAddress", addr).first;
	m_stat_remote_addr = m_stat.add_attribute("RemoteAddress", addr).first;

	BOOST_LOG(m_net) << "Connection on ";
}

void CLogDll::on_disconnected()
{
	LOG_FUNC_TRACER();
	BOOST_LOG(m_net) << "Connection off";

	m_net.remove_attribute(m_net_remote_addr);
	m_stat.remove_attribute(m_stat_remote_addr);
}

void CLogDll::on_data_received(std::size_t size)
{
	LOG_FUNC_TRACER();
	BOOST_LOG(m_stat) << logging::add_value("ReceivedSize", size) << "Some data recv";
}

void CLogDll::on_data_sent(std::size_t size)
{
	LOG_FUNC_TRACER();
	BOOST_LOG(m_stat) << logging::add_value("SentSize", size) << "Some data sent";
}


Connection::Connection() :
	m_net(keywords::channel = "net"),
	m_stat(keywords::channel = "stat")
{
}

void Connection::on_connected(std::string const& remote_addr)
{
	LOG_FUNC_TRACER();
	attrs::constant< std::string > addr(remote_addr);
	m_net_remote_addr = m_net.add_attribute("RemoteAddress", addr).first;
	m_stat_remote_addr = m_stat.add_attribute("RemoteAddress", addr).first;

	BOOST_LOG_SEV(m_net, info) << "Connection on ";
}

void Connection::on_disconnected()
{
	LOG_FUNC_TRACER();
	BOOST_LOG_SEV(m_net, error) << "Connection off";

	m_net.remove_attribute(m_net_remote_addr);
	m_stat.remove_attribute(m_stat_remote_addr);
}

void Connection::on_data_received(std::size_t size)
{
	LOG_FUNC_TRACER();
	BOOST_LOG_SEV(m_stat, debug) << logging::add_value("ReceivedSize", size) << "Some data recv";
}

void Connection::on_data_sent(std::size_t size)
{
	LOG_FUNC_TRACER();
	BOOST_LOG_SEV(m_stat, warn) << logging::add_value("SentSize", size) << "Some data sent";
}
