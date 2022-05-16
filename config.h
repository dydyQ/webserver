/*****************************************************************************
* @author: Qian Deyu
* @data: 22-5-12 下午3:06
* @version: 1.0
* @description:　一些配置模块，后续再详细说明
*******************************************************************************/

#ifndef SYLARWEBSERVER_CONFIG_H
#define SYLARWEBSERVER_CONFIG_H

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "Log.h"
#include "util.h"

namespace sylar {

// 把一些公用的属性都放到这里面
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVarBase(const std::string &name, const std::string &description = "")
                : m_name(name), m_description(description) {}

        virtual ~ConfigVarBase() {}

        const std::string &getName() const { return m_name; }

        const std::string &getDescription() const { return m_description; }

        /**
         * @brief 转成字符串
         */
        virtual std::string toString() = 0;

        /**
         * @brief 从字符串初始化
         */
        virtual bool fromString(const std::string &val) = 0;

    protected:
        std::string m_name;
        std::string m_description;
    };

    template<class T>
    class ConfigVar : public ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;

        ConfigVar(const std::string &name, const T &default_value, const std::string &description = "")
                : ConfigVarBase(name, description), m_val(default_value) {}

        std::string toString() override {
            try {
                return boost::lexical_cast<std::string>(m_val);
            } catch (std::exception &e) {   // 防止和别的库一起使用的时候出现问题
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                                                  << e.what() << "convert: " << typeid(m_val).name() << " to string";
            }
            return "";
        }

        bool fromString(const std::string &val) override {
            try {
                m_val = boost::lexical_cast<T>(val);
            } catch (std::exception &e) {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                                                  << e.what() << "convert string to " << typeid(m_val).name();
            }
            return false;
        }

        const T getValue() const { return m_val; }

        void setValue(const T &v) { m_val = v; }

    private:
        T m_val;
    };


    class Config {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

        template<class T>
        static typename ConfigVar<T>::ptr
        Lookup(const std::string &name, const T &default_value, const std::string &description = "") {
            auto tmp = Lookup<T>(name);
            if (tmp) {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name = " << name << " exists";
                return tmp;
            }

            // 判断名称是否合法
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789")
                != std::string::npos) {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
                throw std::invalid_argument(name);
            }

            // 此处的typename  需要好好想一想
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
            s_datas[name] = v;

            return v;
        }

        template<class T>
        static typename ConfigVar<T>::ptr
        Lookup(const std::string &name) {
            auto it = s_datas.find(name);
            if (it == s_datas.end()) {
                return nullptr;
            }

            return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
        }

    private:
        static ConfigVarMap s_datas;
    };

}
#endif //SYLARWEBSERVER_CONFIG_H
