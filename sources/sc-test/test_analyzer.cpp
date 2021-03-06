﻿/*
 * 测试分析器算法
 * 利用默认配置分别对 resources/sources/ 目录下的示例源文件按照多种统计模式进行统计
 * 比较统计结果的正确性
 */

#include "../third/xf_simple_test.h"

#include "../sc-counter/ReportType.h"
#include "../sc-counter/RuleManager.h"
#include "../sc-counter/analyzer/Analyzer.h"

namespace sc::test
{
    inline const auto& GetRule(const std::string& name) { static sc::RuleManager _mgr; return _mgr.GetRule(name); }

    _xfTest(test_c)
    {
        std::string file("../../resources/sources/demo.c");
        const auto& [type, syntax] = GetRule("C");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;

        _xfExpect(sc::report_t(14, 7, 4, 3) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(14, 7, 6, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(14, 7, 7, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));

        file.assign("../../resources/sources/demo.h");

        _xfExpect(sc::report_t(24, 16, 4, 4) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(24, 16, 9, 4) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(24, 16, 9, 4) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));

        file.assign("../../resources/sources/empty.h");

        _xfExpect(sc::report_t(0, 0, 0, 0) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(0, 0, 0, 0) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(0, 0, 0, 0) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_cpp)
    {
        const std::string file("../../resources/sources/demo.cpp");
        const auto& [type, syntax] = GetRule("C++");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(sc::report_t(18, 15, 1, 2) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(18, 14, 1, 3) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(sc::report_t(18, 15, 2, 2) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(18, 15, 2, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_ruby)
    {
        const std::string file("../../resources/sources/demo.rb");
        const auto& [type, syntax] = GetRule("Ruby");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(sc::report_t(40, 30, 4, 6) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(40, 28, 4, 8) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(sc::report_t(40, 30, 5, 6) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(40, 30, 7, 8) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_python)
    {
        const std::string file("../../resources/sources/demo.py");
        const auto& [type, syntax] = GetRule("Python");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;
        auto m3 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_comment | sc::mode_t::ms_is_blank;
        auto m4 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_comment | sc::mode_t::ms_is_code;

        _xfExpect(sc::report_t(47, 24, 14, 9) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(47, 21, 14, 12) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(sc::report_t(47, 21, 17, 9) == sc::Analyzer::Analyze(file, type, syntax, m3));
        _xfExpect(sc::report_t(47, 24, 17, 6) == sc::Analyzer::Analyze(file, type, syntax, m4));
        _xfExpect(sc::report_t(47, 24, 15, 9) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(47, 24, 18, 12) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_c_sharp)
    {
        const std::string file("../../resources/sources/demo.cs");
        const auto& [type, syntax] = GetRule("C#");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(sc::report_t(21, 18, 1, 2) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(sc::report_t(21, 17, 1, 3) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(sc::report_t(21, 18, 4, 2) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(sc::report_t(21, 18, 4, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

}
