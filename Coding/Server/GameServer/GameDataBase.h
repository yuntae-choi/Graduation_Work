#pragma once

#define NAME_LEN 20  

extern void show_err();
extern void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
extern bool DB_odbc(int id, char* name, char* pw);
extern bool DB_id(char* _id);
extern bool DB_save(int id);