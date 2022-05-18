//
// Created by dy on 2022/5/4.
//

#ifndef SYLARWEBSERVER_LOG_H
#define SYLARWEBSERVER_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>
#include <map>
#include <stdarg.h>
#include "singleton.h"
#include "util.h"

/**
 * @brief ʹ�����ķ�ʽ����־���� level ����־д�뵽 logger
 * ʹ�ú궨��ķ�ʽ
 */
#define SYLAR_LOG_LEVEL(logger, level)                                                              \
        if(logger->getLevel() <= level)                                                             \
            sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,                    \
                                level, __FILE__, __LINE__, 0, sylar::GetThreadId(),                 \
                                sylar::GetFiberId(), time(0)))).getSS()

/**
 * @brief ʹ����ʽ��־�ķ�ʽ����־���� debug ����־д�뵽 logger
 */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEUG)

/**
 * @brief ʹ����ʽ��ʽ����־���� info ����־д�뵽 logger
 */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

/**
 * @brief ʹ����ʽ��ʽ����־����warn������־д�뵽 logger
 */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

/**
 * @brief ʹ����ʽ�ķ�ʽ����־���� error ����־д�뵽 logger
 */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

/**
 * @brief ʹ����ʽ�ķ�ʽ����־���� Fatal ����־д�뵽 logger
 */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� level ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                                         \
        if(logger->getLevel() <= level)                                                      \
            sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level,      \
                                __FILE__, __LINE__, 0, sylar::GetThreadId(),                 \
                                sylar::GetFiberId(), time(0)))                               \
                                ).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� debug ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) \
        SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� info ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) \
        SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� warn ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) \
        SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� ERROR ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) \
        SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief ʹ�ø�ʽ����ʽ����־���� FATAL ����־д�뵽 logger
 */
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) \
        SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief ��ȡ����־��
 */
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief ��ȡ name ����־��
 */
#define SYLAR_LOG_NAME() sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {

    class Logger;

    class LoggerManager;

    // ��־����
    class LogLevel {
    public:
        enum Level {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        static const char *ToString(LogLevel::Level level);
    };

    // ��־�¼�
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                 const char *file, int32_t line, uint32_t elapse,
                 uint32_t thread_id, uint32_t fiber_id, uint64_t time);

        const char *getFile() const { return m_file; }

        int32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        uint32_t getFiberId() const { return m_fiberId; }

        uint64_t getTime() const { return m_time; }

        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        std::string getContent() const { return m_ss.str(); }

        const std::string &getThreadName() const { return m_threadName; }


        LogLevel::Level getLevel() const { return m_level; }

        /**
     * @brief ������־�����ַ�����
     */
        std::stringstream &getSS() { return m_ss; }

        /**
         * @brief ��ʽ��д����־����
         */
        void format(const char *fmt, ...);

        /**
         * @brief ��ʽ��д����־����
         */
        void format(const char *fmt, va_list al);

    private:
        const char *m_file = nullptr;   // �ļ���
        int32_t m_line = 0;             // �к�
        uint32_t m_elapse = 0;          // ����������ʼ�����ڵĺ�����
        uint32_t m_threadId = 0;        // �߳�id
        uint32_t m_fiberId = 0;         // Э��id
        uint64_t m_time;                // ʱ���

        std::string m_threadName;       // �߳�����
        std::shared_ptr<Logger> m_logger;   //����־��
        LogLevel::Level m_level;            // ��־�ȼ�

        std::stringstream m_ss;             // ��־������
    };

    class LogEventWrap {
    public:
        /**
         * @brief ���캯��
         * @param[in] e ��־�¼�
         */
        LogEventWrap(LogEvent::ptr e);

        /**
         * @brief ��������
         */
        ~LogEventWrap();

        /**
         * @brief ��ȡ��־�¼�
         */
        LogEvent::ptr getEvent() const { return m_event; }

        /**
         * @brief ��ȡ��ʽ������
         */
        std::stringstream &getSS();

    private:
        LogEvent::ptr m_event;
    };

    // ��־��ʽ��
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(const std::string &pattern);

        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        std::ostream &
        format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);


    public:
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            virtual ~FormatItem() {}

            /**
             * @brief ��ʽ����־����
             * @param[in, out] os ��־�����
             * @param[in] logger ��־��
             * @param[in] level ��־�ȼ�
             * @param[in] event ��־�¼�
             */
            virtual void
            format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        void init();

    private:
        // ��־��ʽģ��
        std::string m_pattern;
        // ��־pattern������ĸ�ʽ
        std::vector<FormatItem::ptr> m_items;
        // �Ƿ��д���
        bool m_error = false;
    };


    // ��־�����
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;

        LogAppender() : m_level(LogLevel::DEBUG), m_formatter(nullptr) {}

        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val) { m_formatter = val; }

        LogFormatter::ptr getFormatter() const { return m_formatter; }

    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };

    // ��־��
    class Logger : public std::enable_shared_from_this<Logger> {
        friend class LoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string &name = "root");

        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);

        void info(LogEvent::ptr event);

        void warn(LogEvent::ptr event);

        void error(LogEvent::ptr event);

        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);

        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const { return m_level; }

        void setLevel(LogLevel::Level val) { m_level = val; }

        /**
         * @brief ������־����
         */
        const std::string &getName() const { return m_name; }

    private:
        std::string m_name;                              // ��־����
        LogLevel::Level m_level;                    //��־����
        std::list<LogAppender::ptr> m_appeders;     //Appender����
        LogFormatter::ptr m_formatter;  // ��־��ʽ��
        Logger::ptr m_root; // ����־��
    };

    // ���������̨��appender
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;

        void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    // ����������ļ���Appender
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;

        FileLogAppender(const std::string &filename);

        // ���´��ļ����ļ��򿪳ɹ�����true
        bool reopen();

        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };

    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string & = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadName();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
                : m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };


    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string &str)
                : m_string(str) {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }

    private:
        std::string m_string;
    };


    class LoggerManager {
    public:
        LoggerManager();

        /**
         * @brief ��ȡ��־��
         * @param[in] name ��־������
         */
        Logger::ptr getLogger(const std::string &name);

        /**
         * ��brief ��ʼ��
         */
        void init();

        /**
         * @brief ��������־��
         */
        Logger::ptr getRoot() const { return m_root; }

        /**
         * @brief �����е���־������ת�ɡ�YAML string
         */
        std::string toYamlstring();

    private:
        // ��־������
        std::map<std::string, Logger::ptr> m_loggers;
        // ����־��
        Logger::ptr m_root;
    };

    typedef sylar::Singleton<LoggerManager> LoggerMgr;
}

#endif //SYLARWEBSERVER_LOG_H
