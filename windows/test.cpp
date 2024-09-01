#include "FileTool.hpp"
#include "BackDateManage.hpp"
#include "CloudBackup.hpp"

#define DATE_FILE "./backup.dat"
#define BACKUP_DIR "./cloud_backup/"

using namespace cloudb;
using std::vector;

int main()
{
	CloudBackup backup(BACKUP_DIR, DATE_FILE);
	backup.runModule();

	return 0;
}