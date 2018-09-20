//OperationDB.h

#pragma once

#include "..\..\..\third\sqlite\sqlite3.h"
#include "../../common/varname.h"

typedef int (*scSQLExeCallBack)(void *data, int n_columns, char **col_values, char **col_names);

class COperationDB
{
public:
	COperationDB();

	~COperationDB();

	bool Init(LPSTR pszDBName, LPSTR pszError);

	bool UnInit();

	int Write(LPSTR pszSQL, LPSTR pszError);

	int Read(LPSTR pszSQL, scSQLExeCallBack CallBackFunc, LPVOID pUserData, LPSTR pszError);

protected:

	sqlite3 *m_sqlConnect;

private:



};