#pragma once
#include "FileTool.hpp"
#include <unordered_map>
#include <sstream>

namespace cloudb
{
	class BackDateManage
	{
	private:
		string _back_date_file;//备份文件信息存储
		std::unordered_map<string, string> _table;
	public:
		BackDateManage(const string& filename)
			:_back_date_file(filename)
		{
			initLoad();
		}
		bool savaFile()
		{
			auto it = _table.begin();
			std::stringstream ss;
			while(it != _table.end())
			{
				ss << it->first << " " << it->second << "\n";
				++it;
			}
			//保存信息
			FileTool ft(_back_date_file);
			if (ft.setContent(ss.str()) == false)
			{
				cout << "保存文件失败..." << endl;
				return false;
			}
			return true;
			
		}
		void cutString(const string& str, std::vector<std::vector<string>>* vs)
		{
			size_t pos = 0, seek = 0;
			while (true)
			{
				string key;
				string value;
					
				pos = str.find(' ', seek);
				if (pos == string::npos)
				{
					break;
					
				}
				key = str.substr(seek, pos - seek);
				++pos;
				seek = pos;
				pos = str.find('\n', seek);
				value = str.substr(seek, pos - seek);
				std::vector<string > tmp;
				if(!key.empty())
				tmp.push_back(key);
				if(!value.empty())
				tmp.push_back(value);
				vs->push_back(tmp);
				++pos;
				if (pos > str.size())
				{
					break;
				}
				seek = pos;
			}
			return;
		}
		bool initLoad()
		{
			FileTool ft(_back_date_file);
			string body;
			ft.getContent(&body);
			std::vector<std::vector<string>> arry;
			cutString(body, &arry);
			for (auto& it : arry)
			{
				if (it.size() != 2)
				{
					cout << "cutstring 错误" << endl;
					return false;
				}
				_table[it[0]] = it[1];
			}
			return true;
		}
		void print()
		{
			for (auto it : _table)
			{
				cout << it.first << ": " << it.second << endl;
			}
		}
		bool insert(const string& key, const string& value)
		{
			_table[key] = value;
			savaFile();
			return true;
		}
		bool update(const string& key, const string& value)
		{
			_table[key] = value;
			savaFile();
			return true;
		}
		bool getDateByKey(const string& key, string* value)
		{
			
			auto it=_table.find(key);
			if (it == _table.end())
			{
				//cout << "没有这个文件" << endl;
				return false;
			}
			*value = it->second;
			return true;
		}

	};
}
