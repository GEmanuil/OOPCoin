#include <iostream>
#include <fstream>
#include <chrono>
#include <time.h>

//commands
const char* EXIT = "exit";
const char* CREATE_USER = "create-user";


//def values
const char* ADMIN = "admin";
int DEFAULT_AMOUNT_TRANSACTION = 1500;
unsigned ADMIN_ID = 0;

using namespace std::chrono;

unsigned computeHash(const unsigned char* memory, int length) {
    unsigned hash = 0xbeaf;

    for (int c = 0; c < length; c++) {
        hash += memory[c];
        hash += hash << 10;
        hash = hash >> 6;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

struct User {
    unsigned id;
    char name[128];
};

struct Transaction {
    unsigned receiver;
    unsigned sender;
    double coins;
    long long time;
};

struct TransactionBlock {
    unsigned id;
    unsigned prevBlockId;
    unsigned prevBlockHash;
    int validTransactions;
    Transaction transactions[16];
};

long long secondsSince1970() {

    //code from ChatGPT!!! must be changed

    // get the current time as a time_point
    auto tp = system_clock::now();

    // calculate the duration between the time_point and the Unix epoch
    auto duration = duration_cast<seconds>(tp.time_since_epoch());

    return duration.count();

    //v2
    /*
        #include <iostream>
        #include <ctime>

        std::time_t timeSince1970() {
            std::time_t now = std::time(nullptr); // get the current time
            std::tm* start = std::localtime(&now); // get a time object representing the beginning of 1970 in the local timezone
            start->tm_sec = 0;
            start->tm_min = 0;
            start->tm_hour = 0;
            start->tm_mday = 1;
            start->tm_mon = 0;
            start->tm_year = 70;
            std::time_t diff = std::difftime(now, std::mktime(start)); // calculate the difference between the two time objects in seconds
            return diff;
        }

        int main() {
            std::time_t secondsSince1970 = timeSince1970();
            std::cout << "Seconds since 1970: " << secondsSince1970 << std::endl;
            return 0;
    }
    */
}

int randomSixDigitGenerator() {

    int result = 0;
    
    //if rand() returns zero cycle it until it returns something else
    while (result == 0) {
        result = rand() % 10;
    }

    int var = 0;

    for (int i = 0; i < 5; i++) {

        //time(0) + i -> because if not (without "+ i") it will generate only 111111 or 222222 or ... or 999999

        srand(time(0) + i);
        var = rand() % 10;

        result *= 10;
        result += var;
    }

    return result;
}

void commandSeperator(const char* enteredCommand, char* commandFor, char* commandTo) {

    size_t index = 0;

    for (index; enteredCommand[index] != ' '; ++index) {
        commandFor[index] = enteredCommand[index];
    }

    commandFor[index] = '\0';
    ++index;

    size_t i;
    for ( i = 0; enteredCommand[index] != '\0'; ++index, ++i) {
        commandTo[i] = enteredCommand[index];
    }
    commandTo[i] = '\0';
}

bool strCompare(const char* arr1, const char* arr2) {

    for (int i = 0; arr1[i] != '\0' || arr2[i] != '\0'; i++) {

        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    return true;
}

void stringCopy(const char* fromStr, char* toStr) {
    int i;
    for ( i = 0; fromStr[i] != '\0'; i++) {
        toStr[i] = fromStr[i];
    }
    toStr[i] = '\0';
}

void createTransaction(const unsigned from,const unsigned to, const int amount) {

    //TODO make it ifstream whe possible and change the file to .dat
    std::fstream binary("blocks.bin", std::ios::binary | std::ios::in | std::ios::out );

    if (!binary) {

        std::cout << "The file does not exist. Trying to create one..." << '\n';
        //binary.close();
        binary.clear();

        binary.open("blocks.bin", std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
        if (!binary.is_open()) {
            std::cout << "Cant create the file!!" << '\n';
        }

        Transaction transaction{to, from, amount, secondsSince1970()};

        std::cout << "Congrats, now you have " << transaction.coins << " to start trading!!!" << '\n';
        binary.write((const char*)&transaction, sizeof(Transaction));

        std::cout << "Sender: " << transaction.sender << '\n' << "Reciver: " << transaction.receiver << '\n' << "Amount: " << transaction.coins << '\n';

        binary.close();
    }

    else {

        if (!binary.is_open()) {
            std::cout << "Open error" << '\n';
        }
        binary.close();

        std::fstream binary2;
        binary2.open("blocks.bin", std::ios::binary | std::ios::app);

        Transaction transaction{ to, from, amount, secondsSince1970() };

        std::cout << "Congrats, now you have " << transaction.coins << " to start trading!!!" << '\n';
        binary2.write((const char*)(& transaction), sizeof(Transaction));

        std::cout << "Sender: " << transaction.sender << '\n' << "Reciver: " << transaction.receiver << '\n' << "Amount: " << transaction.coins << '\n';

        binary2.close();
    }

}


void createUser(const char* commandTo) {

    //TODO make it ifstream whe possible
    std::fstream binary("users.bin", std::ios::binary | std::ios::in | std::ios::out /*| std::ios::app if you uncomment this it'll not work*/);
    //TODO chage the file to .dat !!!

    //TODO if the file is not created already make a admin with id = 0
    //binary.open

    if (!binary) {

        std::cout << "The file does not exist. Trying to create one..." << '\n';
        //binary.close();
        binary.clear();

        binary.open("users.bin", std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
        if (!binary.is_open()) {
            std::cout << "Cant create the file!!" << '\n';
        }

        //creating the admin user with id = 0 
        User admin;
        admin.id = 0;
        stringCopy(ADMIN, admin.name);


        //making him first
        binary.seekg(0, std::ios::beg);
        binary.write((const char*)&admin, sizeof(User));


        //creating the wanted user
        User user;
        stringCopy(commandTo, user.name);
        user.id = randomSixDigitGenerator();
        binary.write((const char*)&user, sizeof(User));

        //making the default transaction from admin to created user
        createTransaction(ADMIN_ID, user.id, DEFAULT_AMOUNT_TRANSACTION);

        //PRINT EVERYTHING

        binary.seekg(0, std::ios::beg);

        binary.read((char*)&user, sizeof(User));

        std::cout << "User name from file: " << user.name << "; " << "User id: " << user.id << '\n';

        binary.read((char*)&user, sizeof(User));

        std::cout << "User name from file: " << user.name << "; " << "User id: " << user.id << '\n';

        binary.close();

    }

    else {

        binary.close();


        std::fstream binary2;
        binary2.open("users.bin", std::ios::binary | std::ios::app );
        //TODO if file broken to make it do smth OK
        if (!binary2.is_open()) {
            std::cout << "Open error" << '\n';
        }

        User user;

        stringCopy(commandTo, user.name);

        //TODO logic to make sure its random
        user.id = randomSixDigitGenerator();

        //binary.seekg(binary.eof());
        binary2.write((const char*)(&user), sizeof(user));

        createTransaction(ADMIN_ID, user.id, DEFAULT_AMOUNT_TRANSACTION);



        binary2.read((char*)&user, sizeof(User));


        std::cout << "Createed user name from file: " << user.name << " " << "User id: " << user.id << '\n';

        binary2.read((char*)&user, sizeof(User));

        std::cout << "User name from file: " << user.name << " " << "User id: " << user.id << '\n';

        binary2.close();
    }

}

void runCommand(char* commandFor, char* commandTo) {

    if (strCompare(commandFor, CREATE_USER)) {
        createUser(commandTo);
    }
}

void run() {

    char enteredCommand[64];

    while (!strCompare(enteredCommand, EXIT)) {

        std::cin.getline(enteredCommand, 64);

        char commandFor[32];
        char commandTo[64];

        commandSeperator(enteredCommand, commandFor, commandTo);
        runCommand(commandFor, commandTo);

    }

}

int main() {
    
    run();

    return 0;

}