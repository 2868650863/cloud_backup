#include "fileTool.hpp"
#include "config.hpp"
#include "backupInfo.hpp"
#include "hotFileManage.hpp"
#include "server.hpp"
#include <thread>

cloudb::BackManage *_date = new cloudb::BackManage();

void testfile()
{
    // cloudb::FileTool ft("bundle.h");
    //  string s;
    //  ft.getContentPosLen(&s,0,3);
    //  cout<<s<<endl;
    //  s.clear();
    //  ft.getContent(&s);
    //  cout<<s<<endl;
    //     string s("abcdeee");
    //      ft.setContent("\n");
    //      ft.setContent(s);

    //     string body;

    //     ft.getContent(&body);

    //    cout<<body<<endl;

    //     string name="hello.lz";
    //    // ft.pack(name);
    //     cloudb::FileTool ftt(name);
    //     ftt.unpack(name+"txt");

    // cloudb::FileTool ft("t1");
    // if (ft.exists() == false)
    // {
    //     cout << "目录不存在,创建中..." << endl;
    // }
    // ft.createDirectory();
    // sleep(1);
    // std::vector<string> vs;
    // ft.scanDirectory(&vs);
    // if(!vs.empty())
    // {
    //     for (auto &it : vs)
    //     {
    //         cout << it << endl;
    //     }
    // }

    cloudb::Config *cf = cloudb::Config::getInstance();
    cout << cf->getHotTime() << endl;
    cout << cf->getServerPort() << endl;
    cout << cf->getServerIp() << endl;
    cout << cf->getDownloadPrefix() << endl;
    cout << cf->getPackfileSuffix() << endl;
    cout << cf->getPackDir() << endl;
    cout << cf->getBackDir() << endl;
    cout << cf->getBackupFile() << endl;

    return;
}

void testfile1(const string &name)
{

    // cloudb::BackupInfo bi(name);

    // bi.printbm();
    // cloudb::BackManage bm;
    // bi._pack_flag=1;
    // bm.insert(bi);
    // cloudb::BackupInfo b1;
    // bm.getInfoBySavaPath(name,&b1);
    // b1.printbm();
    // cloudb::BackupInfo b2;
    // bm.getInfoByUrl("./download/filetest.cc",&b2);
    // b2.printbm();
    // std::vector<cloudb::BackupInfo> vb;
    // bm.getInfoAll(&vb);
    // for(auto it:vb)
    // {
    //    it.printbm();
    // }
    // bm.savefile();
    // cout<<"111"<<endl;

    cloudb::BackupInfo bi(name);
    cloudb::BackManage bm;
    bm.insert(bi);
    bm.savefile();
    std::vector<cloudb::BackupInfo> vb;
    bm.getInfoAll(&vb);

    for (auto it : vb)
    {
        it.printbm();
    }
    // bm.savefile();
}


//cloudb::BackManage *_date;
void hot_file_run()
{
    cloudb::HotFileManage hot;
    hot.runHotModule();
}
void server_run()
{
    cloudb::Server ser;
    ser.runModule();

    
   
    
}

int main(int argc, char *argv[])
{
    // hot_file_run();
    // server_run();
    std::thread thread_hot(hot_file_run);
    std::thread thread_server(server_run);
    thread_hot.join();
    thread_server.join();
    return 0;
}