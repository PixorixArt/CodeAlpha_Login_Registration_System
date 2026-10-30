// ============================================================================
// Project : CodeAlpha_Login_Registration_System
// Purpose : Console-based Login and Registration System (CodeAlpha C++ Internship)
// Language: Standard C++ (C++11 or newer), no external libraries
//
// SECURITY NOTE (educational project):
//   Passwords are NOT stored in plain text. Each password is combined with a
//   random salt and run through a simple iterated hash (FNV-1a). This is only
//   a teaching example. FNV-1a is NOT a cryptographic hash, so this program is
//   NOT secure enough for real applications. Real systems should use a vetted
//   algorithm such as Argon2, bcrypt, or scrypt from a well-known library.
// ============================================================================

#include <cctype>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

// ---------------------------- Constants -------------------------------------
const string USER_FILE = "users.txt";   // Local file that stores user records
const size_t MIN_USERNAME_LEN = 3;
const size_t MAX_USERNAME_LEN = 20;
const size_t MIN_PASSWORD_LEN = 6;
const size_t MAX_PASSWORD_LEN = 32;
const int HASH_ROUNDS = 10000;          // Number of hashing iterations

// One stored user: username, random salt, and password hash
struct UserRecord {
    string username;
    string salt;
    string hash;
};

// ------------------------- Small helper functions ---------------------------

// Remove leading and trailing whitespace (including '\r' from Windows files)
string trim(const string& text) {
    size_t start = 0;
    while (start < text.size() && isspace(static_cast<unsigned char>(text[start]))) {
        start++;
    }
    size_t end = text.size();
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        end--;
    }
    return text.substr(start, end - start);
}

