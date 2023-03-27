#pragma once
#define NAME_LEN 21  

#include <sqlext.h>
#include <atlconv.h>

extern void show_err();
extern void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
extern void Init_DB();
extern void DB_Rel();
extern bool DB_Login(const char* login_id, const char* login_pw, LoginInfo& p_info);
extern bool DB_Check_Id(const char* _id);
extern bool DB_SignUp(int id);

