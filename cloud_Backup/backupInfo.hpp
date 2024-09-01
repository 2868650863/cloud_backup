#pragma once
#include <unordered_map>
#include <pthread.h>
#include <utility>
#include "fileTool.hpp"
#include "config.hpp"


namespace cloudb
{
    class BackupInfo
    {
    public:
        bool _pack_flag; // 是否压缩
        size_t _size;
        time_t _atime;
        time_t _mtime;
        string _sava_path;
        string _pack_path;
        string _url;

    public:
        BackupInfo()
        {
        }
        void printbm()
        {
            cout << "---------------------" << endl;
            cout << _pack_flag << endl;
            cout << _size << endl;
            cout << _atime << endl;
            cout << _mtime << endl;
            cout << _sava_path << endl;
            cout << _pack_path << endl;
            cout << _url << endl;
            cout << "---------------------" << endl;
        }
        BackupInfo(const BackupInfo& other)
        {
          
            this->_pack_flag=other._pack_flag;
            this->_size=other._size;
            this->_atime = other._atime;
            this->_mtime = other._mtime;
            this->_sava_path = other._sava_path;
            this->_pack_path = other._pack_path;
            this->_url = other._url;
        }
        BackupInfo& operator=(BackupInfo other)
        {
            this->_pack_flag = other._pack_flag;
            this->_size = other._size;
            this->_atime = other._atime;
            this->_mtime = other._mtime;
            this->_sava_path = other._sava_path;
            this->_pack_path = other._pack_path;
            this->_url = other._url;
            
            
            return *this;
        }

        BackupInfo(const string &path)
        {

            FileTool ft(path);
            if (ft.exists() == false)
            {
                cout << "文件不存在..." << endl;
            }
            else
            {
                cloudb::Config *cf = cloudb::Config::getInstance();

                this->_pack_flag = false;
                this->_size = ft.fileSize();
                this->_atime = ft.lastATime();
                this->_mtime = ft.lastMTime();
                this->_sava_path = path;
                // 压缩包目录+包名+压缩后缀
                this->_pack_path = cf->getPackDir() + ft.getFileName() + cf->getPackfileSuffix();
                this->_url = cf->getDownloadPrefix() + ft.getFileName();
            }
        }
    };

    class BackManage
    {
    private:
        string _backup_file;
        pthread_rwlock_t _rwlock;                           // 读写锁
        std::unordered_map<string, BackupInfo> _info_table; // url,info
    public:
        BackManage()
        {
            _backup_file = Config::getInstance()->getBackupFile();
            pthread_rwlock_init(&_rwlock, nullptr);
            loadDat();
        }
        ~BackManage()
        {
            pthread_rwlock_destroy(&_rwlock);
        }
        bool insert(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _info_table[info._url] = info;
            pthread_rwlock_unlock(&_rwlock);
            savefile();
            return true;
        }
        bool update(const BackupInfo &info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _info_table[info._url] = info;
            pthread_rwlock_unlock(&_rwlock);
            savefile();
            return true;
        }
        bool getInfoByUrl(const string &url, BackupInfo *info) // 通过url获取信息
        {
            pthread_rwlock_wrlock(&_rwlock);
            const auto &it = _info_table.find(url);
            if (it == _info_table.end())
            {
                return false;
                pthread_rwlock_unlock(&_rwlock);
            }
            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        bool getInfoBySavaPath(const string &savapath, BackupInfo *info) // 通过存储路径获取信息
        {
            pthread_rwlock_wrlock(&_rwlock);
            
            auto it = _info_table.begin();
            while (it != _info_table.end())
            {
                if (it->second._sava_path == savapath)
                {
                    *info = it->second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
                ++it;
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }
        bool getInfoAll(std::vector<BackupInfo> *arry) // 获取所有
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _info_table.begin();
            while (it != _info_table.end())
            {
                arry->push_back(it->second);
                ++it;
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        // 保存文件
        bool savefile()
        {
            std::vector<BackupInfo> vb; 
            this->getInfoAll(&vb);

            Json::Value root;
            for (int i = 0; i < vb.size(); ++i)
            {
                Json::Value item;
                item["_pack_flag"] = vb[i]._pack_flag;
                item["_size"] = (Json::Int64)vb[i]._size;
                item["_atime"] = (Json::Int64)(vb[i]._atime);
                item["_mtime"] = (Json::Int64)(vb[i]._mtime);
                item["_sava_path"] = vb[i]._sava_path;
                item["_pack_path"] = vb[i]._pack_path;
                item["_url"] = vb[i]._url;
                root.append(item);
            }

            string body;
            JsonTool::serialize(root, &body); // 序列化

            FileTool ft(_backup_file);
            if (ft.setContent(body) == false)
            {
                return false;
            }
            return true;
        }
        // 加载数据
        bool loadDat()
        {

            FileTool ft(_backup_file);
            if (ft.exists() == false)
            {
                return false;
            }
            if (ft.fileSize() == 0)
            {
                return false;
            }
            string body;
            if (ft.getContent(&body) == false)
            {
                return false;
            }
            // 反序列化
            Json::Value root;
            JsonTool::unSerialize(body, &root);
            for (int i = 0; i < root.size(); ++i)
            {
                BackupInfo info;
                info._size = root[i]["_size"].asInt64();
                info._atime = root[i]["_atime"].asInt64();
                info._mtime = root[i]["_mtime"].asInt64();
                info._pack_flag = root[i]["_pack_flag"].asBool();
                info._sava_path = root[i]["_sava_path"].asString();
                info._pack_path = root[i]["_pack_path"].asString();
                info._url = root[i]["_url"].asString();
                insert(info);
            }

            return true;
        }
    };
}