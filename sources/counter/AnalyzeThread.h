#ifndef _Analyze_Thread_H_
#define _Analyze_Thread_H_

#include <mutex>

// �����߳���
class AnalyzeThread
{
public:

    // ����������
    enum LineType
    {
        UnknowLine      = 0x00,     // δ֪����
        EmptyLine       = 0x01,     // �հ���
        EffectiveLine   = 0x02,     // ��Ч��
        CommentLine     = 0x04,     // ע����

        TypeMask        = 0x0f      // ��������
    };

    // ������״̬
    enum class LineMode : unsigned char
    {
        Nothing,                    // ��״̬
        Quoting,                    // ������
        Annotating                  // ע����
    };

private:

    queue_type m_FileQueue;         // �ļ�����
    std::mutex m_Mutex;             // �߳���

    // ������������
    struct _analyze_arg {
        LineMode _lm{ LineMode::Nothing };
        unsigned int _arg{ 0 };
    };

    // ��ֹ����
    AnalyzeThread(const AnalyzeThread&) = delete;
    AnalyzeThread& operator=(const AnalyzeThread&) = delete;

    // ȡһ���ļ�
    bool _PickFile(std::string& file);

    // �����߳�
    void _Analyze(ReportList& reports, const list_type& singles, const pair_list& multiples);

    // ����һ���ļ�������ͳ�Ʊ���
    static FileReport AnalyzeFile(const std::string& file, const list_type& singles, const pair_list& multiples);
    
    // ��Ӧģʽ�µķ�������������3���������໥��ϵ��ù�ͬ����һ�����ݵ����͡�
    static unsigned int AnalyzeByNothing(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
    static unsigned int AnalyzeByQuoting(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
    static unsigned int AnalyzeByAnnotating(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);

public:

    // Ĭ�Ϲ���
    AnalyzeThread() = default;

    const queue_type& GetFileQueue() const { return m_FileQueue; }
    unsigned int GetFileCount() const { return m_FileQueue.size(); }

    // ��ָ��Ŀ¼����ȡָ����׺����Դ�ļ���������
    unsigned int ExtractFile(const std::string& fromPath, const list_type& suffixes);

    // �����߳�
    bool Start(unsigned int nThread, ReportList& reports, const list_type& singles, const pair_list& multiples);

};

#endif // _Analyze_Thread_H_
