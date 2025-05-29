
# PASSPORT_MANAGEMENT_SYSTEM
This is a group project by CS students to implement C++ PASSPORT MANAGEMENT SYSTEM 
## group members ............... id no
1. mujahid muhammednur..........5517/23
2. musa husein .................2093/23
3. misrak firde.................4241/23
4. naf kufa ....................8656/23
5. nardos debisa...............1659/23
6. robel shiferaw...............5124/23
## Group Member
**Member 1**: Implemented created function
**Member 2** :Implemented file save function
**Member 3**: Implemented update function
**Member 4**: Implemented delete and display function
**Member 5**: Implemented search function
**Member 6**: Implemented sort function 

## overview
The Passport Management System is a C++ console-based application that manages the lifecycle of both new and old passport records. It enables users to create, update, delete, search, sort, and display passport information. Data is persisted using CSV files.
The system distinguishes between:
New Passports (Regular or Urgent)
Old/Expired Passports (ExpiredRegular or ExpiredUrgent)
Features
## New passport managment
Create new passports with validation for ID, name, DOB, nationality, and phone number.
Auto-calculated appointment dates:
1 month for Regular
2 days for Urgent
Supports payment confirmation.
## Old Passport Management
Auto-generates sample old passport records.
Requires confirmation of predefined fields (ID, passport number, etc.)
Handles account balance deduction for payment.
## Core Functionalities
Search passport by ID and Name (new or old)
Update passport records with constraints
Delete passport records
Sort records by name or passport type
Persist data using CSV file I/O
File Structure
std.cpp - Main source file containing all logic
regular4.csv - Data file for new regular passports
urgent4.csv - Data file for new urgent passports
expired_regular4.csv - Data file for expired regular passports
expired_urgent4.csv - Data file for expired urgent passports
Build Instructions
🛠 Prerequisites
C++11 or higher
g++, clang++, or compatible compiler
Sample Menu
## Passport Management System ---
1. Create Passport
2. Update Passport
3. Delete Passport
4. Search Passport
5. Display Passports
6. Sort Passports
0. Exit
## Data Validation Rules
ID: Alphanumeric, max 10 chars, unique across all records
Name: Letters only, max 25 chars
DOB: Must be in YYYY-MM-DD format and applicant must be over 18
Phone: Numeric only, max 12 chars
Nationality: Letters only, max 15 chars
## Data Persistence
Uses file I/O to store and load data from .csv files
Records are automatically saved after create, update, and delete operations
Memory Management
All passport records are stored in singly linked lists
Dynamic memory is properly freed before program exits
## Conclusion
This project demonstrates collaborative development using Git, structured C++ programming, and practical data handling through linked lists and file storage. Every team member contributes to specific components to ensure modularity and maintainability.

