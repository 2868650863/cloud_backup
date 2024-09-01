#pragma once
#include "BackDateManage.hpp"

#include <stdio.h>
#include "httplib.h"
#include <Windows.h>

const string server_ip = "47.115.60.56";
const int server_port = 8080;

namespace cloudb
{
	class CloudBackup
	{
	private:
		string _backup_dir;//备份目录
		BackDateManage* _date;
	public:
		CloudBackup(const string& back_dir, const string& back_file)
			:_backup_dir(back_dir)
		{
			_date = new BackDateManage(back_file);
		}

		//获取文件唯一标识
		string getFileOnlyLogo(const string& filename)
		{
			//文件名-文件大小-文件最后修改时间
			FileTool ft(filename);
			std::stringstream ss;
		
		ss << ft.getFileName() << "-" << ft.fileSize() << "-" << ft.lastMTime();
		return ss.str();
		}

		bool isNeedUpload(const string& filename)
		{
			//条件  新增或者修改过
			string id;
			if (_date->getDateByKey(filename, &id) == true)
			{
				//表中有，不是新增
				string new_id = getFileOnlyLogo(filename);
				if (new_id == id)
				{
					//没有修改
					return false;
				}
				//_date->update(filename, new_id);
				return true;

			}
			//新增
			FileTool ft1(filename);
			if (time(nullptr) - ft1.lastMTime() < 3)
			{
				
				return false;
			}
			//_date->update(filename, getFileOnlyLogo(filename));
			return true;
		}

		bool upload(const string& filename)
		{
			FileTool ft(filename);
			string body;
			ft.getContent(&body);

			//上传
			httplib::Client client(server_ip, server_port);
			httplib::MultipartFormData item;
			item.content = body;
			item.filename = ft.getFileName();
			item.name = "file";
			item.content_type = "application/octet-stream";

			httplib::MultipartFormDataItems items;
			items.push_back(item);
			auto res = client.Post("/upload", items);
			if (!res || res->status != 200)
			{
				return false;
			}
			//cout << filename << " -> 文件上传成功" << endl;
			return true;

		}

		void runModule()
		{
			while (1)
			{
				FileTool ft(_backup_dir);
				vector<string> vs;
				ft.scanDirectory(&vs);
				for (auto& it : vs)
				{
					if (isNeedUpload(it) == true)
					{
						cout << it << "->需要备份..." << endl;
						if(upload(it)==true)
						_date->insert(it, getFileOnlyLogo(it));
						cout << it << "-> 文件上传成功..." << endl;
						
					}
					
				}
				Sleep(1);
			}
		
		}
	};
}
