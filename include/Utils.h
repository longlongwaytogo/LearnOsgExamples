#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <windows.h>
#include <io.h>

namespace Utils{

    class Helper 
    {

    private:
        Helper()
        {
            char buf[MAX_PATH];
            GetModuleFileNameA(NULL,buf,MAX_PATH);
            std::string appName = buf;
            m_strAppPath = appName.substr(0,appName.find_last_of("\\/"));
            m_strMediaPath  = m_strAppPath;
            std::string mediaPath = m_strMediaPath + "\\Media";

            // 向上两级回溯搜索
            if(_access(mediaPath.c_str(),0) != 0)
            {
                // not find,向上级目录搜索
                 m_strMediaPath +="\\..";
                 mediaPath = m_strMediaPath + "\\Media";
                 if(_access(mediaPath.c_str(),0) != 0)
                 {
                     m_strMediaPath +="\\..";
                     mediaPath = m_strMediaPath + "\\Media";
                 }
            }
            m_strMediaPath += "\\Media\\";
        }
        std::string m_strAppPath;
        std::string m_strMediaPath;

    public:

        static Helper& Ins()
        {
            static Helper s_helper;
            return s_helper;
        }

        std::string& getMediaPath()
        {
            return m_strMediaPath;
        }

        std::string& getAppPath()
        {
            return m_strAppPath;
        }

    };

   



}

#endif // __UTILS_H__ 
