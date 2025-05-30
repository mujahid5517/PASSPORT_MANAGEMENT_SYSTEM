#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <regex>
#include <ctime>
#include <limits> // For numeric_limits

using namespace std;

struct NewPassport {
    string passType,id,name,dob,nationality,phoneNumber,createdDate,appointmentDate,payment,paymentStatus;
    NewPassport* next;     // Pointer to the next node in the list
};
struct OldPassport {
    string passType,id,name,dob,issueDate,expiredDate,passportNumber,accountNumber,createdDate,appointmentDate,payment,paymentStatus;
    double balance;
    OldPassport* next;      // Pointer to the next node in the list
};
// Global head pointers for the singly linked lists
NewPassport* newHead = nullptr;
OldPassport* oldHead = nullptr;
// File names for each passport type
const string regularFileName = "regular4.csv";
const string urgentFileName = "urgent4.csv";
const string expiredRegularFileName = "expired_regular4.csv";
const string expiredUrgentFileName = "expired_urgent4.csv";
// Forward declarations for all functions
void createNewPassport(), createOldPassports();
void updateNewPassport(), updateOldPassport();
void deleteNewPassport(),deleteOldPassport();
void searchNewPassport(),searchOldPassport();
void sortNewPassports(), sortOldPassports();
void displayNewPassports(), displayOldPassports();
void saveNewPassportsToFile(),saveOldPassportsToFile();
void loadNewPassportsFromFile(), loadOldPassportsFromFile();
void freeNewPassportList(); // Function to deallocate new passport list memory
void freeOldPassportList(); // Function to deallocate old passport list memory

// Helper functions (validation and date)
bool isValidDate(const string& date);
bool isOver18(const string& dob);
bool isAlphanumeric(const string& str);
bool isLettersOnly(const string& str);
bool isNumbersOnly(const string& str);
bool isUniqueNewID(const string& id, const string& excludeID = "");
bool isUniqueOldID(const string& id, const string& excludeID = "");
bool isUniquePassportNumber(const string& passportNumber, const string& excludeID = "");
string getCurrentDate();
string getDateOneMonthLater(const string& date);
string getDateTwoDaysLater(const string& date);
string getFileNameForPassType(const string& passType);

bool isValidDate(const string& date) {
    regex datePattern("\\d{4}-\\d{2}-\\d{2}");
    if (!regex_match(date, datePattern)) return false;

    int year, month, day;
    istringstream iss(date);
    char dash1, dash2;
    iss >> year >> dash1 >> month >> dash2 >> day;
    if (iss.fail() || dash1 != '-' || dash2 != '-') return false;

    if (month < 1 || month > 12 || day < 1 || day > 31) return false;
    return true;
}

bool isOver18(const string& dob) {
    time_t now = time(nullptr);
    tm* current = localtime(&now);
    int currYear = current->tm_year + 1900;
    int currMonth = current->tm_mon + 1;
    int currDay = current->tm_mday;
    int birthYear, birthMonth, birthDay;
    istringstream iss(dob);
    char dash1, dash2;
    iss >> birthYear >> dash1 >> birthMonth >> dash2 >> birthDay;
    if (iss.fail() || dash1 != '-' || dash2 != '-') return false;

    int age = currYear - birthYear;
    if (currMonth < birthMonth || (currMonth == birthMonth && currDay < birthDay)) {
        age--;
    }
    return age >= 18;
}
bool isAlphanumeric(const string& str) {
    regex pattern("^[a-zA-Z0-9]+$");
    return regex_match(str, pattern);
}
bool isLettersOnly(const string& str) {
    regex pattern("^[a-zA-Z ]+$");
    return regex_match(str, pattern);
}
bool isNumbersOnly(const string& str) {
    regex pattern("^[0-9]+$");
    return regex_match(str, pattern);
}
bool isUniqueNewID(const string& id, const string& excludeID) {
    // Check new passports list
    NewPassport* tempNew = newHead;
    while (tempNew != nullptr) {
        if (tempNew->id == id && tempNew->id != excludeID) return false;
        tempNew = tempNew->next;
    }
    // Check old passports list
    OldPassport* tempOld = oldHead;
    while (tempOld != nullptr) {
        if (tempOld->id == id) return false; // An ID from old list cannot be used for new
        tempOld = tempOld->next;
    }
    return true;
}

bool isUniqueOldID(const string& id, const string& excludeID) {
    // Check old passports list
    OldPassport* tempOld = oldHead;
    while (tempOld != nullptr) {
        if (tempOld->id == id && tempOld->id != excludeID) return false;
        tempOld = tempOld->next;
    }
    // Check new passports list
    NewPassport* tempNew = newHead;
    while (tempNew != nullptr) {
        if (tempNew->id == id) return false; // An ID from new list cannot be used for old
        tempNew = tempNew->next;
    }
    return true;
}

