#include "CommandList.h"
#include <cstdio>

// �ڴ�ע������
void(*_commandFunctionList[])(char*, void*)
= {
	notFindException,
	newBlock,
	showBuffer,
	showBlock,
	removeRecord,
	newFile,
	showFile,
	removeBlock,
	createTable,
	showTableStructure,
	createRecord,
	createDatabase,
	removeDatabase,
	showDatabase,
	enterDatabase,
	leaveDatabase,
	removeTable,
	showAllTable,
	insertRecord
};

char * _commandNameList[]
= {
	"",
	"newblock",
	"showbuffer",
	"showblock",
	"removerecord",
	"newfile",
	"showfile",
	"removeblock",
	"createtable",
	"showtable",
	"createrecord",
	"createdb",
	"removedb",
	"showdb",
	"using",
	"leavedb",
	"removetable",
	"showalltable",
	"insert"
};

int _commandList_length = 19;


void notFindException(char *cmd, void *par)
{
	printf("%s is not a correct command.\n", cmd);
}

void newBlock(char *cmd, void *par)
{
	File *file = new File((char*)par);
	Block *temp = file->addNewBlock();
	file->writeToFile(temp);
	delete file;
	//buffer.addBlock(new Block(atoi((char *)par)));
}

void showBuffer(char *cmd, void *par)
{

}

void showBlock(char *cmd, void *par)
{

}

void removeRecord(char *cmd, void *par)
{
	char *str = (char*)par;
	char name[100];
	int bno, rindex;
	sscanf(str, "%s %d %d", name, &bno, &rindex);
	File *file = new File(name);
	Block *block = file->readBlock(bno);
	block->removeRecord(rindex);
	file->writeToFile(block);
	delete file;
}

void newFile(char *cmd, void *par)
{
	File::initFile((char*)par);
}

void showFile(char *cmd, void *par)
{
	File *file = new File((char*)par);
	file->showFile();
	delete file;
}

void removeBlock(char *cmd, void *par)
{
	char *str = (char*)par;
	char name[100];
	int bno, front;
	sscanf(str, "%s %d %d", name, &bno, &front);
	File *file = new File(name);
	file->removeBlock(file->readBlock(bno), front);
	delete file;
}

void createTable(char *cmd, void *par)
{
	if (!curDatabase.length())	throw("please select a database first!");
	char table[100];
	int attributeNumber;
	std::vector<Column> tableStructure;
	sscanf((char *)par, "%s %d", table, &attributeNumber);
	if (!databaseCategory.addTable(curDatabase, table))
		throw("table already exist!");
	for (int i = 0; i < attributeNumber; i++)
	{
		char tname[64];
		int length;
		char name[61];
		int sn = scanf("%s", &tname);
		ushort typeNum = Column::parseType(tname[0], length);
		if (!typeNum) {
			databaseCategory.removeTable(curDatabase, table);
			throw("Error Type!");
		}
		if (!~length) {
			scanf("%d", &length);
		}
		scanf("%s", name);
		getchar();
		Column c(typeNum, length, name);
		tableStructure.push_back(c);
	}
	std::string *filename = CategoryMgr::makePath(curDatabase.c_str(), table, "schema");
	Table::initTable(filename->c_str(), tableStructure);
	delete filename;
	filename = CategoryMgr::makePath(curDatabase.c_str(), table, "data");
	File::initFile(filename->c_str());
	delete filename;

}

void showTableStructure(char *cmd, void *par)
{
	if (!curDatabase.length())	throw("please select a database first!");
	char table[100];
	std::vector<Column> tableStructure;
	sscanf((char*)par, "%s", table);
	if (!databaseCategory.existTable(curDatabase, table))
		throw("database not exist!");
	std::string *filename = CategoryMgr::makePath(curDatabase.c_str(), table, "schema");
	Table t(filename->c_str());
	t.showTableStructure();
}

void createRecord(char *cmd, void *par)
{
	char table[100];
	int id;
	int age;
	char name[100];
	sscanf((char*)par, "%d %d %s", &id, &age, name);
	Table t("stu");
	std::vector<byte*> originalData;
	originalData.push_back((byte*)&id);
	originalData.push_back((byte*)&age);
	originalData.push_back((byte*)&name);
	byte* record;
	byte* b = originalData[0];
	int a = *(int*)(b);
	record = t.formRecord(originalData);
	id = *(int*)t.parseRecord(record, 2);
	char* s = (char*)t.parseRecord(record, 3);
	printf("%d\n", id);
	printf("%s\n", s);
}

void createDatabase(char *cmd, void *par)
{
	if (curDatabase.length())
		throw("Please leave database first");
	if (databaseCategory.addDataBase((char*)par)) {
		printf("create database success!\n");
	}
	else {
		throw("Failed! already exist.");
	}
}

void removeDatabase(char *cmd, void *par)
{
	if (curDatabase.length())
		throw("Please leave database first");
	if (databaseCategory.removeDataBase((char*)par)) {
		printf("remove database success!\n");
	}
	else {
		throw("Failed! not exist.");
	}
}

void showDatabase(char*cmd, void *par)
{
	databaseCategory.showDatabase();
}

void enterDatabase(char *cmd, void *par)
{
	if (!databaseCategory.existDataBase((char*)par))
		throw("database not exist.");
	curDatabase = (char*)par;
}

void leaveDatabase(char *cmd, void *par)
{
	databaseBuffer.clear();
	curDatabase.clear();
}

void removeTable(char *cmd, void *par)
{
	if (!curDatabase.length())
		throw("Please select database first");
	if (databaseCategory.removeTable(curDatabase, (char*)par)) {
		printf("delete table success!\n");
	}
	else {
		throw("table not exist.");
	}
}

void showAllTable(char *cmd, void *par)
{
	if (!curDatabase.length())
		throw("Please select database first");
	databaseCategory.showTable(curDatabase);
}

void insertRecord(char *cmd, void *par)
{
	char *tablename = (char*)par;
	if (!curDatabase.length())
		throw("Please select database first");
	std::string *schemaName = CategoryMgr::makePath(curDatabase.c_str(), tablename, "schema");
	std::string *dataName = CategoryMgr::makePath(curDatabase.c_str(), tablename, "data");
	Table table(schemaName->c_str());
	std::vector<byte*> originalData;
	for (int i = 0; i < table.getColumnSize(); i++) {
		byte* data = table.getColumnData(i);
		originalData.push_back(data);
	}
	byte* record = table.formRecord(originalData);
	//TODO
}