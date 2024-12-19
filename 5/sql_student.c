#include "sqlite3.h"
#include<stdio.h>
#include<string.h>
/**
*通过表 student 和表 course 查询选择课程 1 的学生学号，姓名，课程 1 成绩
*并打印查询结果
*/
void sqlJoinSearch (sqlite3 *db)
{
char *errmsg ;
//通过表 student 和表 course 查询选择课程 1 的学生学号，姓名，课程 1 成绩
char *strSql = “//补充 sqlite 代码”;
char** pResult;
int nRow;
int nCol;
//调用 sqlite3_get_table 函数，得到查询结果
int nResult = ____________________;
if (nResult != SQLITE_OK)
{
sqlite3_close(db);
sqlite3_free(errmsg);
}
char* strOut;
int nIndex = nCol;
for(int i=0;i<nRow;i++)
{
printf("id: %s---->name: %s---->score: %s\n", pResult[nIndex],
pResult[nIndex+1],pResult[nIndex+2]);
nIndex = nIndex + 3;
}
_________________ //释放为记录分配的内存，否则会内存泄漏
}
//回调函数
static int dbInfo (void *para, int column, char **column_value, char
**column_name)
{
//补充代码；
}
/**
*创建视图 student_view，视图中每条记录包含的元素为选择课程 2 的学生的学号、姓名和成绩
*并利用回调函数打印查询结果*/
void sqlCreateView(sqlite3 *db){
//补充代码
}
int main(int argc, char *argv[])
{
sqlite3 *db;
int rv;
//创建数据库 StuInfo.db
rv = __________________
if(rv){
sqlite3_close(db);
return (-1);
}
//创建 student 表，course 表和 student_course 表
//补充代码
/**
*分别向 student 表 course 表和 student_course 表中插入记录
*/
/**
*补充代码
*/
sqlJoinSearch (db);
sqlCreateView(db);
return (0);
}