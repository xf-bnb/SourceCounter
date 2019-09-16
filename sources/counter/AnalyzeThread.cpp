#include <list>
#include <queue>
#include <thread>
#include <iostream>
#include <fstream>

#include "utils/PathUtils.h"
#include "log/sc_log.h"
#include "counter/FileReport.h"
#include "counter/ReportList.h"
#include "counter/AnalyzeThread.h"

bool AnalyzeThread::Start(unsigned int nThread, ReportList& reports, const list_type& singles, const pair_list& multiples)
{
    if (0 < nThread && !m_FileQueue.empty())
    {
        /*
         * ���� nThread ���̣߳��� Analyze ��Ա�����󶨵��̶߳�����뵽�߳������С�
         * Analyze �������ڶ���߳���ͬʱִ�У��������������ӵ� reports �����С�
         * ʹ�� std::bind ����Ա���� Analyze ���̶߳���󶨣�ʹ�� std::ref �����������õķ�ʽ���̰߳󶨡�
         */
        std::vector<std::thread> vtrThread;
        for (unsigned int i = 0; i < nThread; ++i)
            // vtrThread.emplace_back(std::thread(std::bind(&AnalyzeThread::_Analyze, this, std::ref(reports), std::ref(singles), std::ref(multiples))));
            vtrThread.emplace_back(std::thread([this, &reports, &singles, &multiples]() mutable { this->_Analyze(reports, singles, multiples); }));

        // ���ε��� join �����ȴ������߳�ִ�����
        for (std::thread& t : vtrThread)
            t.join();

        return true;
    }

    return false;
}

unsigned int AnalyzeThread::ExtractFile(const std::string & fromPath, const list_type & suffixes)
{
    return FilterFile(m_FileQueue, fromPath, suffixes);
}

bool AnalyzeThread::_PickFile(std::string & file)
{
    /*
     * ȡ�ļ������ǻ������
     * ͬһʱ��ֻ����һ���߳̿��Դ��ļ�����ȡ���ļ�
     * �������Ϊ����ȡ�ļ�ʧ�ܣ�����ȡ�����ļ��Ӳ����������������ļ��Ӷ����Ƴ���
     */
    std::lock_guard<std::mutex> automtx(m_Mutex);

    if (m_FileQueue.empty())
        return false;

    file = m_FileQueue.front();
    m_FileQueue.pop();
    return true;
}

void AnalyzeThread::_Analyze(ReportList& reports, const list_type& singles, const pair_list& multiples)
{
    // ѭ��ȡ�ļ�
    for (std::string file; _PickFile(file); )
    {
        // �����������ӵ�ͳ�Ʊ����б�
        reports.AddReport(AnalyzeFile(file, singles, multiples));
    }
}

