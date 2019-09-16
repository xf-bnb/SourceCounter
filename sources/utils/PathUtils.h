#ifndef _Path_Utils_H_
#define _Path_Utils_H_

#include <list>
#include <queue>

// ���幫�����ͺͷ���

using list_type = std::vector<std::string>;
using pair_list = std::vector<std::pair<std::string, std::string>>;
using queue_type = std::queue<std::string, std::list<std::string>>;

// �ж�Ŀ¼�Ƿ���Ч
bool IsValidDirectory(const char* dir);

// �ռ�Ŀ¼���б���
bool ExtractDirectory(list_type& pathlist, const char* dir);

// ɸѡָ��Ŀ¼��ָ�����͵��ļ���������
unsigned int FilterFile(queue_type& fileQueue, const std::string& dir, const list_type& suffixes);

// ���ļ�����ָ����׺�����֮ǰ�ĺ�׺�������滻�������µ��ļ�·����
std::string AppointSuffix(const char * appPath, const char * suffix);

#endif // _Path_Utils_H_
