/*****************************************************************************
 * @author: Qian Deyu
 * @data: 22-5-12 下午3:06
 * @version: 1.0
 * @description:
 ******************************************************************************/

#include <yaml-cpp/yaml.h>

#include "config.h"
#include "Log.h"

namespace sylar
{
    Config::ConfigVarMap Config::s_datas;

    ConfigVarBase::ptr Config::LookupBase(const std::string &name)
    {
        auto it = s_datas.find(name);
        return it == s_datas.end() ? nullptr : it->second;
    }

    /** 需要　"A.B", 10
     * Yaml 中为 A:
     *            B: 10
     *            C: str
     * 需要另外一个函数辅助
     */
    /**
     * 把YAML的层级结构展平，把Yaml::Node 的结构转换成为链表类型
     * @param prefix 前面的Ａ
     * @param node　Ｂ
     * @param output key-map存储于链表中
     */
    static void ListAllMember(const std::string &prefix,
                              const YAML::Node &node,
                              std::list<std::pair<std::string, const YAML::Node>> &output)
    {
        // std::string::npos 表示一个不存在的值，如果prefix第一个值不满足要求，出错
        if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
        {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        }
        output.push_back(std::make_pair(prefix, node));
        if (node.IsMap())
        {
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node &root)
    {
        std::list<std::pair<std::string, const YAML::Node>> all_nodes;
        ListAllMember("", root, all_nodes);
        for (auto &i : all_nodes)
        {
            std::string key = i.first;
            if (key.empty())
            {
                continue;
            }
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            ConfigVarBase::ptr var = LookupBase(key);

            if (var)
            {
                if (i.second.IsScalar())
                {
                    var->fromString(i.second.Scalar());
                }
                else
                {
                    std::stringstream ss;
                    ss << i.second;
                    var->fromString(ss.str());
                }
            }
        }
    }
}
