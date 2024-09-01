#pragma once
#include "backupInfo.hpp"
#include "fileTool.hpp"

extern cloudb::BackManage *_date;

namespace cloudb
{
    class HotFileManage
    {
    private:
        string _back_dir;
        string _pack_dir;
        string _pack_dir_suffix;
        int _hot_time;

    public:
        HotFileManage()
        {
            Config *conf = Config::getInstance();
            _back_dir = conf->getBackDir();
            _pack_dir = conf->getPackDir();
            _pack_dir_suffix = conf->getPackfileSuffix();
            _hot_time = conf->getHotTime();
            FileTool ft1(_back_dir);
            FileTool ft2(_pack_dir);
            ft1.createDirectory();
            ft2.createDirectory();
        }
        // 判断是否非热点文件
        bool hotFileJudge(const string &name)
        {
            FileTool ft(name);
            time_t last_time = ft.lastATime();
            time_t cur_time = time(nullptr);

            if (cur_time - last_time > _hot_time)
            {
                return true;
            }
            return false;
        }
        bool runHotModule()
        {

            while (true)
            {
                FileTool ft(_back_dir);
                std::vector<string> vs;
                ft.scanDirectory(&vs); // 获取目录下的所有文件

                for (auto it : vs)
                {
                    if (hotFileJudge(it) == true)
                    {
                        // 是非热点文件
                        cout << it << "：不是热点文件" << endl;
                        BackupInfo info;
                        // 判断是否在info_table表中
                        if (_date->getInfoBySavaPath(it, &info) == false)
                        {
                            // cout<<"path未在表中"<<endl;
                            info = BackupInfo(it);
                        }
                        // 压缩删除
                        // cout<<"1111"<<endl;
                        FileTool ft1(it);
                        if (ft1.pack(info._pack_path))
                        {
                            cout << it << "->文件压缩成功.." << endl;
                        }
                        ft1.removeFile();
                        info._pack_flag = true;
                        _date->update(info);
                    }
                }
                sleep(5);
            }
            return true;
        }
    };
}