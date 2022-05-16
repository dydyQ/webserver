//
// Created by dy on 2022/5/5.
//
#include <functional>
#include <cassert>
#include <yaml-cpp/yaml.h>

#include "Log.h"
namespace sylar {

    LogEventWrap::LogEventWrap(LogEvent::ptr e) : m_event(e) {}

    LogEventWrap::~LogEventWrap() {
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    std::stringstream &LogEventWrap::getSS() {
        return m_event->getSS();
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t line,
                       uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time)
            : m_file(file), m_line(line), m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id), m_time(time),
              m_logger(logger), m_level(level) {}

    void LogEvent::format(const char *fmt, ...) {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void LogEvent::format(const char *fmt, va_list al) {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if(len != -1) {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    Logger::Logger(const std::string &name) : m_name(name), m_level(LogLevel::DEBUG) {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            for (auto &i: m_appeders) {
                i->log(self, level, event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }

    void Logger::addAppender(LogAppender::ptr appender) {
        if(!appender->getFormatter()) {
            appender->setFormatter(m_formatter);
        }
        m_appeders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender) {
        for (auto it = m_appeders.begin(); it != m_appeders.end(); ++it) {
            if (*it == appender) {
                m_appeders.erase(it);
                break;
            }
        }
    }

    void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level)
            assert(m_formatter != nullptr);
            std::cout << m_formatter->format(logger, level, event);
    }

    void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename) {
    }

    bool FileLogAppender::reopen() {
        if (m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !!m_filestream;
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i: m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    std::ostream &LogFormatter::format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level,
                                       LogEvent::ptr event) {
        for (auto &i: m_items) {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

    // 支持的格式：%xxx, %xxx{xxx} 转义%%
    void LogFormatter::init() {
        //str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i) {
            if (m_pattern[i] != '%') {
                // 解析两个模式子串之间的连接字符部分，例如空格和中括号之类的
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            /**
             * fmt_status = 0表示普通格式控制连接符，如'[', ' ', ':'等
             * fmt_status = 1表示正在解析 %xx{yyy}类型的数据
             */
            int fmt_status = 0;
            // fmt_begin 表示fmt字符串的起始下标
            size_t fmt_begin = 0;

            // str 表示format 字符
            std::string str;
            // fmt表示格式化字符串，比如 %xxx 中的 xxx
            std::string fmt;

            /**
             * 如果m_pattern[i] == ‘%’, 且m_pattern[i+1] != '%',
             * 说明从 i+1 开始是一个新的pattern子串了，进行解析
             */
            while (n < m_pattern.size()) {
                /**
                 * fmt_status = 0 表示 此时为普通状态
                 * fmt_status = 1 表示读取pattern子串完成
                 * 此时正在读取pattern子串，并且 m_pattern[i] 不是字母也不是‘{’ ‘}’,
                 * 表示 m_pattern[i]开始已经不是模式子串了，使用str 保存此模式子串，退出解析模式子串的循环
                 */
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                                    && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()) {
                    if (str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, std::string(), 0));
        }
        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

                XX(m, MessageFormatItem),           //m:消息
                XX(p, LevelFormatItem),             //p:日志级别1
                XX(r, ElapseFormatItem),            //r:累计毫秒数
                XX(c, NameFormatItem),              //c:日志名称
                XX(t, ThreadIdFormatItem),          //t:线程id
                XX(n, NewLineFormatItem),           //n:换行
                XX(d, DateTimeFormatItem),          //d:时间
                XX(f, FilenameFormatItem),          //f:文件名
                XX(l, LineFormatItem),              //l:行号
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberIdFormatItem),           //F:协程id
                XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
        };

        for (auto &i: vec) {
            if (std::get<2>(i) == 0) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end()) {
                    m_items.push_back(
                            FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

//            std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
//        std::cout << m_items.size() << std::endl;
    }

    const char *LogLevel::ToString(LogLevel::Level level) {
        switch (level) {
#define xx(name) \
        case LogLevel::name: \
            return #name;    \
            break;
            xx(DEBUG);
            xx(INFO);
            xx(WARN);
            xx(ERROR);
            xx(FATAL);
#undef xx
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
    }

    LoggerManager::LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

        m_loggers[m_root->m_name] = m_root;

        init();
    }

    Logger::ptr LoggerManager::getLogger(const std::string &name) {
        auto it = m_loggers.find(name);
        if(it != m_loggers.end()) {
            return it->second;
        }

        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }

//    std::string LoggerManager::toYamlstring() {
//        YAML::Node node;
//    }

    void LoggerManager::init() {

    }
}