bool isUniquePassportNumber(const string& passportNumber, const string& excludeID) {
    OldPassport* temp = oldHead;
    while (temp != nullptr) {
        if (temp->passportNumber == passportNumber && temp->id != excludeID) return false;
        temp = temp->next;
    }
    return true;
}

// --- Date Helper Functions ---
string getCurrentDate() {
    time_t now = time(nullptr);
    tm* current = localtime(&now);
    stringstream ss;
    ss << setfill('0') << setw(4) << current->tm_year + 1900 << "-"
       << setw(2) << current->tm_mon + 1 << "-"
       << setw(2) << current->tm_mday;
    return ss.str();
}

string getDateOneMonthLater(const string& date) {
    int year, month, day;
    istringstream iss(date);
    char dash1, dash2;
    iss >> year >> dash1 >> month >> dash2 >> day;
    if (iss.fail() || dash1 != '-' || dash2 != '-') return "";

    month += 1;
    if (month > 12) {
        month = 1;
        year++;
    }

    stringstream ss;
    ss << setfill('0') << setw(4) << year << "-"
       << setw(2) << month << "-"
       << setw(2) << day;
    return ss.str();
}

string getDateTwoDaysLater(const string& date) {
    int year, month, day;
    istringstream iss(date);
    char dash1, dash2;
    iss >> year >> dash1 >> month >> dash2 >> day;
    if (iss.fail() || dash1 != '-' || dash2 != '-') return "";

    // Convert to tm struct for easier date manipulation
   tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1; // Not daylight saving time

    // Convert to time_t, add two days, then convert back to tm
    time_t time_stamp = mktime(&t);
    time_stamp += (2 * 24 * 60 * 60); // Add 2 days in seconds
    tm* new_time = localtime(&time_stamp);

    stringstream ss;
    ss << setfill('0') << setw(4) << new_time->tm_year + 1900 << "-"
       << setw(2) << new_time->tm_mon + 1 << "-"
       << setw(2) << new_time->tm_mday;
    return ss.str();
}

string getFileNameForPassType(const string& passType) {
    if (passType == "Regular") return regularFileName;
    if (passType == "Urgent") return urgentFileName;
    if (passType == "ExpiredRegular") return expiredRegularFileName;
    if (passType == "ExpiredUrgent") return expiredUrgentFileName;
    return "";
}

