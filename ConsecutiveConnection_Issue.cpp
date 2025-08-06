#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

void checkRet(SQLRETURN ret, const char* msg) {
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error: " << msg << " (ret=" << ret << ")" << std::endl;
        exit(1);
    }
}

void runMetadataQuery(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    checkRet(ret, "AllocHandle STMT");

    ret = SQLTables(stmt, NULL, 0, NULL, 0, NULL, 0, (SQLWCHAR*)L"TABLE", SQL_NTS);
    checkRet(ret, "SQLTables");

    SQLCHAR tableName[256];
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 3, SQL_C_CHAR, tableName, sizeof(tableName), NULL);
        std::cout << "Table: " << tableName << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

int main() {
    std::cout << "=== ODBC Connection Issue Application ===" << std::endl;
    std::cin.get();

    const wchar_t* connStr = L"DSN=Simba Onelake DSN;";

    SQLHENV env = nullptr;
    SQLHDBC dbc = nullptr;
    SQLRETURN ret;

    // === First Connection ===
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    checkRet(ret, "Alloc ENV");

    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    checkRet(ret, "Set ODBC Version");

    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    checkRet(ret, "Alloc DBC (1)");

    std::cout << "[INFO] Connecting first time..." << std::endl;
    ret = SQLDriverConnect(dbc, NULL, (SQLWCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkRet(ret, "DriverConnect (1)");

    runMetadataQuery(dbc);

    ret = SQLDisconnect(dbc);
    checkRet(ret, "Disconnect (1)");
    SQLFreeHandle(SQL_HANDLE_DBC, dbc); // Free old handle

    // === Second Connection ===
    std::cout << "[INFO] Connecting second time..." << std::endl;
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    checkRet(ret, "Alloc DBC (2)");

    ret = SQLDriverConnect(dbc, NULL, (SQLWCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkRet(ret, "DriverConnect (2)");

    runMetadataQuery(dbc);

    ret = SQLDisconnect(dbc);
    checkRet(ret, "Disconnect (2)");

    std::cout << "[INFO] Final cleanup..." << std::endl;
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    std::cout << "[SUCCESS] Test complete." << std::endl;
    return 0;
}