#include "pch.h"
#include "GameDataBase.h"


void show_err() {
	cout << "error" << endl;
}

// DB 에러 출력
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	setlocale(LC_ALL, "korean");
	std::wcout.imbue(std::locale("korean"));
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle! n");
		return;
	}

	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			wprintf(L"[%s] : ", wszState);
			wprintf(L"%s", wszMessage);
			wprintf(L" - %d\n", iError);
		}
	}
}

//플레이어 데이터 받아오기
bool DB_odbc(int id, char* name, char* pw)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	//cout <<"ID" << name << endl;
	//cout << "PW" << pw << endl;

	char temp[BUFSIZE] = {};
	sprintf_s(temp, sizeof(temp), "EXEC login_player %s, %s", name, pw);
	wchar_t* exec;
	int str_size = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, NULL);
	exec = new WCHAR[str_size];
	MultiByteToWideChar(CP_ACP, 0, temp, sizeof(temp) + 1, exec, str_size);

	SQLWCHAR p_id[NAME_LEN] = {};
	SQLLEN cbP_ID = 0;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);


		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);


			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);


				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2021_Class_ODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);


				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "ODBC Connection Success" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_id, NAME_LEN, &cbP_ID);

						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_err();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								if (p_id != 0) {
									return true;
								}
								else
								{
									printf(" [DB_odbc] 로그인 실패 \n");
									return false;
								}
							}
							else
								break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else cout << "ODBC Connected Failed" << endl;
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	return false;
}

//플레이어 아이디 확인
bool DB_id(char* _id)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	cout << "[DB_id] ID : " << _id << endl;

	char temp[BUFSIZE] = {};
	sprintf_s(temp, sizeof(temp), "EXEC select_id %s", _id);
	wchar_t* exec;
	int str_size = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, NULL);
	exec = new WCHAR[str_size];
	MultiByteToWideChar(CP_ACP, 0, temp, sizeof(temp) + 1, exec, str_size);

	SQLWCHAR p_id[NAME_LEN] = {};
	SQLLEN cbP_ID = 0;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);


		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);


			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);


				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2021_Class_ODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);


				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "ODBC Connection Success" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_id, NAME_LEN, &cbP_ID);
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								//show_err();
								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									if (p_id != 0) {
										printf("등록된 아이디 \n");
										return true;
									}
									else
									{
										printf("[DB_ID] 실패 \n");
										return false;
									}
								}
								else
									break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else cout << "ODBC Connected Failed" << endl;
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	return false;
}

//플레이어 데이터 저장
bool DB_save(int id)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	char temp[BUFSIZE] = {};
	sprintf_s(temp, sizeof(temp), "EXEC account_creation %s,%s", clients[id]._id, clients[id]._pw);
	wchar_t* exec;
	int str_size = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, NULL);
	exec = new WCHAR[str_size];
	MultiByteToWideChar(CP_ACP, 0, temp, sizeof(temp) + 1, exec, str_size);
	cout << temp << endl;

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2021_Class_ODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "ODBC Connection Success" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)exec, SQL_NTS);
					HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_err();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								return true;
							}
							else
								break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else cout << "ODBC Connected Failed" << endl;
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	return false;
}