#pragma once
#include <mutex>
#include "fileTool.hpp"

namespace cloudb
#define CONF_FILE "./cloud.conf"
{
    class Config
    {
    private:
        Config() 
        {
            if(!readConfigFile())
            {
                exit(-1);
            }
        }
        static Config *_instance;
        static std::mutex _mtx;

    private:
        int _hot_time; // 热点时间
        int _server_port;
        string _server_ip;
        string _download_prefix; // 下载文件前缀  /download
        string _packfile_suffix; // 压缩包后缀
        string _pack_dir;        // 压缩包文件
        string _back_dir;        // 备份文件目录
        string _backup_file;     // 备份文件信息

        bool readConfigFile()
        {
            cloudb::FileTool ft(CONF_FILE);
            string body;
            if (ft.getContent(&body) == false)
            {
                cout << "读取配置文件失败..." << endl;
                return false;
            }

            Json::Value root;
            JsonTool::unSerialize(body, &root);
            _hot_time = root["hot_time"].asInt();
            _server_port = root["server_port"].asInt();
            _server_ip = root["server_ip"].asString();
            _download_prefix = root["download_prefix"].asString();
            _packfile_suffix = root["packfile_suffix"].asString();
            _pack_dir = root["pack_dir"].asString();
            _back_dir = root["back_dir"].asString();
            _backup_file = root["backup_file"].asString();
            return true;
        }

    public:
        static Config *getInstance()
        {
            if(_instance==nullptr)
            {
                _mtx.lock();
                if(_instance==nullptr)
                {
                    _instance=new Config();
                }
                _mtx.unlock();

            }
            return _instance;
        }
        int getHotTime()
        {
            return _hot_time;
        }
        int getServerPort()
        {
            return _server_port;
        }
        string getServerIp()
        {
            return _server_ip;
        }
        string getDownloadPrefix()
        {
            return _download_prefix;
        }
        string getPackfileSuffix()
        {
            return _packfile_suffix;
        }
        string getPackDir()
        {
            return _pack_dir;
        }
        string getBackDir()
        {
            return _back_dir;
        }
        string getBackupFile()
        {
            return _backup_file;
        }
    };
    Config *Config::_instance = nullptr;
    std::mutex Config::_mtx;
}