FileReport AnalyzeThread::AnalyzeFile(const std::string & file, const list_type & singles, const pair_list & multiples)
{
    // ������ļ���ͳ�Ʊ������
    FileReport fr(file);
    clock_t t = clock();
    try
    {
        std::ifstream fin;
        fin.open(file);
        if (fin.is_open())
        {
            for (_analyze_arg aa; ; )
            {
                char line[0x0400]{ 0 };
                if (!fin.getline(line, 0x0400))
                    break;

                unsigned int ly = LineType::UnknowLine;
                // ���ݵ�ǰ�е�״̬�ֱ���з���
                switch (aa._lm)
                {
                case LineMode::Nothing:
                    ly = AnalyzeByNothing(aa, line, singles, multiples);
                    break;
                case LineMode::Quoting:
                    ly = AnalyzeByQuoting(aa, line, singles, multiples);
                    break;
                case LineMode::Annotating:
                    ly = AnalyzeByAnnotating(aa, line, singles, multiples);
                    break;
                default:
                    break;
                }

                // _log("line: %s\ntype: %d, mode: %d", line, ly, static_cast<int>(aa._lm));

                // ��������+1
                fr.AddTotal(1);

                // ���ݷ������ͳ�ƶ�Ӧ���͵�����
                if (LineType::EffectiveLine == (LineType::EffectiveLine & ly))
                    fr.AddEffective(1);
                if (LineType::CommentLine == (LineType::CommentLine & ly))
                    fr.AddComment(1);

                // ����Ǵ��п��б�ǲ���û���������ͱ����Ϊ����
                if (LineType::EmptyLine == (LineType::EmptyLine & ly) && 0 == (ly & (LineType::EffectiveLine | LineType::CommentLine)))
                    fr.AddEmpty(1);
            }

            fin.close();
        }
        else
        {
            _log("open file \"%s\" failed !", file.c_str());
        }
    }
    catch (const std::exception& _ex)
    {
        _log("analyze file \"%s\" occur exception: %s", file.c_str(), _ex.what());
    }

    // ��¼����ʱ��
    fr.SetSpendTime(clock() - t);

    _log("file: %s, tolal: %d, empty: %d, effective: %d, comment: %d, time: %d"
        , fr.GetFilePath().c_str()
        , fr.GetTotal()
        , fr.GetEmpty()
        , fr.GetEffective()
        , fr.GetComment()
        , fr.GetSpendTime());

    return fr;
}

// ������Ч����Сֵ����
static inline unsigned int _TellFirstPos(const int(&_arr)[4])
{
    int minpos = INT_MAX;
    unsigned int x = 0;

    for (unsigned int i = 1; i < 4; ++i)
    {
        if (0 < _arr[i] && _arr[i] < minpos)
        {
            x = i;
            minpos = _arr[i];
        }
    }

    return x;
}

// ���ҵ�һ����Ч������λ��
static inline const char* _FindQuotePos(const char* start)
{
    if (*start == '\"')
        return start;

    for (;;)
    {
        // ���������ַ�λ��
        const char* ptr = strchr(start, '\"');
        // �Ҳ���ֱ�ӷ��ؿ�
        if (ptr == nullptr)
            return nullptr;

        // ͳ������ǰ�������ķ�б������
        unsigned int n = 0;
        for (const char* slash = ptr - 1; ; --slash)
        {
            if (*slash == '\\')
                ++n;
            else
                break;

            if (slash == start)
                break;
        }
        // ���ǰ��ķ�б���������Ե��������ҵ���һ����Ч������λ��
        if (0 == n % 2)
            return ptr;

        // �����´β���λ��
        start = ptr + 1;
    }
}

