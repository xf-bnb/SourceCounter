#ifndef _Config_Info_H_
#define _Config_Info_H_

// ������Ϣ
class ConfigInfo
{
private:

    unsigned int m_ThreadCount{ 0 };    // �߳�����             
    list_type m_Suffixes;               // Ŀ���ļ����ͺ�׺�б�
    list_type m_Singles;                // ����ע�ͷ����б�
    pair_list m_Multiples;              // ����ע�ͱ�Ƕ��б�

    // �������õı���������͵�������
    void _AdjustConfig(const char* language);

public:

    const unsigned int& GetThreadCount() const { return m_ThreadCount; }
    const list_type& GetSuffixList() const { return m_Suffixes; }
    const list_type& GetSingleList() const { return m_Singles; }
    const pair_list& GetMultipleList() const { return m_Multiples; }

    // ��ʾ����
    void Show() const;

    // ���ļ���������
    bool Load(const char* fromFile);

};

#endif // _Config_Info_H_
