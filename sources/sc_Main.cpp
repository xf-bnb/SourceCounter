#include <list>
#include <queue>
#include <iostream>

#include "utils/PathUtils.h"
#include "log/sc_log.h"
#include "config/ConfigInfo.h"
#include "counter/FileReport.h"
#include "counter/ReportList.h"
#include "counter/AnalyzeThread.h"

int main(int argc, char *argv[])
{
    // У�����
    if (argc < 2)
    {
        std::cout << "You don\'t specify any path !" << std::endl;
        return 0;
    }

    // ��ʼ����־
    _log_init(argv[0]);

    // ��ȡ·�������б�
    std::vector<std::string> dirList;
    for (int i = 1; i < argc; ++i)
        ExtractDirectory(dirList, argv[i]);

    // �ڲ����б���û���ҵ��κ���Ч��·��
    if (dirList.empty())
    {
        std::cout << "All of the paths you have specified are invalid !" << std::endl;
        return 0;
    }

    // ���������ļ�
    ConfigInfo config;
    config.Load(AppointSuffix(argv[0], ".ini").c_str());
    config.Show();

    // ��������̶߳���
    AnalyzeThread fileThread;

    // ����ָ�����ļ���׺����Ŀ¼��ͳ��Դ�����ļ��б�
    for (std::string& dir : dirList)
    {
        unsigned int n = fileThread.ExtractFile(dir, config.GetSuffixList());
        _log("found %d files from \"%s\"", n, dir.c_str());
    }

    // �����еĲ����б�·����û���ҵ��κ�Դ�����ļ�
    if (0 == fileThread.GetFileCount())
    {
        std::cout << "All of the paths you have specified hasn\'t found any source file !" << std::endl;
        return 0;
    }

    clock_t t1 = clock();

    // ����ͳ�Ʊ������
    ReportList reports(fileThread.GetFileCount());

    // ʹ��ָ�����ò�����������ͳ�ƹ���
    fileThread.Start(config.GetThreadCount(), reports, config.GetSingleList(), config.GetMultipleList());

    clock_t t2 = clock();

    // �Լ���õ�ͳ�ƽ������
    reports.Sort(
        [](const FileReport& a, const FileReport& b)
        {
            // Ĭ�ϰ��ļ�������
            return a.GetFilePath() < b.GetFilePath();
            // ���߰�������ʽ����
            // return a.GetFilePath()   > b.GetFilePath();
            // return a.GetTotal()      < b.GetTotal();
            // return a.GetEffective()  < b.GetEffective();
            // return a.GetSpendTime()  < b.GetSpendTime();
        }
    );

    clock_t t3 = clock();

    // ��ʾͳ�Ʊ�����
    reports.Show();

    _log("analyze %d files spend: %ums, sort spend: %ums", reports.GetTotalFile(), t2 - t1, t3 - t2);

    return 0;
}