unsigned int AnalyzeThread::AnalyzeByNothing(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return AnalyzeThread::LineType::EmptyLine;

    // ������������λ��
    int qkey = -1;
    const char* qptr = _FindQuotePos(start);
    // ������������ף����������ģʽ
    if (qptr == start)
    {
        _aa._lm = LineMode::Quoting;
        return LineType::EffectiveLine | AnalyzeByQuoting(_aa, start + 1, singles, multiples);
    }
    // ����ҵ����¼�ҵ���λ��
    if (qptr != nullptr)
        qkey = qptr - start;

    // ���ҵ�һ������ע�͵�λ��
    std::pair<int, int> skey{ -1, -1 };
    for (unsigned int i = 0; i < singles.size(); ++i)
    {
        const char* ptr = strstr(start, singles[i].c_str());
        if (ptr != nullptr)
        {
            int pos = ptr - start;
            if (skey.first < 0 || pos < skey.second)
            {
                skey.first = i;
                skey.second = pos;
            }
        }
    }
    // �������ע�ͷ��������ף�ֱ�ӷ���ע����
    if (skey.second == 0)
    {
        _aa._lm = LineMode::Nothing;
        return AnalyzeThread::LineType::CommentLine;
    }

    // ���ҵ�һ������ע�͵�λ��
    std::pair<int, int> mkey{ -1, -1 };
    for (unsigned int i = 0; i < multiples.size(); ++i)
    {
        const char* ptr = strstr(start, multiples[i].first.c_str());
        if (ptr != nullptr)
        {
            int pos = ptr - start;
            if (mkey.first < 0 || pos < mkey.second)
            {
                mkey.first = i;
                mkey.second = pos;
            }
        }
    }
    // �������ע�ͷ��������ף������ע��ģʽ
    if (mkey.second == 0)
    {
        _aa._lm = LineMode::Annotating;
        _aa._arg = mkey.first;
        return LineType::CommentLine | AnalyzeByAnnotating(_aa, start + multiples[mkey.first].first.size(), singles, multiples);
    }

    unsigned int index = _TellFirstPos({ INT_MAX, qkey, skey.second, mkey.second });
    switch (index)
    {
    case 0:
    {
        _aa._lm = LineMode::Nothing;
        // ���û���ҵ��κ����š�����ע�͡�����ע�ͱ�ǣ����������ַ�����һ���ǿհ׷���Ϊ��Ч�����У�����Ϊ�հ��С�
        for (const unsigned char* ptr = reinterpret_cast<const unsigned char*>(start); *ptr; ++ptr)
            if (0 == isspace(*ptr))
                return LineType::EffectiveLine;

        return LineType::EmptyLine;
    }
    case 1:
    {
        // ������ű������ǰ�棬���������ģʽ
        _aa._lm = LineMode::Quoting;
        return AnalyzeThread::LineType::EffectiveLine | AnalyzeByQuoting(_aa, start + qkey + 1, singles, multiples);
    }
    case 2:
    {
        _aa._lm = LineMode::Nothing;
        // �������ע�ͱ������ǰ�棬���ע��֮ǰ�Ƿ�����Ч����
        for (int i = 0; i < skey.second; ++i)
            if (0 == isspace((unsigned int)start[i]))
                return LineType::EffectiveLine | LineType::CommentLine;

        return LineType::CommentLine;
    }
    case 3:
    {
        _aa._lm = LineMode::Annotating;
        _aa._arg = mkey.first;
        // �������ע�ͱ������ǰ�棬���ע��֮ǰ�Ƿ�����Ч����
        unsigned int ly = AnalyzeThread::LineType::CommentLine;
        for (int i = 0; i < mkey.second; ++i)
        {
            if (0 == isspace((unsigned int)start[i]))
            {
                ly |= LineType::EffectiveLine;
                break;
            }
        }
        // ������ű������ǰ�棬���������ģʽ
        return ly | AnalyzeByAnnotating(_aa, start + mkey.second + multiples[mkey.first].first.size(), singles, multiples);
    }
    default:
        break;
    }

    return 0;
}

unsigned int AnalyzeThread::AnalyzeByQuoting(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return LineType::EffectiveLine;

    // �������Ž������λ��
    const char* ptr = _FindQuotePos(start);
    // �Ҳ���ֱ�ӷ�����Ч������
    if (ptr == nullptr)
        return LineType::EffectiveLine;

    // ����ҵ����������ģʽ��������ͨģʽ�ж�
    _aa._lm = LineMode::Nothing;
    return LineType::EffectiveLine | AnalyzeByNothing(_aa, ptr + 1, singles, multiples);
}

unsigned int AnalyzeThread::AnalyzeByAnnotating(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return LineType::EmptyLine;

    // ���Ҷ�Ӧ�Ķ���ע�ͽ������
    const char* ptr = strstr(start, multiples[_aa._arg].second.c_str());
    // ���û���ҵ�
    if (ptr == nullptr)
    {
        for (ptr = start; *ptr; ++ptr)
            if (0 == isspace(unsigned int(*ptr)))
                return LineType::CommentLine;

        return LineType::EmptyLine;
    }
    else
    {
        // ����ҵ��������ע��ģʽ��������ͨģʽ�ж�
        _aa._lm = LineMode::Nothing;
        return LineType::CommentLine | AnalyzeByNothing(_aa, ptr + multiples[_aa._arg].second.size(), singles, multiples);
    }
}
