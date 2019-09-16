
#include <io.h>
#include <string>
#include <queue>
#include <list>

#include "utils/PathUtils.h"
#include "log/sc_log.h"

// �ж��ַ��� str �Ƿ��� suffixes ����һԪ�ؽ�β
static inline bool IsEndWith(const list_type& suffixes, const char* str)
{
    unsigned int n = strlen(str);
    for (const std::string& s : suffixes)
    {
        if (s.size() < n)
        {
            if (0 == strcmp(s.c_str(), str + n - s.size()))
            {
                return true;
            }
        }
    }

    return false;
}

bool IsValidDirectory(const char* dir)
{
    struct _finddata_t _info;
    if (_findfirst(dir, &_info) != -1L)
    {
        return (_A_SUBDIR == (_A_SUBDIR & _info.attrib));
    }

    return false;
}

bool ExtractDirectory(list_type& pathlist, const char * path)
{
    std::string dir(path);
    // ͳһĿ¼�ָ���
    std::replace(dir.begin(), dir.end(), '\\', '/');
    if (dir.back() == '/')
        dir.pop_back();

    // �ж�Ŀ¼�Ƿ���Ч
    if (IsValidDirectory(dir.c_str()))
    {
        pathlist.emplace_back(dir);
        return true;
    }
    else
    {
        _log("the path isn\'t existing or isn\'t a valid direatory !");
        return false;
    }
}

unsigned int FilterFile(queue_type& fileQueue, const std::string& dir, const list_type& suffixes)
{
    struct _finddata_t fileInfo;

    intptr_t hFile = _findfirst((dir + "/*.*").c_str(), &fileInfo);
    if (-1L == hFile)
    {
        _log("the path \"%s\" is invalid !", dir.c_str());
        return 0;
    }

    unsigned int n = 0;

    do {
        if (_A_SUBDIR == (_A_SUBDIR & fileInfo.attrib))
        {
            if (0 != strcmp(".", fileInfo.name) && 0 != strcmp("..", fileInfo.name))
            {
                // �������ЧĿ¼����еݹ�����
                n += FilterFile(fileQueue, dir + '/' + fileInfo.name, suffixes);
            }
        }
        else
        {
            // ���ļ�����׺ƥ����ļ���ӵ�������
            if (suffixes.empty() || IsEndWith(suffixes, fileInfo.name))
            {
                std::string target = dir + '/' + fileInfo.name;
                _log("found file: %s", target.c_str());
                fileQueue.emplace(target);
                ++n;
            }
            else
            {
                // _log("ignore file: %s", (dir + '/' + fileInfo.name).c_str());
            }
        }
    } while (_findnext(hFile, &fileInfo) == 0);

    _findclose(hFile);

    return n;
}

std::string AppointSuffix(const char * appPath, const char * suffix)
{
    std::string resultPath(appPath);
    // ͳһĿ¼�ָ���
    std::replace(resultPath.begin(), resultPath.end(), '\\', '/');

    // ƴ����־�ļ���
    std::size_t point = resultPath.rfind('.');
    if (std::string::npos == point)
    {
        resultPath.append(suffix);
    }
    else
    {
        std::size_t slash = resultPath.rfind('/');
        if (std::string::npos == slash || slash < point)
            resultPath.replace(point, std::string::npos, suffix);
        else
            resultPath.append(suffix);
    }

    return resultPath;
}
