#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <filesystem>
//#include "bundle.h"
//#include <jsoncpp/json/json.h>
#include <memory>

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace cloudb
{
    namespace fs = std::filesystem;

    //class JsonTool
    //{
    //public:
    //    static bool serialize(const Json::Value& root, std::string* str)
    //    {
    //        //Json::StreamWriterBuilder swb;
    //        std::unique_ptr<Json::StreamWriter> ptr(Json::StreamWriterBuilder().newStreamWriter());
    //        std::stringstream ss;
    //        ptr->write(root, &ss);
    //        *str = ss.str();
    //        return true;
    //    }
    //    static bool unSerialize(const std::string& str, Json::Value* root)
    //    {
    //        std::unique_ptr<Json::CharReader> ptr(Json::CharReaderBuilder().newCharReader());
    //        string err;
    //        bool ret = ptr->parse(str.c_str(), str.c_str() + str.size(), root, &err);
    //        if (ret == false)
    //        {
    //            cout << "unserialize fail" << endl;
    //            return false;
    //        }
    //        return true;
    //    }
    //};


    class FileTool
    {
    private:
        std::string _filename; // 文件名   /a/b/c/xxx.txt

    public:
        FileTool(const std::string& filename)
            : _filename(filename)
        {
        }
        // 获取文件大小
        uint64_t fileSize()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat file 错误..." << std::endl;
                return -1;
            }
            return st.st_size;
        }
        // 文件最后修改时间
        time_t lastMTime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat file 错误..." << std::endl;
                return -1;
            }
            return st.st_mtime;
        }

        // 文件最后访问时间
        time_t lastATime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "stat file 错误..." << std::endl;
                return -1;
            }
            return st.st_atime;
        }
        // 不带路径的获取  文件名
        std::string getFileName()
        {
            size_t pos = _filename.find_last_of("/");
            if (pos == std::string::npos)
            {
                return _filename;
            }
            return _filename.substr(pos + 1);
        }

        // 获取指定的文件内容
        bool getContentPosLen(string* body, size_t pos, size_t len)
        {
            if (pos + len > this->fileSize())
            {
                return false;
            }
            std::ifstream ifs;
            ifs.open(_filename, std::ifstream::binary);
            if (ifs.is_open() == false)
            {
                cout << "打开文件失败..." << endl;
                return false;
            }
            (*body).resize(len);
            ifs.seekg(pos, std::ios::beg);
            if (ifs.good() == false)
            {
                cout << "seekg 失败..." << endl;
                ifs.close();
                return false;
            }
            ifs.read(&(*body)[0], len);
            if (ifs.good() == false)
            {
                cout << "read 失败..." << endl;
                ifs.close();
                return false;
            }
            ifs.close();
            return true;
        }

        // 获取全部文件内容
        bool getContent(string* body)
        {
            return getContentPosLen(body, 0, this->fileSize());
        }

        // 向文件中写入内容
        bool setContent(const string& body)
        {
            std::ofstream ofs;
            ofs.open(_filename, std::ofstream::binary);
            if (ofs.is_open() == false)
            {
                cout << "open 失败..." << endl;
                return false;
            }
            ofs.write(&body[0], body.size());
            if (!ofs.good())
            {
                cout << "写入文件失败..." << endl;
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }

        //删除文件
        bool removeFile()
        {
            if (this->exists() == false)
            {
                return true;
            }
            if (remove(_filename.c_str()) == 0)
            {
                //cout<<"删除文件成功"<<endl;
            }
            else
            {
                cout << "删除文件失败" << endl;
                return false;
            }
            return true;
        }

        // 压缩文件
       /* bool pack(const string& packname)
        {
            string body;
            if (this->getContent(&body) == false)
            {

                cout << "read fail..." << endl;
            }
            string packs = bundle::pack(bundle::LZIP, body);

            FileTool ft(packname);
            if (ft.setContent(packs) == false)
            {
                cout << "weite fail..." << endl;
            }
            return true;
        }
         解压缩文件
        bool unpack(const string& unpackname)
        {
            string body;
            if (this->getContent(&body) == false)
            {
                return false;
            }
            string unpackbody = bundle::unpack(body);
            FileTool ft(unpackname);
            if (ft.setContent(unpackbody) == false)
            {
                return false;
            }

            return true;
        }*/

        //判断文件是否存在
        bool exists()
        {
            return fs::exists(_filename);
        }
        //创建目录
        bool createDirectory()
        {
            if (this->exists())
            {
                return true;
            }
            return fs::create_directories(_filename);
        }
        //浏览目录下的所有文件
        bool scanDirectory(std::vector<string>* arry)
        {
            if (exists() == false)
            {
                createDirectory();
            }
            for (auto& p : fs::directory_iterator(_filename))
            {
                if (fs::is_directory(p))
                {
                    continue;
                }
                arry->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }


    };
}
