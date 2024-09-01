#pragma once
#include "config.hpp"
#include "httplib.h"
#include "backupInfo.hpp"
#include "fileTool.hpp"
#include <vector>

extern cloudb::BackManage *_date;

namespace cloudb
{
    class Server
    {
    private:
        string _server_ip;
        int _server_port;
        string _download_prefix;
        httplib::Server _ser;

    private:
        void upload(const httplib::Request &req, httplib::Response &res) // 上传
        {
            auto ret = req.has_file("file");
            if (ret == false)
            {
                // 没有上传

                res.status = 400;
                return;
            }
            const auto &file = req.get_file_value("file");
            if (file.filename == "")
            {
                res.status = 400;
                return;
            }
            // 文件名称=存储目录+文件名
            string backdir = Config::getInstance()->getBackDir();
            string savapath = backdir + FileTool(file.filename).getFileName();
            cout << savapath << "-----" << endl;
            // 文件内容
            FileTool ft(savapath);
            if (ft.setContent(file.content) == false)
            {
                return;
            }
            // 更新_info_table表
            BackupInfo info(savapath);
            _date->insert(info);
            cout << "成功保存一个文件-> " << FileTool(file.filename).getFileName() << endl;

            // 响应
            std::stringstream ss;
            ss << "<!DOCTYPE html>";
            ss << "<html lang = 'en'>";
            ss << "<head>";
            ss << "<meta charset = 'UTF-8'>";
            ss << " <meta name = 'viewport' content = 'width=device-width,initial-scale = 1.0'>";
            ss<<"<title>upload</title>";
            ss << "</head>";
            ss << "<body>";
            ss << "<h1>"<<file.filename<<" 文件上传成功</h1>";
            ss << "</body> </html>";

            res.body = ss.str();
            res.set_header("Content-type", "text/html");
            res.status = 200;
            return;
        }
        string gettime(time_t time)
        {
            string stime = std::ctime(&time);
            return stime;
        }
        void listShow(const httplib::Request &req, httplib::Response &res) // 查看
        {
            std::vector<BackupInfo> arry;
            _date->getInfoAll(&arry); // 获取表中所有备份文件信息
            // 根据信息组织响应数据
            std::stringstream ss;
            ss << "<html><head><meta charset='UTF-8'><title>Download</title></head>";
            ss << "<body><h1>Download</h1><table>";
            for (auto &it : arry)
            {
                ss << "<tr>";
                string filename = FileTool(it._sava_path).getFileName();
                ss << "<td><a href='" << it._url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << gettime(it._mtime) << "</td>";
                ss << " <td align='right'>" << (float)(it._size / 1024.0) << "k</td>";
                ss << "</tr>";
            }
            ss << "</table></body></html>";

            res.body = ss.str();
            res.set_header("Content-type", "text/html");
            res.status = 200;
        }
        string getETag(const BackupInfo& info)
        {
            //文件名+文件大小+最后修改时间
            FileTool ft(info._sava_path);
            string s=ft.getFileName();
            s+='-';
            s+=std::to_string(info._size);
            s+='-';
            s+=std::to_string(info._mtime);
            return s;
        }
        void downLoad(const httplib::Request &req, httplib::Response &res) // 下载
        {
            BackupInfo info;
            _date->getInfoByUrl(req.path,&info);//获取备份信息

            //判断是否被压缩
            
            if(info._pack_flag==true)
            {
                FileTool ft(info._pack_path);
                if(ft.unpack(info._sava_path)==false)//将压缩文件解压缩到原来存储目录
                {
                    cout<<"访问压缩文件失败"<<endl;
                    return;
                }
                cout<<"解压缩成功"<<endl;
                ft.removeFile();
                info._pack_flag=false;
                _date->update(info);
            }

           
            //判断是否有断点续传请求
            bool isetag=false;//是否断点续传
            string old_etag;
            if(req.has_header("If-Range"))
            {
                string client_etag=req.get_header_value("If-Range");
                if(client_etag==getETag(info))
                {
                    //想同则符合断点续传
                    isetag=true;
                }
            }
            FileTool fts(info._sava_path);
            fts.getContent(&res.body);

            if(isetag==true)
            {
                //是断点续传
                res.set_header("Accept-Ranges", "bytes");
                res.set_header("ETag", getETag(info));
                //res.set_header("Content-Type", "application/octet-stream");
                res.status = 206;
            }
            else
            {
                res.set_header("Accept-Ranges", "bytes");
                res.set_header("ETag", getETag(info));
                res.set_header("Content-Type", "application/octet-stream");
                res.status = 200;
            }
           


        }

    public:
        Server()
        {
            Config *conf = Config::getInstance();
            _server_ip = conf->getServerIp();
            _server_port = conf->getServerPort();
            _download_prefix = conf->getDownloadPrefix();
        }
        void runModule()
        {
            _ser.Post("/upload", [&](const httplib::Request &req, httplib::Response &res)
                      { this->upload(req, res); });
            _ser.Get("/listshow", [&](const httplib::Request &req, httplib::Response &res)
                     { this->listShow(req, res); });
            _ser.Get("/", [&](const httplib::Request &req, httplib::Response &res)
                     { this->listShow(req, res); });

            string download_url = _download_prefix + "(.*)";
            _ser.Get(download_url, [&](const httplib::Request &req, httplib::Response &res)
                     { this->downLoad(req, res); });
            _ser.listen(_server_ip.c_str(), _server_port);
        }
    };
}