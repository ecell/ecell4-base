#include <string>
#include <map>
#include <utility>
#include <memory>
#include <cstdio>
#include <functional>
// #include <boost/regex.hpp> //XXX: disabled pattern matching once
#include <boost/foreach.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/bind.hpp>
#include "Logger.hpp"
#include "ConsoleAppender.hpp"
#include "utils/pair.hpp"
#include "utils/fun_composition.hpp"

namespace ecell4
{
namespace egfrd
{

class LoggerManagerRegistry
{
private:
    typedef std::pair<std::string, boost::shared_ptr<LoggerManager> > entry_type;
    // typedef std::pair<boost::regex, boost::shared_ptr<LoggerManager> > entry_type;
public:
    void register_logger_manager(char const* logger_name_pattern,
                                 boost::shared_ptr<LoggerManager> const& manager)
    {
        managers_.push_back(entry_type(entry_type::first_type(logger_name_pattern), manager));
    }

    boost::shared_ptr<LoggerManager>
    get_default_logger_manager() const
    {
        return default_manager_;
    }

    boost::shared_ptr<LoggerManager>
    operator()(char const* logger_name) const
    {
        if (!logger_name)
            return default_manager_;


        // char const* const logger_name_end(logger_name + std::strlen(logger_name));
        // BOOST_FOREACH (entry_type const& i, managers_)
        // {
        //     if (boost::regex_match(logger_name, logger_name_end, i.first))
        //         return i.second;
        // }
        const std::string _logger_name(logger_name);
        BOOST_FOREACH (entry_type const& i, managers_)
        {
            if (_logger_name == i.first)
                return i.second;
        }

        BOOST_ASSERT(default_manager_.get());
        return default_manager_;
    }

    LoggerManagerRegistry(): default_manager_(new LoggerManager("default"))
    {
        default_manager_->add_appender(boost::shared_ptr<LogAppender>(new ConsoleAppender()));
    }

private:
    std::vector<entry_type> managers_;
    boost::shared_ptr<LoggerManager> default_manager_;
};

static LoggerManagerRegistry registry;
    
void LoggerManager::register_logger_manager(
        char const* logger_name_pattern,
        boost::shared_ptr<LoggerManager> const& manager)
{
    registry.register_logger_manager(logger_name_pattern, manager);
}

boost::shared_ptr<LoggerManager> LoggerManager::get_logger_manager(char const* logger_name_pattern)
{
    return registry(logger_name_pattern);
}

boost::shared_ptr<LoggerManager> Logger::manager() const
{
    const_cast<Logger*>(this)->ensure_initialized();
    return manager_;
}

Logger& Logger::get_logger(char const* name)
{
    static std::map<std::string, std::unique_ptr<Logger>> loggers;

    auto inserted(loggers.emplace(std::string(name), nullptr));
    if (inserted.second)
    {
        std::unique_ptr<Logger> log(new Logger(registry, name));
        inserted.first->second = std::move(log);
    }
    return *(inserted.first->second);
}

char const* Logger::stringize_error_level(enum level lv)
{
    static char const* names[] = {
        "OFF",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };
    return static_cast<std::size_t>(lv) >= sizeof(names) / sizeof(*names) ? "???": names[lv];
}

Logger::~Logger()
{
}

struct invoke_appender
{
    void operator()(boost::shared_ptr<LogAppender> const& appender) const
    {
        const char* chunks[] = { formatted_msg, NULL };
        (*appender)(level, name, chunks);
    }

    invoke_appender(enum Logger::level level,
                    const char* name, char const *formatted_msg)
        : level(level), name(name),
          formatted_msg(formatted_msg) {}

    enum Logger::level const level;
    char const* const name;
    char const* const formatted_msg;
};

void Logger::level(enum Logger::level level)
{
    ensure_initialized();
    level_ = level;
}

enum Logger::level Logger::level() const
{
    const_cast<Logger*>(this)->ensure_initialized();
    return level_;
}

void Logger::logv(enum level lv, char const* format, va_list ap)
{
    ensure_initialized();

    if (lv < level_)
        return;

    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, ap);

    std::for_each(appenders_.begin(), appenders_.end(),
            invoke_appender(lv, name_.c_str(),
                            buf));
}

void Logger::flush()
{
    ensure_initialized();

    std::for_each(appenders_.begin(), appenders_.end(),
            boost::bind(&LogAppender::flush, _1));
}

inline void Logger::ensure_initialized()
{
    if (!manager_)
    {
        boost::shared_ptr<LoggerManager> manager(registry_(name_.c_str()));
        std::vector<boost::shared_ptr<LogAppender> > appenders(manager->appenders());
        level_ = manager->level();
        appenders_.swap(appenders);
        manager->manage(this);
        manager_ = manager;
    }
}

Logger::Logger(LoggerManagerRegistry const& registry, char const* name)
        : registry_(registry), name_(name), manager_() {}

void LoggerManager::level(enum Logger::level level)
{
    /* synchronized { */
    level_ = level;
    std::for_each(managed_loggers_.begin(), managed_loggers_.end(),
                  boost::bind(&Logger::level, _1, level));
    /* } */
}

enum Logger::level LoggerManager::level() const
{
    return level_;
}

char const* LoggerManager::name() const
{
    return name_.c_str();
}

std::vector<boost::shared_ptr<LogAppender> > const& LoggerManager::appenders() const
{
    /* synchronized() { */
    return appenders_;
    /* } */
}

void LoggerManager::add_appender(boost::shared_ptr<LogAppender> const& appender)
{
    /* synchronized() { */
    appenders_.push_back(appender);
    /* } */
}

LoggerManager::LoggerManager(char const* name, enum Logger::level level)
    : name_(name), level_(level) {}

void LoggerManager::manage(Logger* logger)
{
    /* synchronized { */
    managed_loggers_.insert(logger);
    /* }} */
}
LogAppender::~LogAppender() {}
} // egfrd
} // ecell4
