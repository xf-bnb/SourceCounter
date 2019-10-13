#pragma once


namespace sc
{
    class FileReport;

    class Analyzer
    {
    public:

        // ����������
        enum LineType
        {
            UnknowLine = 0x00,     // δ֪����
            EmptyLine = 0x01,     // �հ���
            EffectiveLine = 0x02,     // ��Ч��
            CommentLine = 0x04,     // ע����

            TypeMask = 0x0f      // ��������
        };

        // ������״̬
        enum class LineMode : unsigned char
        {
            Nothing,                    // ��״̬
            Quoting,                    // ������
            Annotating                  // ע����
        };

        // ������������
        struct _analyze_arg {
            LineMode _lm{ LineMode::Nothing };
            unsigned int _arg{ 0 };
        };

        // ��Ӧģʽ�µķ�������������3���������໥��ϵ��ù�ͬ����һ�����ݵ����͡�
        /*
        static unsigned int AnalyzeByNothing(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByQuoting(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByAnnotating(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        */
    public:

        FileReport Analyze(const std::string& file) const;



    };

}
