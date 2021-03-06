﻿/*
 * 定义规则管理器类
 * 规则包括：源文件扩展名，对应的分析器，以及相应的语法规则。
 * 提供对配置规则的加载和判定方法。
 */

#pragma once

namespace sc
{

    class RuleManager
    {
    private:

        // 文件扩展名对应的语言
        std::map<string_type, string_type, _str_compare> m_ExtMap;

        // 语言对应的语法配置项
        std::map<string_type, std::tuple<string_type, syntax_t>, _str_compare> m_SyntaxMap;

        // 默认的分析器名称
        static const string_type _DefaultAnalyzer;

    public:

        RuleManager();

        // 从文件加载配置
        bool Load(const string_type& filename, string_type& error);

        // 获取支持的语言列表
        list_t GetLanguages() const;

        // 根据文件扩展名获取对应的语言
        string_type GetLanguage(const string_type& ext) const;

        // 获取语言对应的扩展名
        list_t GetExtensions(const string_type& name) const;

        // 根据语言名称获取对应的配置规则
        const auto& GetRule(const string_type& name) const { return m_SyntaxMap.at(name); }

        // 根据语言名称获取语法配置项
        const syntax_t& GetSyntax(const string_type& name) const { return std::get<1>(GetRule(name)); }

        // 判定是否支持的语言
        bool Contains(const string_type& name) const { return (m_SyntaxMap.find(name) != m_SyntaxMap.end()); }

        // 内置支持的分析器配置
        static const std::map<string_type, std::tuple<list_t, syntax_t>, _str_compare>& BuildInRules();

        // 判定是否支持的分析器
        static bool IsSupport(const string_type& name);

    };  // class RuleManager

}   // namespace sc
