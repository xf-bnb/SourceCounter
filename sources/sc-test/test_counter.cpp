﻿/*
 * 测试统计器
 * 测试统计器加载源文件路径的正确性以及对批量源文件统计结果的正确性
 */

#include <string>
#include <filesystem>

#include "../third/xf_simple_test.h"
#include "../sc-counter/Counter.h"

namespace sc::test
{
    // 汇总统计报告
    inline sc::report_t _TotalReport(const std::vector<sc::Counter::file_report_t>& reports)
    {
        unsigned int x1(0), x2(0), x3(0), x4(0);
        for (const auto& [_nemo1, _nemo2, report] : reports)
        {
            const auto& [y1, y2, y3, y4] = report;
            x1 += y1; x2 += y2; x3 += y3; x4 += y4;
        }

        return { x1, x2, x3, x4 };
    }

    _xfTest(test_counter_for_load)
    {
        sc::Counter counter;

        auto languages = counter.RuleMgr().GetLanguages();
        _xfExpect(8 == counter.LoadFile("../../resources/sources", "", languages, true));

        counter.Start(4, sc::mode_t::default_mode);

        _xfExpect(8 == counter.Reports().size());
    }

    _xfTest(test_counter_for_empty)
    {
        sc::Counter counter;

        auto languages = counter.RuleMgr().GetLanguages();
        _xfExpect(7 == counter.LoadFile("../../resources/sources", "", languages, false));
    }

    _xfTest(test_counter_language)
    {
        sc::Counter counter;

        _xfExpect(3 == counter.LoadFile("../../resources/sources", "", { "C", "Frank" }, true));
    }

    _xfTest(test_counter_for_exclude)
    {
        sc::Counter counter;

        auto languages = counter.RuleMgr().GetLanguages();
        _xfExpect(6 == counter.LoadFile("../../resources/sources", R"(\.c$)", languages, false));
    }

    class _test_soource_dir final
    {
        _test_soource_dir(const _test_soource_dir&) = delete;
        _test_soource_dir& operator=(const _test_soource_dir&) = delete;

    public:

        const std::filesystem::path _from;
        const std::filesystem::path _to;

        // 将目录 from 中的内容复制 n 份到目录 to 中
        _test_soource_dir(const std::string& from, const std::string& to, unsigned int n)
            : _from(from), _to(to)
        {
            if (std::filesystem::exists(_from))
            {
                if (!std::filesystem::exists(_to))
                    std::filesystem::create_directories(_to);

                if (std::filesystem::exists(_to))
                    for (unsigned int i = 0; i < n; ++i)
                        std::filesystem::copy(_from, (_to / _from.filename()).concat(std::to_string(i + 1)));
            }
        }

        // 删除 to 目录
        ~_test_soource_dir()
        {
            if (std::filesystem::exists(_to))
                std::filesystem::remove_all(_to);
        }
    };

    _xfTest(test_counter)
    {
        const std::string sources_dir("../../resources/sources");
        const std::string test_dir("../../outputs/test-sources");

        // 定义统计器1，加载资源文件
        sc::Counter counter1;
        auto languages = counter1.RuleMgr().GetLanguages();
        auto nSource = counter1.LoadFile(sources_dir, std::string(), languages, true);

        // 获取资源文件的统计报告
        counter1.Start(1, sc::mode_t::full_mode);
        auto [a, b, c, d] = _TotalReport(counter1.Reports());

        // 生成测试目录
        constexpr unsigned int n(100);
        _test_soource_dir _dir_inst(sources_dir, test_dir, n);

        // 定义统计器2，加载测试目录文件
        sc::Counter counter2;
        _xfAssert((nSource * n) == counter2.LoadFile(test_dir, std::string(), languages, true));

        // 对测试目录进行统计，比较统计结果
        counter2.Start(32, sc::mode_t::full_mode);
        _xfExpect(sc::report_t(a * n, b * n, c * n, d * n) == _TotalReport(counter2.Reports()));
    }

}