void saveNewPassportsToFile() {
 
    
}
void saveOldPassportsToFile() {


}
void loadNewPassportsFromFile() {


}
void loadOldPassportsFromFile() {


}
void createNewPassport() {


}
void createOldPassports() {


}
void updateNewPassport() {


}
void updateOldPassport() {


}
void deleteNewPassport() {
 string idToDelete;
    cout << "Enter New Passport ID to delete: ";
    getline(cin, idToDelete);
    NewPassport* current = newHead;
    NewPassport* prev = nullptr;
    while (current != nullptr && current->id != idToDelete) {
        prev = current;
        current = current->next;
    }
    if (current == nullptr) {
        cout << "New passport ID not found.\n";
        return;
    }
    if (prev == nullptr) { // Deleting the head node
        newHead = current->next;
    } else { // Deleting a non-head node
        prev->next = current->next;
    }
    delete current; // Free the memory
    saveNewPassportsToFile();
    cout << "New passport deleted successfully!\n";
}
void deleteOldPassport() {
string idToDelete;
    cout << "Enter Old Passport ID to delete: ";
    getline(cin, idToDelete);
    OldPassport* current = oldHead;
    OldPassport* prev = nullptr;
    while (current != nullptr && current->id != idToDelete) {
        prev = current;
        current = current->next;
    }
    if (current == nullptr) {
        cout << "Old passport ID not found.\n";
        return;
    }
    if (prev == nullptr) { // Deleting the head node
        oldHead = current->next;
    } else { // Deleting a non-head node
        prev->next = current->next;
    }
    delete current; // Free the memory
    saveOldPassportsToFile();
    cout << "Old passport deleted successfully!\n";
}
void searchNewPassport() {


}
void searchOldPassport() {


}
void sortNewPassports() {


}
void sortOldPassports() {


}
void displayNewPassports() {
    cout << "\n--- New Passports ---\n";
    if (newHead == nullptr) {
        cout << "No new passports to display.\n";
        return;
    }
    NewPassport* temp = newHead;
    while (temp != nullptr) {
        cout << "--------------------------------\n";
        cout << "Passport Type: " << temp->passType << "\n";
        // Removed display for Site Location, City, Office
        cout << "ID: " << temp->id << "\n";
        cout << "Name: " << temp->name << "\n";
        cout << "DOB: " << temp->dob << "\n";
        cout << "Nationality: " << temp->nationality << "\n";
        cout << "Phone Number: " << temp->phoneNumber << "\n";
        cout << "Created Date: " << temp->createdDate << "\n";
        cout << "Appointment Date: " << temp->appointmentDate << "\n";
        cout << "Payment: " << temp->payment << "\n";
        cout << "Payment Status: " << temp->paymentStatus << "\n";
        temp = temp->next;
    }
    cout << "--------------------------------\n";
}
void displayOldPassports() {
    if (oldHead == nullptr) {
        cout << "No old passports found.\n";
        return;
    }
    cout << "\n-- List of Old Passports --\n";
    OldPassport* temp = oldHead;
    while (temp != nullptr) {
        cout << "Type: " << temp->passType << ", ID: " << temp->id << ", Name: " << temp->name
             << ", DOB: " << temp->dob << ", Issue Date: " << temp->issueDate
             << ", Expiry Date: " << temp->expiredDate << ", Passport Number: " << temp->passportNumber
             << ", Account Number: " << temp->accountNumber << ", Balance: $" << fixed << setprecision(2) << temp->balance
             << ", Created: " << temp->createdDate << ", Appointment: " << temp->appointmentDate
             << ", Payment: $" << temp->payment << ", Status: " << temp->paymentStatus << "\n";
        temp = temp->next;
    }
}
void freeNewPassportList() {
   
   
}
void freeOldPassportList() {


}
int main() {
    loadNewPassportsFromFile(); // Load data on startup
    loadOldPassportsFromFile(); // Load data on startup

    int choice;
    do {
        cout << "\n--- Passport Management System ---\n";
        cout << "1. Create Passport\n";
        cout << "2. Update Passport\n";   
        cout << "3. Delete Passport\n";   
        cout << "4. Search Passport\n";  
        cout << "5. Display Passports\n"; 
        cout << "6. Sort Passports\n";    
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer after numeric input

        switch (choice) {
            case 1: { // Create Passport Sub-menu
                int createChoice;
                cout << "\n--- Create Passport ---\n";
                cout << "1. Create New Passport\n";
                cout << "2. Create Old Passports (Automated Sample)\n";
                cout << "Enter choice (1 or 2): ";
                cin >> createChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (createChoice) {
                    case 1: createNewPassport(); break;
                    case 2: createOldPassports(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 2: { // Update Passport Sub-menu
                int updateChoice;
                cout << "\n--- Update Passport ---\n";
                cout << "1. Update New Passport\n";
                cout << "2. Update Old Passport\n";
                cout << "Enter choice (1 or 2): ";
                cin >> updateChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (updateChoice) {
                    case 1: updateNewPassport(); break;
                    case 2: updateOldPassport(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 3: { // Delete Passport Sub-menu
                int deleteChoice;
                cout << "\n--- Delete Passport ---\n";
                cout << "1. Delete New Passport\n";
                cout << "2. Delete Old Passport\n";
                cout << "Enter choice (1 or 2): ";
                cin >> deleteChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (deleteChoice) {
                    case 1: deleteNewPassport(); break;
                    case 2: deleteOldPassport(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 4: { // Search Passport Sub-menu
                int searchChoice;
                cout << "\n--- Search Passport ---\n";
                cout << "1. Search New Passport\n";
                cout << "2. Search Old Passport\n";
                cout << "Enter choice (1 or 2): ";
                cin >> searchChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (searchChoice) {
                    case 1: searchNewPassport(); break;
                    case 2: searchOldPassport(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 5: { // Display Passports Sub-menu
                int displayChoice;
                cout << "\n--- Display Passports ---\n";
                cout << "1. Display New Passports\n";
                cout << "2. Display Old Passports\n";
                cout << "Enter choice (1 or 2): ";
                cin >> displayChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (displayChoice) {
                    case 1: displayNewPassports(); break;
                    case 2: displayOldPassports(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 6: { 
                int sortChoice;
                cout << "\n--- Sort Passports ---\n";
                cout << "1. Sort New Passports\n";
                cout << "2. Sort Old Passports\n";
                cout << "Enter choice (1 or 2): ";
                cin >> sortChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (sortChoice) {
                    case 1: sortNewPassports(); break;
                    case 2: sortOldPassports(); break;
                    default: cout << "Invalid choice. Returning to main menu.\n"; break;
                }
                break;
            }
            case 0: cout << "Exiting program. Goodbye!\n"; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    } while (choice != 0);

    // Free memory before exiting
    freeNewPassportList();
    freeOldPassportList();

    return 0;
}