#pragma once
#define NAME_LEN 21  

#include <sqlext.h>
#include <atlconv.h>

extern SQLHENV henv;
extern SQLHDBC hdbc;
extern SQLHSTMT hstmt;

void show_err();
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
void Init_DB();
void DB_Rel();
bool DB_Login(char* login_id, char* login_pw, LoginInfo& p_info);
bool DB_Check_Id(const char* _id);
bool DB_SignUp(int id);

