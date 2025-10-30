#ifdef _WIN32
#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <cstring>

// Define platform-specific string types and functions
#ifdef _WIN32
#define CONN_STR_TYPE SQLWCHAR
#define CONN_STR(str) L##str
#define SQL_DRIVER_CONNECT_FUNC SQLDriverConnectW
#define SQL_TABLES_FUNC SQLTablesW
#else
#define CONN_STR_TYPE SQLCHAR
#define CONN_STR(str) (SQLCHAR*)str
#define SQL_DRIVER_CONNECT_FUNC SQLDriverConnect
#define SQL_TABLES_FUNC SQLTables
#endif

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

    ret = SQL_TABLES_FUNC(stmt, NULL, 0, NULL, 0, NULL, 0, CONN_STR("TABLE"), SQL_NTS);
    checkRet(ret, "SQLTables");

    SQLCHAR tableName[256];
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 3, SQL_C_CHAR, tableName, sizeof(tableName), NULL);
        std::cout << "Table: " << tableName << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

// Simulate a �full disconnect�
void fullDisconnect(SQLHENV& env, SQLHDBC& dbc) {
    if (dbc) {
        SQLRETURN ret = SQLDisconnect(dbc);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
            std::cerr << "[WARN] SQLDisconnect failed." << std::endl;

        ret = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
            std::cerr << "[WARN] SQLFreeHandle DBC failed." << std::endl;

        dbc = nullptr;
    }

    if (env) {
        SQLRETURN ret = SQLFreeHandle(SQL_HANDLE_ENV, env);
        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
            std::cerr << "[WARN] SQLFreeHandle ENV failed." << std::endl;

        env = nullptr;
    }
}

int main() {
    std::cout << "=== ODBC Full Disconnect Test ===" << std::endl;
    std::cin.get();

    CONN_STR_TYPE* connStr = CONN_STR("DSN=Simba Onelake DSN;");

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
    ret = SQL_DRIVER_CONNECT_FUNC(dbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkRet(ret, "DriverConnect (1)");

    runMetadataQuery(dbc);

    std::cout << "[INFO] Performing full disconnect..." << std::endl;
    fullDisconnect(env, dbc);

    // === Second Connection ===
    std::cout << "[INFO] Connecting second time (new handles)..." << std::endl;
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    checkRet(ret, "Alloc ENV (2)");

    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    checkRet(ret, "Set ODBC Version (2)");

    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    checkRet(ret, "Alloc DBC (2)");

    ret = SQL_DRIVER_CONNECT_FUNC(dbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkRet(ret, "DriverConnect (2)");

    runMetadataQuery(dbc);

    std::cout << "[INFO] Final cleanup..." << std::endl;
    fullDisconnect(env, dbc);

    std::cout << "[SUCCESS] Test complete." << std::endl;
    return 0;
}
