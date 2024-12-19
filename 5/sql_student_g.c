#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

/**
* 通过表 student 和表 course 查询选择课程 1 的学生学号，姓名，课程 1 成绩
* 并打印查询结果
*/
void sqlJoinSearch (sqlite3 *db)
{
    char *errmsg;
    // 通过表 student 和表 course 查询选择课程 1 的学生学号，姓名，课程 1 成绩
    char *strSql = "SELECT student.student_id, student.student_name, student_course.score FROM student "
                   "JOIN student_course ON student.student_id = student_course.student_id "
                   "WHERE student_course.course_id = 1;";
    
    char** pResult;
    int nRow, nCol;
    // 调用 sqlite3_get_table 函数，得到查询结果
    int nResult = sqlite3_get_table(db, strSql, &pResult, &nRow, &nCol, &errmsg);
    if (nResult != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return;
    }

    // 输出查询结果
    int nIndex = nCol;
    for (int i = 0; i < nRow; i++)
    {
        printf("id: %s ----> name: %s ----> score: %s\n", pResult[nIndex], pResult[nIndex+1], pResult[nIndex+2]);
        nIndex = nIndex + 3;
    }

    // 释放内存
    sqlite3_free_table(pResult);
}

// 回调函数
static int dbInfo (void *para, int column, char **column_value, char **column_name)
{
    printf("----- Query Result -----\n");
    for (int i = 0; i < column; i++) 
    {
        printf("%s: %s\n", column_name[i], column_value[i] ? column_value[i] : "NULL");
    }
    printf("------------------------\n");
    return 0;
}

/**
* 创建视图 student_view，视图中每条记录包含的元素为选择课程 2 的学生的学号、姓名和成绩
* 并利用回调函数打印查询结果
*/
void sqlCreateView(sqlite3 *db)
{
    char *errmsg;
    // 创建视图 student_view
    char *strSql = "CREATE VIEW student_view AS "
                   "SELECT student.student_id, student.student_name, student_course.score FROM student "
                   "JOIN student_course ON student.student_id = student_course.student_id "
                   "WHERE student_course.course_id = 2;";
    
    // 执行 SQL 创建视图
    int nResult = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (nResult != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return;
    }

    // 查询并打印视图内容
    printf("View student_view created successfully. Querying results...\n");
    strSql = "SELECT * FROM student_view;";
    sqlite3_exec(db, strSql, dbInfo, 0, &errmsg);
}

/**
* 主函数：创建数据库 StuInfo.db，创建表格并插入数据
*/
int main(int argc, char *argv[])
{
    sqlite3 *db;
    int rv;

    // 创建数据库 StuInfo.db
    rv = sqlite3_open("StuInfo.db", &db);
    if (rv)
    {
        sqlite3_close(db);
        return (-1);
    }

    // 创建 student 表，course 表和 student_course 表
    char *errmsg;
    char *strSql;

    // 创建 student 表
    strSql = "CREATE TABLE IF NOT EXISTS student (student_id INTEGER PRIMARY KEY, student_name TEXT, student_sex TEXT, student_age INTEGER, dept_id TEXT, class_id TEXT);";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 创建 course 表
    strSql = "CREATE TABLE IF NOT EXISTS course (course_id INTEGER PRIMARY KEY, course_name TEXT, course_hours INTEGER, course_credit INTEGER);";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 创建 student_course 表
    strSql = "CREATE TABLE IF NOT EXISTS student_course (student_id INTEGER, course_id INTEGER, score REAL, PRIMARY KEY (student_id, course_id));";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 向 student 表插入记录
    strSql = "INSERT INTO student (student_id, student_name, student_sex, student_age, dept_id, class_id) VALUES "
             "(112001, 'zhaoyi', 'man', 19, '03', '0301'), "
             "(112002, 'zhaoer', 'man', 21, '03', '0301'), "
             "(112003, 'zhaosan', 'man', 18, '01', '0101'), "
             "(112004, 'zhaosi', 'woman', 20, '01', '0102'), "
             "(112005, 'zhaowu', 'man', 19, '02', '0201'), "
             "(112006, 'zhaoliu', 'woman', 18, '04', '0401');";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 向 course 表插入记录
    strSql = "INSERT INTO course (course_id, course_name, course_hours, course_credit) VALUES "
             "(1, 'math', 64, 3), "
             "(2, 'English', 64, 2);";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 向 student_course 表插入记录
    strSql = "INSERT INTO student_course (student_id, course_id, score) VALUES "
             "(112001, 1, 80), (112001, 2, 90), "
             "(112002, 1, 70), "
             "(112003, 2, 65), "
             "(112004, 1, 73.5);";
    rv = sqlite3_exec(db, strSql, 0, 0, &errmsg);
    if (rv != SQLITE_OK)
    {
        printf("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return (-1);
    }

    // 执行查询操作
    sqlJoinSearch(db);

    // 创建视图并查询
    sqlCreateView(db);

    // 关闭数据库
    sqlite3_close(db);

    return (0);
}
