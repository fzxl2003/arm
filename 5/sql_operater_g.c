/*
* sql_operater.c
*
* Created on: 2019-11-28
* Author: user
*/
#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

// 向表中插入数据
void sqlInsert (sqlite3 *db)
{
    char *errmsg;
    sqlite3_exec(db, "insert into d_table(name) values ('demo1')", 0, 0, &errmsg);
    sqlite3_exec(db, "insert into d_table(name) values ('demo2')", 0, 0, &errmsg);
    sqlite3_exec(db, "insert into d_table(name) values ('demo3')", 0, 0, &errmsg);
}

// 删除数据
void sqlDelete (sqlite3 *db)
{
    char *errmsg;
    // 删除表中名字为 'demo1' 的记录
    sqlite3_exec(db, "delete from d_table where name = 'demo1'", 0, 0, &errmsg);
}

// 删除数据1
void sqlDelete1 (sqlite3 *db)
{
    char *errmsg;
    // 删除 name 为 demo2 的记录
    sqlite3_exec(db, "delete from d_table where name = 'demo2'", 0, 0, &errmsg);
}


// 回调函数
static int dbInfo (void *para, int column, char **column_value, char **column_name)
{
    int i;
    printf("column: %d\n", column);
    for (i = 0; i < column; i++)
        printf("word: %s value: %s\n", column_name[i], column_value[i]);
    printf("------------\n");
    return 0;
}

void sqlSelect (sqlite3 *db)
{
    char *errmsg;
    // 查询 d_table 所有数据，并利用回调函数显示
    sqlite3_exec(db, "select * from d_table", dbInfo, 0, &errmsg);
}

// 创建表
void sqlCreate (sqlite3 *db)
{
    char *errmsg;
    sqlite3_exec(db, "create table d_table(ID integer primary key autoincrement, name nvarchar(32))", NULL, NULL, &errmsg);
}

/***************************************************************************
***
** 函数名称: main
** 功能描述: main 函数
****************************************************************************
**/
int main (int argc, char *argv[])
{
    sqlite3 *db;
    int rv;
    rv = sqlite3_open("demo.db", &db);
    if (rv) {
        sqlite3_close(db);
        return (-1);
    }
    sqlCreate(db);   // 创建表
    printf("Create table d_table success!\n");
    sqlInsert(db);   // 插入数据
    printf("Insert data success!\n");
    sqlDelete(db);   // 删除数据
    printf("Delete data success!\n");
    printf("Select data:\n");
    sqlSelect(db);   // 查询并显示数据
    sqlDelete1(db);  // 删除数据1
    printf("Delete data1 success!\n");
    printf("Select data:\n");
    sqlSelect(db);   // 查询并显示数据
    sqlite3_close(db);  // 关闭数据库
    printf("Close database success!\n");
    return (0);
}
