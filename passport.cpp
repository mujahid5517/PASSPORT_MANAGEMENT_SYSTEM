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
  ofstream outRegular(regularFileName, ios::trunc);
    if (outRegular) {
        outRegular << "PassType,ID,Name,DOB,Nationality,Phone,CreatedDate,AppointmentDate,Payment,PaymentStatus\n";
        outRegular.close();
    }
    ofstream outUrgent(urgentFileName, ios::trunc);
    if (outUrgent) {
        outUrgent << "PassType,ID,Name,DOB,Nationality,Phone,CreatedDate,AppointmentDate,Payment,PaymentStatus\n";
        outUrgent.close();
    }
    NewPassport* temp = newHead;
    while (temp != nullptr) {
        string fileName = getFileNameForPassType(temp->passType);
        ofstream out(fileName, ios::app); // Open in append mode
        if (!out) {
            cout << "Error opening file " << fileName << " for writing!\n";
            return;
        }
        out << temp->passType << ","
            << temp->id << "," << temp->name << ","
            << temp->dob << "," << temp->nationality << "," << temp->phoneNumber << ","
            << temp->createdDate << "," << temp->appointmentDate << "," << temp->payment << ","
            << temp->paymentStatus << "\n";
        out.close();
        temp = temp->next;
    }   
}
void saveOldPassportsToFile() {
 ofstream outExpiredRegular(expiredRegularFileName, ios::trunc);
    if (outExpiredRegular) {
        outExpiredRegular << "PassType,ID,Name,DOB,IssueDate,ExpiredDate,PassportNumber,AccountNumber,Balance,CreatedDate,AppointmentDate,Payment,PaymentStatus\n";
        outExpiredRegular.close();
    }
    ofstream outExpiredUrgent(expiredUrgentFileName, ios::trunc);
    if (outExpiredUrgent) {
        outExpiredUrgent << "PassType,ID,Name,DOB,IssueDate,ExpiredDate,PassportNumber,AccountNumber,Balance,CreatedDate,AppointmentDate,Payment,PaymentStatus\n";
        outExpiredUrgent.close();
    }
    OldPassport* temp = oldHead;
    while (temp != nullptr) {
        string fileName = getFileNameForPassType(temp->passType);
        ofstream out(fileName, ios::app); // Open in append mode
        if (!out) {
            cout << "Error opening file " << fileName << " for writing!\n";
            return;
        }
        out << temp->passType << "," << temp->id << "," << temp->name << ","
            << temp->dob << "," << temp->issueDate << "," << temp->expiredDate << ","
            << temp->passportNumber << "," << temp->accountNumber << "," << fixed << setprecision(2) << temp->balance << ","
            << temp->createdDate << "," << temp->appointmentDate << "," << temp->payment << ","
            << temp->paymentStatus << "\n";
        out.close();
        temp = temp->next;
    }
}
void loadNewPassportsFromFile() {
freeNewPassportList(); 
    string fileNames[] = {regularFileName, urgentFileName};
    for (const string& fileName : fileNames) {
        ifstream in(fileName);
        if (!in) continue; 
        string line;
        getline(in, line); 
        while (getline(in, line)) {
            stringstream ss(line);
            string token;
            NewPassport* newPass = new NewPassport(); 
            getline(ss, newPass->passType, ',');
            getline(ss, newPass->id, ',');
            getline(ss, newPass->name, ',');
            getline(ss, newPass->dob, ',');
            getline(ss, newPass->nationality, ',');
            getline(ss, newPass->phoneNumber, ',');
            getline(ss, newPass->createdDate, ',');
            getline(ss, newPass->appointmentDate, ',');
            getline(ss, newPass->payment, ',');
            getline(ss, newPass->paymentStatus); 
            newPass->next = nullptr;
            // Add to the end of the list
            if (newHead == nullptr) {
                newHead = newPass;
            } else {
                NewPassport* temp = newHead;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = newPass;
            }
        }
        in.close();
    }
}
void loadOldPassportsFromFile() {
 freeOldPassportList(); // Clear existing list before loading
    string fileNames[] = {expiredRegularFileName, expiredUrgentFileName};
    for (const string& fileName : fileNames) {
        ifstream in(fileName);
        if (!in) continue; // File might not exist yet
        string line;
        getline(in, line); // Skip header
        while (getline(in, line)) {
            stringstream ss(line);
            string token;
            OldPassport* oldPass = new OldPassport(); 
            getline(ss, oldPass->passType, ',');
            getline(ss, oldPass->id, ',');
            getline(ss, oldPass->name, ',');
            getline(ss, oldPass->dob, ',');
            getline(ss, oldPass->issueDate, ',');
            getline(ss, oldPass->expiredDate, ',');
            getline(ss, oldPass->passportNumber, ',');
            getline(ss, oldPass->accountNumber, ',');
            getline(ss, token, ','); // Read balance as string first
            oldPass->balance = stod(token); 
            getline(ss, oldPass->createdDate, ',');
            getline(ss, oldPass->appointmentDate, ',');
            getline(ss, oldPass->payment, ',');
            getline(ss, oldPass->paymentStatus);
            oldPass->next = nullptr;
            // Add to the end of the list
            if (oldHead == nullptr) {
                oldHead = oldPass;
            } else {
                OldPassport* temp = oldHead;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = oldPass;
            }
        }
        in.close();
    }
}
void createNewPassport() {
string id, name, dob, nationality, phoneNumber, payment, paymentStatus, passType;
    int passportTypeChoice;
    cout << "Select Passport Type:\n1. Regular\n2. Urgent\nEnter choice (1 or 2): ";
    cin >> passportTypeChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
    const int MAX_ID_LEN = 10;
    do {
        cout << "Enter ID (max " << MAX_ID_LEN << " chars, alphanumeric): ";
        getline(cin, id);
        if (id.length() > MAX_ID_LEN) {
            cout << "your size is limit pls try again (max " << MAX_ID_LEN << " chars).\n";
            continue; // Ask for input again
        }
        if (!isAlphanumeric(id)) {
            cout << "Invalid ID: Must be alphanumeric.\n";
        } else if (!isUniqueNewID(id)) {
            cout << "Error: This ID already exists.\n";
        }
    } while (id.length() > MAX_ID_LEN || !isAlphanumeric(id) || !isUniqueNewID(id));
    const int MAX_NAME_LEN = 25;
    do {
        cout << "Enter Full Name (max " << MAX_NAME_LEN << " chars, letters only): ";
        getline(cin, name);
        if (name.length() > MAX_NAME_LEN) {
            cout << "your size is limit pls try again (max " << MAX_NAME_LEN << " chars).\n";
            continue;
        }
        if (!isLettersOnly(name)) cout << "Invalid name: Must contain letters only.\n";
    } while (name.length() > MAX_NAME_LEN || !isLettersOnly(name));
    do {
        cout << "Enter Date of Birth (YYYY-MM-DD): ";
        getline(cin, dob);
        if (!isValidDate(dob)) cout << "Invalid format. Try again.\n";
        else if (!isOver18(dob)) cout << "Error: Applicant must be 18 or older.\n";
    } while (!isValidDate(dob) || !isOver18(dob));
    const int MAX_NATIONALITY_LEN = 15;
    do {
        cout << "Enter Nationality (max " << MAX_NATIONALITY_LEN << " chars, letters only): ";
        getline(cin, nationality);
        if (nationality.length() > MAX_NATIONALITY_LEN) {
            cout << "your size is limit pls try again (max " << MAX_NATIONALITY_LEN << " chars).\n";
            continue;
        }
        if (!isLettersOnly(nationality)) cout << "Invalid nationality: Must contain letters only.\n";
    } while (nationality.length() > MAX_NATIONALITY_LEN || !isLettersOnly(nationality));
    const int MAX_PHONE_LEN = 12;
    do {
        cout << "Enter Phone Number (max " << MAX_PHONE_LEN << " chars, numbers only): ";
        getline(cin, phoneNumber);
        if (phoneNumber.length() > MAX_PHONE_LEN) {
            cout << "your size is limit pls try again (max " << MAX_PHONE_LEN << " chars).\n";
            continue;
        }
        if (!isNumbersOnly(phoneNumber)) cout << "Invalid phone number: Must contain numbers only.\n";
    } while (phoneNumber.length() > MAX_PHONE_LEN || !isNumbersOnly(phoneNumber));
    if (passportTypeChoice == 1) {
        payment = "5000";
        passType = "Regular";
        cout << "Payment Amount: $" << payment << " (Regular Passport)\n";
    } else if (passportTypeChoice == 2) {
        payment = "25000";
        passType = "Urgent";
        cout << "Payment Amount: $" << payment << " (Urgent Passport)\n";
    } else {
        cout << "Invalid passport type choice. Passport creation cancelled.\n";
        return;
    }
    do {
        cout << "Is payment confirmed? (Yes/No): ";
        getline(cin, paymentStatus);
        if (paymentStatus != "Yes" && paymentStatus != "No") {
            cout << "Invalid status: Must be Yes or No.\n";
        }
    } while (paymentStatus != "Yes" && paymentStatus != "No");
    if (paymentStatus != "Yes") {
        cout << "Error: Passport cannot be created until payment is confirmed.\n";
        return;
    }
    string createdDate = getCurrentDate();
    cout << "Created Date (auto-set): " << createdDate << "\n";
    string appointmentDate;
    if (passType == "Regular") {
        appointmentDate = getDateOneMonthLater(createdDate);
        cout << "Appointment Date (auto-set, 1 month from creation): " << appointmentDate << "\n";
    } else {
        appointmentDate = getDateTwoDaysLater(createdDate);
        cout << "Appointment Date (auto-set, 2 days from creation): " << appointmentDate << "\n";
    }
    if (!isValidDate(appointmentDate)) {
        cout << "Error: Invalid appointment date generated. Passport creation cancelled.\n";
        return;
    }
  // Create a new node and add to the end of the list
    NewPassport* newPass = new NewPassport;
    newPass->passType = passType;
    newPass->id = id;
    newPass->name = name;
    newPass->dob = dob;
    newPass->nationality = nationality;
    newPass->phoneNumber = phoneNumber;
    newPass->createdDate = createdDate;
    newPass->appointmentDate = appointmentDate;
    newPass->payment = payment;
    newPass->paymentStatus = paymentStatus;
    newPass->next = nullptr;
    if (newHead == nullptr) {
        newHead = newPass;
    } else {
        NewPassport* temp = newHead;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newPass;
    }
    saveNewPassportsToFile();
    cout << "New passport added successfully!\n";
}
void createOldPassports() {
 string persons[5][8] = {
        {"001", "Abebea", "1990-05-15", "2015-06-01", "2020-06-01", "P123", "ACC1001", "20000.00"},
        {"002", "Aster", "1985-08-22", "2014-09-10", "2019-09-10", "P234", "ACC1002", "3000.00"},
        {"003", "abdi", "1992-03-10", "2016-04-15", "2021-04-15", "P345", "ACC1003", "35000.00"},
        {"004", "Lami", "1988-11-30", "2013-12-05", "2018-12-05", "P456", "ACC1004", "10000.00"},
        {"005", "Robel", "1995-07-20", "2017-08-25", "2022-08-25", "P567", "ACC1005", "40000.00"},
    };
    cout << "--- Create Old Passport ---\n";
    int searchOption;
    string searchValue;
    int index = -1;
    cout << "Search Old Record By:\n";
    cout << "1. ID\n2. Name\n3. Passport Number\nEnter choice: ";
    cin >> searchOption;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch (searchOption) {
        case 1:
            cout << "Enter ID (or ## to cancel): ";
            getline(cin, searchValue);
            if (searchValue == "##") return;
            for (int i = 0; i < 5; ++i) {
                if (persons[i][0] == searchValue) {
                    index = i;
                    break;
                }
            }
            break;
        case 2:
            cout << "Enter Name: ";
            getline(cin, searchValue);
            for (int i = 0; i < 5; ++i) {
                if (persons[i][1] == searchValue) {
                    index = i;
                    break;
                }
            }
            break;
        case 3:
            cout << "Enter Passport Number: ";
            getline(cin, searchValue);
            for (int i = 0; i < 5; ++i) {
                if (persons[i][5] == searchValue) {
                    index = i;
                    break;
                }
            }
            break;
        default:
            cout << "Invalid search option.\n";
            return;
    }
    if (index == -1) {
        cout << "No matching record found.\n";
        return;
    }
    int typeChoice;
    string passType="expired";
    cout << "Select Passport Type:\n1. Expired Regular\n2. Expired Urgent\nEnter choice (1 or 2): ";
    cin >> typeChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (typeChoice == 1) passType = "ExpiredRegular";
    else if (typeChoice == 2) passType = "ExpiredUrgent";
    else {
        cout << "Invalid passport type choice. Operation cancelled.\n";
        return;
    }
    string enteredId = persons[index][0];
    string enteredPassportNumber = persons[index][5];
    if (!isAlphanumeric(enteredId)) {
        cout << "Invalid ID: Must be alphanumeric.\n";
        return;
    }
    if (!isUniqueOldID(enteredId)) {
        cout << "Error: This ID already exists in the system.\n";
        return;
    }
    if (!isUniquePassportNumber(enteredPassportNumber)) {
        cout << "Error: Passport number already exists in the system.\n";
        return;
    }
    // Create and fill old passport object
    OldPassport* oldPass = new OldPassport();
    oldPass->passType = passType;
    oldPass->id = enteredId;
    oldPass->name = persons[index][1];
    oldPass->dob = persons[index][2];
    oldPass->issueDate = persons[index][3];
    oldPass->expiredDate = persons[index][4];
    oldPass->passportNumber = enteredPassportNumber;
    oldPass->accountNumber = persons[index][6];
    oldPass->balance = stod(persons[index][7]);
    oldPass->createdDate = getCurrentDate();
    oldPass->appointmentDate = (passType == "ExpiredRegular")
                                ? getDateOneMonthLater(oldPass->createdDate)
                                : getDateTwoDaysLater(oldPass->createdDate);
    oldPass->payment = "0.0";
    oldPass->paymentStatus = "Pending";
    oldPass->next = nullptr;
    if (oldHead == nullptr) {
        oldHead = oldPass;
    } else {
        OldPassport* temp = oldHead;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = oldPass;
    }
    cout << "Old Passport Created:\n";
    cout << "ID: " << oldPass->id
         << " | Name: " << oldPass->name
         << " | DOB: " << oldPass->dob
         << " | Issue Date: " << oldPass->issueDate
         << " | Expired Date: " << oldPass->expiredDate
         << " | Passport No.: " << oldPass->passportNumber
         << " | Balance: $" << fixed << setprecision(2) << oldPass->balance
         << " | Type: " << oldPass->passType
         << " | Created: " << oldPass->createdDate
         << " | Appointment: " << oldPass->appointmentDate
         << " | Payment: " << oldPass->payment
         << " | Status: " << oldPass->paymentStatus << "\n";
    saveOldPassportsToFile();
    cout << "--- Passport creation completed ---\n";
}
void updateNewPassport() {
string idToUpdate;
    cout << "Enter New Passport ID to update: ";
    getline(cin, idToUpdate);

    NewPassport* current = newHead;
    while (current != nullptr && current->id != idToUpdate) {
        current = current->next;
    }

    if (current != nullptr) {

        string newId, newName, newDob, newNationality, newPhoneNumber, newPayment, paymentStatus, newPassType;
        int passportTypeChoice;

        cout << "Select New Passport Type:\n1. Regular\n2. Urgent\nEnter choice (1 or 2): ";
        cin >> passportTypeChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        const int MAX_ID_LEN = 10;
        do {
            cout << "Enter New ID (max " << MAX_ID_LEN << " chars, alphanumeric, current: " << current->id << "): ";
            getline(cin, newId);
            if (newId.length() > MAX_ID_LEN) {
                cout << "your size is limit pls try again (max " << MAX_ID_LEN << " chars).\n";
                continue;
            }
            if (!isAlphanumeric(newId)) {
                cout << "Invalid ID: Must be alphanumeric.\n";
            } else if (!isUniqueNewID(newId, current->id)) {
                cout << "Error: This ID already exists.\n";
            }
        } while (newId.length() > MAX_ID_LEN || !isAlphanumeric(newId) || !isUniqueNewID(newId, current->id));

        const int MAX_NAME_LEN = 25;
        do {
            cout << "Enter New Full Name (max " << MAX_NAME_LEN << " chars, letters only, current: " << current->name << "): ";
            getline(cin, newName);
            if (newName.length() > MAX_NAME_LEN) {
                cout << "your size is limit pls try again (max " << MAX_NAME_LEN << " chars).\n";
                continue;
            }
            if (!isLettersOnly(newName)) cout << "Invalid name: Must contain letters only.\n";
        } while (newName.length() > MAX_NAME_LEN || !isLettersOnly(newName));

        do {
            cout << "Enter New Date of Birth (YYYY-MM-DD, current: " << current->dob << "): ";
            getline(cin, newDob);
            if (!isValidDate(newDob)) cout << "Invalid format. Try again.\n";
            else if (!isOver18(newDob)) cout << "Error: Applicant must be 18 or older.\n";
        } while (!isValidDate(newDob) || !isOver18(newDob));


        const int MAX_NATIONALITY_LEN = 15;
        do {
            cout << "Enter New Nationality (max " << MAX_NATIONALITY_LEN << " chars, letters only, current: " << current->nationality << "): ";
            getline(cin, newNationality);
            if (newNationality.length() > MAX_NATIONALITY_LEN) {
                cout << "your size is limit pls try again (max " << MAX_NATIONALITY_LEN << " chars).\n";
                continue;
            }
            if (!isLettersOnly(newNationality)) cout << "Invalid nationality: Must contain letters only.\n";
        } while (newNationality.length() > MAX_NATIONALITY_LEN || !isLettersOnly(newNationality));

        const int MAX_PHONE_LEN = 12;
        do {
            cout << "Enter New Phone Number (max " << MAX_PHONE_LEN << " chars, numbers only, current: " << current->phoneNumber << "): ";
            getline(cin, newPhoneNumber);
            if (newPhoneNumber.length() > MAX_PHONE_LEN) {
                cout << "your size is limit pls try again (max " << MAX_PHONE_LEN << " chars).\n";
                continue;
            }
            if (!isNumbersOnly(newPhoneNumber)) cout << "Invalid phone number: Must contain numbers only.\n";
        } while (newPhoneNumber.length() > MAX_PHONE_LEN || !isNumbersOnly(newPhoneNumber));

        if (passportTypeChoice == 1) {
            newPayment = "5000";
            newPassType = "Regular";
            cout << "Payment Amount: $" << newPayment << " (Regular Passport)\n";
        } else if (passportTypeChoice == 2) {
            newPayment = "25000";
            newPassType = "Urgent";
            cout << "Payment Amount: $" << newPayment << " (Urgent Passport)\n";
        } else {
            cout << "Invalid passport type choice. Update cancelled.\n";
            return;
        }

        do {
            cout << "Is payment confirmed? (Yes/No): ";
            getline(cin, paymentStatus);
            if (paymentStatus != "Yes" && paymentStatus != "No") {
                cout << "Invalid status: Must be Yes or No.\n";
            }
        } while (paymentStatus != "Yes" && paymentStatus != "No");

        if (paymentStatus != "Yes") {
            cout << "Error: Passport cannot be updated until payment is confirmed.\n";
            return;
        }

        string createdDate = getCurrentDate();
        cout << "Created Date (auto-set): " << createdDate << "\n";

        string appointmentDate;
        if (newPassType == "Regular") {
            appointmentDate = getDateOneMonthLater(createdDate);
            cout << "Appointment Date (auto-set, 1 month from creation): " << appointmentDate << "\n";
        } else {
            appointmentDate = getDateTwoDaysLater(createdDate);
            cout << "Appointment Date (auto-set, 2 days from creation): " << appointmentDate << "\n";
        }
        if (!isValidDate(appointmentDate)) {
            cout << "Error: Invalid appointment date generated. Passport creation cancelled.\n";
            return;
        }
        // Update the struct fields of the found node
        current->passType = newPassType;
        current->id = newId;
        current->name = newName;
        current->dob = newDob;
        current->nationality = newNationality;
        current->phoneNumber = newPhoneNumber;
        current->createdDate = createdDate;
        current->appointmentDate = appointmentDate;
        current->payment = newPayment;
        current->paymentStatus = paymentStatus;

        saveNewPassportsToFile();
        cout << "New passport updated successfully!\n";
    } else {
        cout << "New passport ID not found.\n";
    }

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
   int choice;
    cout << "Search New Passport By:\n1. ID\n2. Name\nEnter choice: ";
    cin >> choice;
    cin.ignore();
    string input;
    if (choice == 1) {
        cout << "Enter  ID to search: ";
    } else if (choice == 2) {
        cout << "Enter  Name to search: ";
    } else {
        cout << "Invalid choice.\n";
        return;
    }
    getline(cin, input);
    NewPassport* temp = newHead;
    while (temp != nullptr) {
        if ((choice == 1 && temp->id == input) || (choice == 2 && temp->name == input)) {
            cout << "New Passport Found:\n";
            cout << "Type: " << temp->passType << ", ID: " << temp->id << ", Name: " << temp->name
                 << ", DOB: " << temp->dob << ", Nationality: " << temp->nationality
                 << ", Phone: " << temp->phoneNumber
                 << ", Created: " << temp->createdDate << ", Appointment: " << temp->appointmentDate
                 << ", Payment: $" << temp->payment << ", Status: " << temp->paymentStatus << "\n";
            return;
        }
        temp = temp->next;
    }
    cout << "New passport not found.\n";
}
void searchOldPassport() {
    int choice;
    cout << "Search Old Passport By:\n1. ID\n2. Name\nEnter choice: ";
    cin >> choice;
    cin.ignore();  
    string input;
    if (choice == 1) {
        cout << "Enter Old Passport ID to search: ";
    } else if (choice == 2) {
        cout << "Enter Old Passport Name to search: ";
    } else {
        cout << "Invalid choice.\n";
        return;
    }
    getline(cin, input);
    OldPassport* temp = oldHead;
    while (temp != nullptr) {
        if ((choice == 1 && temp->id == input) || (choice == 2 && temp->name == input)) {
            cout << "Old Passport Found:\n";
            cout << "Type: " << temp->passType << ", ID: " << temp->id << ", Name: " << temp->name
                 << ", DOB: " << temp->dob << ", Issue Date: " << temp->issueDate
                 << ", Expiry Date: " << temp->expiredDate << ", Passport Number: " << temp->passportNumber
                 << ", Account Number: " << temp->accountNumber << ", Balance: $" << fixed << setprecision(2) << temp->balance
                 << ", Created: " << temp->createdDate << ", Appointment: " << temp->appointmentDate
                 << ", Payment: $" << temp->payment << ", Status: " << temp->paymentStatus << "\n";
            return;
        }
        temp = temp->next;
    }
    cout << "Old passport not found.\n";
}
void sortNewPassports() {
 if (newHead == nullptr || newHead->next == nullptr) {
        cout << "No new passports to sort or only one passport exists.\n";
        return;
    }
    int sortOption;
    cout << "Sort New Passports Options:\n1. Sort by Name\n2. Sort by Passport Type\nEnter choice (1 or 2): ";
    cin >> sortOption;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    if (sortOption != 1 && sortOption != 2) {
        cout << "Invalid sort option.\n";
        return;
    }
    NewPassport* current;
    NewPassport* index;
    for (current = newHead; current != nullptr; current = current->next) {
        for (index = current->next; index != nullptr; index = index->next) {
            bool shouldSwap = false;
            if (sortOption == 1) {
                if (current->name > index->name) {
                    shouldSwap = true;
                } else if (current->name == index->name && current->passType > index->passType) {
                    shouldSwap = true; 
                }
            } else { 
                if (current->passType > index->passType) {
                    shouldSwap = true;
                } else if (current->passType == index->passType && current->name > index->name) {
                    shouldSwap = true; 
                }
            }
            if (shouldSwap) {          
                string tempPassType = current->passType;
                string tempId = current->id;
                string tempName = current->name;
                string tempDob = current->dob;
                string tempNationality = current->nationality;
                string tempPhoneNumber = current->phoneNumber;
                string tempCreatedDate = current->createdDate;
                string tempAppointmentDate = current->appointmentDate;
                string tempPayment = current->payment;
                string tempPaymentStatus = current->paymentStatus;
                current->passType = index->passType;
                current->id = index->id;
                current->name = index->name;
                current->dob = index->dob;
                current->nationality = index->nationality;
                current->phoneNumber = index->phoneNumber;
                current->createdDate = index->createdDate;
                current->appointmentDate = index->appointmentDate;
                current->payment = index->payment;
                current->paymentStatus = index->paymentStatus;
                index->passType = tempPassType;
                index->id = tempId;
                index->name = tempName;
                index->dob = tempDob;
                index->nationality = tempNationality;
                index->phoneNumber = tempPhoneNumber;
                index->createdDate = tempCreatedDate;
                index->appointmentDate = tempAppointmentDate;
                index->payment = tempPayment;
                index->paymentStatus = tempPaymentStatus;
            }
        }
    }
    saveNewPassportsToFile();
    cout << "New passports sorted by " << (sortOption == 1 ? "name" : "passport type") << ".\n";
}
void sortOldPassports() {
   if (oldHead == nullptr || oldHead->next == nullptr) {
        cout << "No old passports to sort or only one passport exists.\n";
        return;
    }
    int sortOption;
    cout << "Sort Old Passports Options:\n1. Sort by Name\n2. Sort by Passport Type\nEnter choice (1 or 2): ";
    cin >> sortOption;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    if (sortOption != 1 && sortOption != 2) {
        cout << "Invalid sort option.\n";
        return;
    }
    OldPassport* current;
    OldPassport* index;
    for (current = oldHead; current != nullptr; current = current->next) {
        for (index = current->next; index != nullptr; index = index->next) {
            bool shouldSwap = false;
            if (sortOption == 1) { 
                if (current->name > index->name) {
                    shouldSwap = true;
                } else if (current->name == index->name && current->passType > index->passType) {
                    shouldSwap = true; 
                }
            } else { 
                if (current->passType > index->passType) {
                    shouldSwap = true;
                } else if (current->passType == index->passType && current->name > index->name) {
                    shouldSwap = true; 
                }
            }
            if (shouldSwap) {               
                string tempPassType = current->passType;
                string tempId = current->id;
                string tempName = current->name;
                string tempDob = current->dob;
                string tempIssueDate = current->issueDate;
                string tempExpiredDate = current->expiredDate;
                string tempPassportNumber = current->passportNumber;
                string tempAccountNumber = current->accountNumber;
                double tempBalance = current->balance;
                string tempCreatedDate = current->createdDate;
                string tempAppointmentDate = current->appointmentDate;
                string tempPayment = current->payment;
                string tempPaymentStatus = current->paymentStatus;
                current->passType = index->passType;
                current->id = index->id;
                current->name = index->name;
                current->dob = index->dob;
                current->issueDate = index->issueDate;
                current->expiredDate = index->expiredDate;
                current->passportNumber = index->passportNumber;
                current->accountNumber = index->accountNumber;
                current->balance = index->balance;
                current->createdDate = index->createdDate;
                current->appointmentDate = index->appointmentDate;
                current->payment = index->payment;
                current->paymentStatus = index->paymentStatus;
                index->passType = tempPassType;
                index->id = tempId;
                index->name = tempName;
                index->dob = tempDob;
                index->issueDate = tempIssueDate;
                index->expiredDate = tempExpiredDate;
                index->passportNumber = tempPassportNumber;
                index->accountNumber = tempAccountNumber;
                index->balance = tempBalance;
                index->createdDate = tempCreatedDate;
                index->appointmentDate = tempAppointmentDate;
                index->payment = tempPayment;
                index->paymentStatus = tempPaymentStatus;
            }
        }
    }
    saveOldPassportsToFile();
    cout << "Old passports sorted by " << (sortOption == 1 ? "name" : "passport type") << ".\n";
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
    NewPassport* current = newHead;
    NewPassport* nextNode;
    while (current != nullptr) {
        nextNode = current->next;
        delete current;
        current = nextNode;
    }
    newHead = nullptr;
}
void freeOldPassportList() {
 OldPassport* current = oldHead;
    OldPassport* nextNode;
    while (current != nullptr) {
        nextNode = current->next;
        delete current;
        current = nextNode;
    }
    oldHead = nullptr;

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