// Convert a string to lowercase (used for case-insensitive username checks)
string toLower(string text) {
    for (size_t i = 0; i < text.size(); i++) {
        text[i] = static_cast<char>(tolower(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// Convert a 64-bit number to a 16-character hexadecimal string
string toHex(uint64_t value) {
    ostringstream out;
    out << hex << setfill('0') << setw(16) << value;
    return out.str();
}

// Print a prompt and read one full line. Returns false if input has ended.
bool readLine(const string& prompt, string& result) {
    cout << prompt;
    if (!getline(cin, result)) {
        return false;   // End of input (e.g., Ctrl+D / Ctrl+Z)
    }
    return true;
}

// ----------------------------- Validation -----------------------------------

// Username: 3-20 characters; only letters, digits, and underscore
bool isValidUsername(const string& username, string& errorMessage) {
    if (username.empty()) {
        errorMessage = "Username cannot be empty.";
        return false;
    }
    if (username.size() < MIN_USERNAME_LEN || username.size() > MAX_USERNAME_LEN) {
        errorMessage = "Username must be 3 to 20 characters long.";
        return false;
    }
    for (size_t i = 0; i < username.size(); i++) {
        unsigned char c = static_cast<unsigned char>(username[i]);
        if (!isalnum(c) && c != '_') {
            errorMessage = "Username may only contain letters, numbers, and underscores.";
            return false;
        }
    }
    return true;
}

// Password: 6-32 characters; no spaces
bool isValidPassword(const string& password, string& errorMessage) {
    if (password.empty()) {
        errorMessage = "Password cannot be empty.";
        return false;
    }
    if (password.size() < MIN_PASSWORD_LEN || password.size() > MAX_PASSWORD_LEN) {
        errorMessage = "Password must be 6 to 32 characters long.";
        return false;
    }
    for (size_t i = 0; i < password.size(); i++) {
        if (isspace(static_cast<unsigned char>(password[i]))) {
            errorMessage = "Password must not contain spaces.";
            return false;
        }
    }
    return true;
}

// ------------------------ Salting and hashing -------------------------------

// Generate a random 16-character hexadecimal salt
string generateSalt() {
    random_device rd;
    mt19937_64 generator(static_cast<uint64_t>(rd()) ^
                         (static_cast<uint64_t>(time(nullptr)) << 16));
    return toHex(generator());
}

// 64-bit FNV-1a hash of a string
uint64_t fnv1a(const string& data) {
    uint64_t hash = 14695981039346656037ULL;   // FNV offset basis
    for (size_t i = 0; i < data.size(); i++) {
        hash ^= static_cast<unsigned char>(data[i]);
        hash *= 1099511628211ULL;              // FNV prime
    }
    return hash;
}

// Hash "salt + password" many times (educational only, NOT cryptographically secure)
string hashPassword(const string& password, const string& salt) {
    string base = salt + password;
    uint64_t current = fnv1a(base);
    for (int i = 0; i < HASH_ROUNDS; i++) {
        current = fnv1a(toHex(current) + base);
    }
    return toHex(current);
}

// --------------------------- File operations --------------------------------

// Check whether the user file exists and can be opened
bool userFileExists() {
    ifstream file(USER_FILE.c_str());
    return file.is_open();
}

// Look for a user (case-insensitive). Returns true and fills 'found' if present.
// A missing file simply means no users have registered yet.
bool findUser(const string& username, UserRecord& found) {
    ifstream file(USER_FILE.c_str());
    if (!file.is_open()) {
        return false;
    }

    string line;
    string target = toLower(username);
    while (getline(file, line)) {
        istringstream lineStream(line);
        UserRecord record;
        // Skip blank or damaged lines
        if (!(lineStream >> record.username >> record.salt >> record.hash)) {
            continue;
        }
        if (toLower(record.username) == target) {
            found = record;
            return true;
        }
    }
    return false;
}

// Append a new user record to the file. Returns false on any file error.
bool saveUser(const UserRecord& record) {
    ofstream file(USER_FILE.c_str(), ios::app);
    if (!file.is_open()) {
        return false;
    }
    file << record.username << ' ' << record.salt << ' ' << record.hash << '\n';
    file.flush();
    return file.good();
}

// ---------------------------- Main features ---------------------------------

// Register a new user
void registerUser() {
    cout << "\n--- Registration ---\n";

    string username, password, confirm, error;

    if (!readLine("Enter username (3-20 chars, letters/numbers/underscore): ", username)) return;
    username = trim(username);
    if (!isValidUsername(username, error)) {
        cout << "Error: " << error << "\n";
        return;
    }

    // Check for a duplicate BEFORE asking for a password
    UserRecord existing;
    if (findUser(username, existing)) {
        cout << "Error: Username '" << username << "' already exists. Please choose another.\n";
        return;
    }

    if (!readLine("Enter password (6-32 chars, no spaces): ", password)) return;
    if (!isValidPassword(password, error)) {
        cout << "Error: " << error << "\n";
        return;
    }

    if (!readLine("Confirm password: ", confirm)) return;
    if (password != confirm) {
        cout << "Error: Passwords do not match. Registration cancelled.\n";
        return;
    }

    // Build the record with a fresh salt and the hashed password
    UserRecord record;
    record.username = username;
    record.salt = generateSalt();
    record.hash = hashPassword(password, record.salt);

    if (saveUser(record)) {
        cout << "Success: User '" << username << "' registered successfully!\n";
    } else {
        cout << "Error: Could not write to '" << USER_FILE
             << "'. Check folder permissions and try again.\n";
    }
}

// Log in an existing user
void loginUser() {
    cout << "\n--- Login ---\n";

    if (!userFileExists()) {
        cout << "Error: No users are registered yet. Please register first.\n";
        return;
    }

    string username, password;

    if (!readLine("Enter username: ", username)) return;
    username = trim(username);
    if (username.empty()) {
        cout << "Error: Username cannot be empty.\n";
        return;
    }

    if (!readLine("Enter password: ", password)) return;
    if (password.empty()) {
        cout << "Error: Password cannot be empty.\n";
        return;
    }

    UserRecord record;
    // The same message is used for "unknown user" and "wrong password" so the
    // program does not reveal which usernames exist.
    if (!findUser(username, record) ||
        hashPassword(password, record.salt) != record.hash) {
        cout << "Error: Incorrect username or password.\n";
        return;
    }

    cout << "Login successful! Welcome, " << record.username << ".\n";
}

// Show the menu and return the choice (1-3), or -1 if the input is invalid.
// Returns 0 if input has ended.
int showMainMenu() {
    cout << "\n===== CodeAlpha Login & Registration System =====\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. Exit\n";

    string input;
    if (!readLine("Enter your choice (1-3): ", input)) {
        return 0;
    }
    input = trim(input);
    if (input == "1") return 1;
    if (input == "2") return 2;
    if (input == "3") return 3;
    return -1;
}

// ------------------------------- main ---------------------------------------
int main() {
    bool running = true;

    while (running) {
        int choice = showMainMenu();

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                cout << "\nGoodbye! Thank you for using the system.\n";
                running = false;
                break;
            case 0:   // Input stream closed; exit cleanly
                cout << "\nInput ended. Exiting program.\n";
                running = false;
                break;
            default:
                cout << "Error: Invalid choice. Please enter 1, 2, or 3.\n";
                break;
        }
    }
    return 0;
}
