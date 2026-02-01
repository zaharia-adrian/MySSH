#include "Database.hpp"

class Statement {
public:
    Statement(sqlite3* db, const std::string& sql) {
        check_ok(SQLITE_OK, sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr),  "[db] Statement preparation failed");
    }

    ~Statement() { 
        if (stmt) sqlite3_finalize(stmt); 
    }

    sqlite3_stmt* get() { return stmt; }
    operator sqlite3_stmt*() { return stmt; }

private:
    sqlite3_stmt* stmt = nullptr;
};

Database::Database() {
    check_ok(SQLITE_OK, sqlite3_open("utils/ssh_users.db", &db), "[db] Could not open database file");
}
void Database::init(){

    sqlite3* db = nullptr;
    check_ok(SQLITE_OK, sqlite3_open("utils/ssh_users.db", &db), "[db] Could not open database file");

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        if (err_msg) {
            std::cerr << "[db] WAL mode error: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
        check_ok(SQLITE_OK, rc, "[db] Failed to set WAL mode"); 
    }
    check_ok(SQLITE_OK, sqlite3_busy_timeout(db, 2000), "[db] Failed to set busy timeout");

    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL);";

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        if (err_msg) {
            std::cerr << "[db] Schema init error: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
        check_ok(SQLITE_OK, rc, "[db] Critical: Could not create tables");
    }

    sqlite3_close(db);
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}


bool Database::create_user(const std::string& username, const std::string& password) {
    std::string hash = hash_password(password);

    Statement stmt(db, "INSERT INTO users (username, password_hash) VALUES (?, ?);");

    check_ok(SQLITE_OK, sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC),  "[db] Bind username failed");
             
    check_ok(SQLITE_OK, sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_STATIC), "[db] Bind hash failed");

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr<<"Could not create the user!\n";
        return false; 
    }
    return true;
}

int Database::authenticate_user(const std::string& username, const std::string& password) {
    Statement stmt(db, "SELECT id, password_hash FROM users WHERE username = ?;");
    check_ok(SQLITE_OK, sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC), "[db] Auth bind failed");

    int code = sqlite3_step(stmt);

    if (code != SQLITE_ROW) {
        return -1; // user not found
    }

    unsigned int user_id = sqlite3_column_int(stmt, 0);
    const unsigned char* db_hash = sqlite3_column_text(stmt, 1);
    check_failure(nullptr, (void*)db_hash, "[db] Integrity error: NULL hash in DB");

    std::string stored_hash = reinterpret_cast<const char*>(db_hash);
    std::string check_hash = hash_password(password);

    if (check_hash == stored_hash) {
        return user_id; // correct password
    }

    return 0; 
}

std::string Database::hash_password(const std::string& password) {
    std::string input = password;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    
    check_failure(nullptr, context, "[crypto] Failed to create OpenSSL context");

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    check(1 == EVP_DigestInit_ex(context, EVP_sha256(), nullptr), "[crypto] DigestInit failed");

    check(1 == EVP_DigestUpdate(context, input.c_str(), input.length()), "[crypto] DigestUpdate failed");
             
    check(1 == EVP_DigestFinal_ex(context, hash, &hash_length), "[crypto] DigestFinal failed");

    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for (unsigned int i = 0; i < hash_length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}