# CodeAlpha_Login_Registration_System
Secure console-based Login &amp; Registration System in Standard C++ (C++11) with salted &amp; iterated hashing (FNV-1a), file handling &amp; input validation | CodeAlpha C++ Internship Task 2
# CodeAlpha_Login_Registration_System

A beginner-friendly, console-based **Login and Registration System** written in standard C++.
Built as part of the **CodeAlpha C++ Programming Internship**.

## Project Overview
Users can register with a username and password, then log in with those credentials.
Records are stored in a local text file (`users.txt`) that is created automatically on the first registration.
No external libraries or databases are used.

## Features
- Menu-driven program that runs until the user chooses **Exit**
- Registration with input validation (no empty or invalid entries)
- Duplicate username detection (case-insensitive)
- Password confirmation during registration
- Passwords stored as **salted, iterated hashes**, not as plain text
- Login with clear success and error messages
- Handles missing files, file-open errors, and invalid menu input
- Compatible with Cxxdroid (Android) and standard C++ compilers (g++, clang++, MSVC)

## Technologies
- C++11 or newer
- Standard Library only: `<iostream>`, `<fstream>`, `<string>`, `<sstream>`, `<random>`, and others

## Validation Rules
| Field    | Rules                                                |
|----------|------------------------------------------------------|
| Username | 3-20 characters; letters, digits, and `_` only       |
| Password | 6-32 characters; no spaces                           |

## How to Compile and Run

**Linux / macOS / Windows (MinGW):**
```bash
g++ -std=c++11 -Wall -Wextra -o login_system main.cpp
./login_system          # On Windows: login_system.exe
```

**Cxxdroid (Android):** open `main.cpp` and press **Run**. Cxxdroid stores `users.txt` in the app's working directory.

## Sample Output
```
===== CodeAlpha Login & Registration System =====
1. Register
2. Login
3. Exit
Enter your choice (1-3): 1

--- Registration ---
Enter username (3-20 chars, letters/numbers/underscore): alice_01
Enter password (6-32 chars, no spaces): Secret123
Confirm password: Secret123
Success: User 'alice_01' registered successfully!

===== CodeAlpha Login & Registration System =====
1. Register
2. Login
3. Exit
Enter your choice (1-3): 2

--- Login ---
Enter username: alice_01
Enter password: Secret123
Login successful! Welcome, alice_01.

===== CodeAlpha Login & Registration System =====
1. Register
2. Login
3. Exit
Enter your choice (1-3): 2

--- Login ---
Enter username: alice_01
Enter password: WrongPass
Error: Incorrect username or password.

===== CodeAlpha Login & Registration System =====
1. Register
2. Login
3. Exit
Enter your choice (1-3): 1

--- Registration ---
Enter username (3-20 chars, letters/numbers/underscore): alice_01
Error: Username 'alice_01' already exists. Please choose another.

===== CodeAlpha Login & Registration System =====
1. Register
2. Login
3. Exit
Enter your choice (1-3): 3

Goodbye! Thank you for using the system.
```

## Security Limitations (Please Read)
This is an **educational project, not production-ready software**.
- Passwords are salted and hashed, but the hash function (FNV-1a) is **not cryptographically secure**. A real application should use Argon2, bcrypt, or scrypt from a well-reviewed library.
- Typed passwords are visible on screen (hiding input is not portable across platforms and Cxxdroid).
- `users.txt` has no encryption or file-permission protection.
- There is no limit on failed login attempts and no account lockout.
- No password reset, user deletion, or password change.
- The file format is space-separated, which is why usernames cannot contain spaces.
- Not safe for multiple programs writing to the file at the same time.

## Notes
`users.txt` is listed in `.gitignore`. **Never commit real credentials.**

## Author
Pixorix Art and Code- CodeAlpha C++ Programming Intern
