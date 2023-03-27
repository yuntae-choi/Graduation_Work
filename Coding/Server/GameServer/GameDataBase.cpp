#include "pch.h"
#include "GameDataBase.h"

SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt;

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

//DB 초기화
void Init_DB()
{
	SQLRETURN retcode;
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
				}
				else{
					cout << "ODBC Connected Failed" << endl;
					HandleDiagnosticRecord(hstmt, SQL_HANDLE_DBC, retcode);
				}
			}
		}
	}
}

//DB 해제
void DB_Rel()
{
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}


//Login
bool DB_Login(const char* login_id, const char* login_pw, LoginInfo& p_info)
{
	SQLRETURN retcode{};
	SQLINTEGER p_win{}, p_lose{}; //플레이어 승리 카운트, 패배 카운트
	SQLSMALLINT p_color{}, p_grade{};//플레이어 컬러, 등급
	SQLLEN L_win = 0, L_lose = 0, L_color, L_grade = 0;
	
	wstring LoginQuery{ L"EXEC login_player " };
	USES_CONVERSION;
	LoginQuery += A2W(login_id);
	LoginQuery += L",";
	LoginQuery += A2W(login_pw);
   

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)LoginQuery.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{
		retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &p_win, 100, &L_win);
		retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &p_lose, 100, &L_lose);
		retcode = SQLBindCol(hstmt, 1, SQL_C_SHORT, &p_color, 100, &L_color);
		retcode = SQLBindCol(hstmt, 1, SQL_C_SHORT, &p_grade, 100, &L_grade);

		retcode = SQLFetch(hstmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			p_info.p_win = p_win;
			p_info.p_lose = p_lose;
			p_info.p_color = p_color;
			p_info.p_grade = p_grade;
			SQLCancel(hstmt);
			show_err();
			return true;
		}
	}
	else
	{
		cout << "[DB_odbc] 로그인 실패" << login_id;
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_DBC, retcode);
		SQLCancel(hstmt);
		return false;
	}

}

//플레이어 아이디 확인
bool DB_Check_Id(const char* login_id)
{
	SQLRETURN retcode;
	SQLWCHAR p_name[NAME_LEN]{};
	SQLLEN L_id = 0;

	wstring CheckIDQuery{ L"EXEC check_id " };
	USES_CONVERSION;
	CheckIDQuery += A2W(login_id);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)CheckIDQuery.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, p_name, NAME_LEN, &L_id);
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			show_err();
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			if (p_name != 0) {
				printf(" [DB_ID] 아이디 존재\n");
				SQLCancel(hstmt);
				return true;
			}
			else
			{
				printf(" [DB_ID] 실패 \n");
				SQLCancel(hstmt);
				return false;
			}
		}
	}
	SQLCancel(hstmt);
	return false;
}

//플레이어 계정생성
bool DB_SignUp(int id)
{

	SQLRETURN retcode;
	SQLWCHAR p_name[NAME_LEN]{}, p_pw[NAME_LEN]{};
	SQLLEN L_id = 0, L_pw;

	wstring SignUpQuery{ L"EXEC account_creation " };
	USES_CONVERSION;
	SignUpQuery += A2W(clients[id]._id);
	SignUpQuery += L",";
	SignUpQuery += A2W(clients[id]._pw);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)SignUpQuery.c_str(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{

		retcode = SQLFetch(hstmt);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLCancel(hstmt);
			return true;
		}
		else if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLCancel(hstmt);
			return false;
		}
	}
	
	cout << "[DB_odbc] 로그인 실패" << clients[id]._id;
	HandleDiagnosticRecord(hstmt, SQL_HANDLE_DBC, retcode);
	SQLCancel(hstmt);
	return false;
}