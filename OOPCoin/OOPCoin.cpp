
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//                      RAZQSNENIQ    
//  
//       Izbral sum da napravq dopulnitelen fail transactions.dat koito da
//       zapazva structorite Transaction, za po lesno razrabotvane i chetene na
//       programata. Izbral sum i podhoda da ne zapazvam danni v RAM pametta za 
//       osigorqvane na burzodeistvieto, no cheteneto i pisaneto v fail vinagi 
//       kogato imame nujda ot takova, za da ima po sigurno suhranenie na danni
//       i ulesnena logika.
//  
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <iostream>
#include <fstream>
#include <cstring>
#include <random>
#include <ctime>

#pragma warning (disable: 4996)


void run();
void runCommand(const char* commandFor, const char* commandTo);
void welcomeMessage();
void printHelp();
void commandSeperator(const char* enteredCommand, char* commandFor, char* commandTo);
void computeBiggestBlocks();
void sortBlocks(int blocks[1024], int length);
double checkBlockAmount(int blockID);
int giveBlockTransactionArrIndex(int blockID);
unsigned computeHash(const unsigned char* memory, int length);
std::time_t secondsSince1970();
int randomSixDigitGenerator();
bool strCompare(const char* arr1, const char* arr2);
void stringCopy(const char* fromStr, char* toStr);
short computeBlockArrSize(int blockCounter);
void computeWealthiestUsers();
void sortUsers(int users[1024], int length);
void readTransactionInfo();
bool userIdExistsInArr(int userID, int users[1024]);
void checkTransactions();
void sendCoins(const char* sendFromTO);
double checkUserAmount(const int userID);
bool ifUserDeletedChecker(unsigned id);
double checkUserAmount(const char* userName);
void getUserName(const int ID, char* userName);
int getUserId(const char* userName);
bool checkIfUserExists(const char* userName);
void seperateSenderFromReciever(const char* sendFromTo, char* from, char* to);
void removeUser(const char* userToRemove);
void createUser(const char* commandTo);
void createTransaction(const unsigned from, const unsigned to, const double amount);
short giveBlockTransactionArrIndex();


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

std::time_t secondsSince1970() {

    //                                                  ...!!!!!...

      /* !!!vrushta vremeto v sekundi ot nachaloto na taka narecheniq "Unix epoch" koeto suvsem sluchaino se pada 01.01.1970 00:00:00 :) */



    time_t timeSince1970 = time(nullptr);

    return timeSince1970;
}

int randomSixDigitGenerator() {

    int result = 0;

    //if rand() returns zero cycle it until it returns something else
    while (result == 0) {
        result = rand() % 10;
    }

    int var = 0;

    //Making sure that it'll never genrate the same number with seeding (current time - random num) 

    srand(time(nullptr) - rand());

    for (int i = 0; i < 5; i++) {

        var = rand() % 10;

        result *= 10;
        result += var;
    }

    return result;
}

short giveBlockTransactionArrIndex() {

    std::ifstream binary;

    binary.open("transactions.dat", std::ios::binary | std::ios::in);
    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    binary.seekg(binary.beg);
    int transactionCounter = 0;

    Transaction transaction;

    while (binary.read((char*)&transaction, sizeof(Transaction))) {
        transactionCounter++;
    }

    binary.close();

    const unsigned numOfTransactionInBlock = 16;
    return (transactionCounter % numOfTransactionInBlock);
}

void mentionTransaction(const Transaction& transaction, const bool fileExists) {

    std::ofstream binary;


    if (fileExists) {
        binary.open("transactions.dat", std::ios::binary | std::ios::app | std::ios::out);
        if (!binary.is_open()) {
            std::cout << "Transaction file open error!!! ";
        }
        binary.write((const char*)&transaction, sizeof(Transaction));
        std::cout << "\n" << "Saved a transaction with a reciver id = " << transaction.receiver << "\n" << "\n";

    }
    else {
        binary.open("transactions.dat", std::ios::binary | std::ios::trunc | std::ios::out);
        if (!binary.is_open()) {
            std::cout << "Transaction file open error!!! ";
        }
        binary.write((const char*)&transaction, sizeof(Transaction));
        std::cout << "\n" << "Saved a transaction with a reciver id = " << transaction.receiver << "\n" << "\n";
    }

    binary.close();
}

void createTransaction(const unsigned from, const unsigned to, const double amount) {

    std::fstream binary("blocks.dat", std::ios::binary | std::ios::in | std::ios::out);

    if (!binary) {

        std::cout << '\n' << "The file does not exist. Trying to create one..." << '\n';
        binary.clear();

        binary.open("blocks.dat", std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
        if (!binary.is_open()) {
            std::cout << "Cant create the file!!" << '\n';
        }

        long long sec = secondsSince1970();

        Transaction transaction{ to, from, amount, sec };

        mentionTransaction(transaction, false);

        TransactionBlock block;
        block.id = randomSixDigitGenerator();

        std::cout << '\n' << "!!!!CREATED A TRANSACTION BLOCK with ID: " << block.id << '\n';

        block.prevBlockId = block.id;
        block.validTransactions = 0;
        block.transactions[0] = transaction;
        block.validTransactions++;

        binary.write((const char*)&block, sizeof(block));

        binary.close();
    }

    else {
        if (!binary.is_open()) {
            std::cout << "Open error" << '\n';
        }
        binary.close();

        std::fstream binary2;
        binary2.open("blocks.dat", std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);

        if (!binary2.is_open()) {
            std::cout << "Open error" << '\n';
        }

        long long sec = secondsSince1970();

        Transaction transaction{ to, from, amount, sec };
        TransactionBlock block;

        if (giveBlockTransactionArrIndex() == 0) {

            block.id = randomSixDigitGenerator();
            block.transactions[0] = transaction;

            TransactionBlock lastBlock;

            binary2.seekg(0, std::ios::end);


            const std::streampos fileSize = binary2.tellg();
            // Calculate the number of structures in the file
            const std::size_t numStructs = fileSize / sizeof(TransactionBlock);

            // Move the file pointer to the last structure in the file
            binary2.seekg((numStructs - 1) * sizeof(TransactionBlock), std::ios::beg);

            binary2.read((char*)(&lastBlock), sizeof(TransactionBlock));


            block.prevBlockHash = computeHash((const unsigned char*)(&lastBlock), sizeof(lastBlock));

            std::cout << '\n' << "Previous block hash:  " << block.prevBlockHash << "\n";

            block.prevBlockId = lastBlock.id;

            binary2.write((const char*)(&block), sizeof(block));

        }
        else {

           
            std::size_t numStructs = binary2.tellg() / sizeof(TransactionBlock);

            binary2.seekg((numStructs - 1) * sizeof(TransactionBlock), std::ios::beg);


            binary2.read((char*)(&block), sizeof(block));

            block.transactions[giveBlockTransactionArrIndex()] = transaction;


            numStructs = binary2.tellg() / sizeof(TransactionBlock);

            binary2.seekg((numStructs - 1) * sizeof(TransactionBlock), std::ios::beg);


            binary2.write((char*)(&block), sizeof(block));
        }
        mentionTransaction(transaction, true);

        binary2.close();
    }

}

void createUser(const char* commandTo) {

    std::fstream binary("users.dat", std::ios::binary | std::ios::in | std::ios::out /*| std::ios::app if you uncomment this it'll not work*/);

    if (!binary) {

        std::cout << "The User file does not exist. Trying to create one..." << '\n';
        binary.clear();

        binary.open("users.dat", std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
        if (!binary.is_open()) {
            std::cout << "Cant create the file!!" << '\n';
        }

        //creating the admin user with id = 0 
        User admin;
        admin.id = 0;
        stringCopy("admin", admin.name);


        //making him first
        binary.seekg(0, std::ios::beg);
        binary.write(reinterpret_cast<const char*>(&admin), sizeof(User));


        //creating the wanted user
        User user;
        stringCopy(commandTo, user.name);
        user.id = randomSixDigitGenerator();
        binary.write((const char*)&user, sizeof(User));

        //making the default transaction from admin to created user
        const double DEFAULT_AMOUNT_TRANSACTION = 1500;

        const unsigned ADMIN_ID = 0;

        createTransaction(ADMIN_ID, user.id, DEFAULT_AMOUNT_TRANSACTION);

        std::cout << "Created User with a name: " << user.name << " " << ", user id: " << user.id << '\n';

        binary.close();
    }

    else {

        binary.close();

        if (!checkIfUserExists(commandTo)) {


            std::fstream binary2;
            binary2.open("users.dat", std::ios::binary | std::ios::app | std::ios::in | std::ios::out);

            if (!binary2.is_open()) {
                std::cout << "Open error" << '\n';
            }

            User user;

            stringCopy(commandTo, user.name);

            user.id = randomSixDigitGenerator();

            binary2.write(reinterpret_cast<const char*>(&user), sizeof(user));

            std::cout << "Created a user with a name: " << user.name << " " << " and id: " << user.id << '\n';
            const double DEFAULT_AMOUNT_TRANSACTION = 1500;

            const unsigned ADMIN_ID = 0;
            createTransaction(ADMIN_ID, user.id, DEFAULT_AMOUNT_TRANSACTION);

            binary2.close();
        }
        else {
            std::cout << '\n' << "User already exists!!!" << '\n' << '\n';
        }
    }

}

void removeUser(const char* userToRemove) {
    std::fstream binary("users.dat", std::ios::binary | std::ios::in | std::ios::out);
    User user;

    while (binary.read(reinterpret_cast<char*>(& user), sizeof(User))) {
        if (strCompare(user.name, userToRemove)) {
            break;
        }
    }

    if (user.id == 0) {
        std::cout << "YOU CAN'T REMOVE THE SYS_USER!!!" << '\n';
        return;
    }
    if (ifUserDeletedChecker(user.id) || !checkIfUserExists(userToRemove)) {
        std::cout << "User " << userToRemove << " doesn't exist!!!" << '\n' << '\n';
        return;
    }
    std::cout << "Removing user with a name: " << user.name << "\n" << "User id: " << user.id << '\n';

    user.id = 1; // = 1 => user is deleted and from now he is gonna be ignored



    binary.seekg((binary.tellg() / sizeof(User) - 1) * sizeof(User), std::ios::beg);

    binary.write(reinterpret_cast<const char*>(&user), sizeof(User));

    binary.seekp((binary.tellg() / sizeof(User) - 1) * sizeof(User), std::ios::beg);
    binary.read(reinterpret_cast<char*>(&user), sizeof(User));

    //transfer all the money to admin
    createTransaction(user.id, 0, checkUserAmount(user.id));


    if (ifUserDeletedChecker(user.id)) {
        std::cout << "Sucsessfully removed" << '\n';
    }
    else {
        std::cout << '\n' << "Smth went wrong with removing the user" << '\n';
    }
    binary.close();
}

void seperateSenderFromReciever(const char* sendFromTo, char* from, char* to) {
    int i;
    int secIndex = 0;
    for (i = 0; sendFromTo[i] != ' ' && i < 64; i++) {
        from[i] = sendFromTo[i];
    }

    //nothing in sendFromTo arr
    if (i == 64) {
        stringCopy("no_name?!", from);
        stringCopy("no_name?!", to);

        return;
    }
    from[i] = '\0';
    i++;
    for (i; sendFromTo[i] != '\0'; secIndex++, i++) {
        to[secIndex] = sendFromTo[i];
    }
    to[secIndex] = '\0';
}

bool checkIfUserExists(const char* userName) {
    std::ifstream binary;
    binary.open("users.dat", std::ios::binary);

    if (!binary.is_open()) {
        std::cout << "OPen error";
    }

    User user;

    while (binary.read((char*)&user, sizeof(User))) {

        if (strCompare(user.name, userName) && user.id != 1) {
            return true;
        }
    }

    binary.close();
    return false;
}

int getUserId(const char* userName) {

    std::ifstream stream;
    stream.open("users.dat", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    User user;
    while (stream.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        if (strCompare(user.name, userName)) {
            stream.close();
            return user.id;
        }
    }
    std::cout << '\n' << "Smth went wrong with getting the ID of a user!!" << '\n';
    stream.close();

    return -1;
}

void getUserName(const int ID, char* userName) {

    std::ifstream stream;
    stream.open("users.dat", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    User user;
    while (stream.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        if (user.id == ID) {
            stringCopy(user.name, userName);
            stream.close();
            return;
        }
    }
    std::cout << '\n' << "SMth went wrong with getting the user name!!" << '\n';
    stream.close();

    return;
}

double checkUserAmount(const char* userName) {

    int userID = getUserId(userName);

    if (userID == 0) {
        return 99999.9;
    }

    std::ifstream stream;
    stream.open("transactions.dat", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    Transaction transaction;
    double userAmount = 0;
    while (stream.read(reinterpret_cast<char*>(&transaction), sizeof(Transaction))) {
        if (transaction.receiver == userID) {
            userAmount += transaction.coins;
        }
        else if (transaction.sender == userID) {
            userAmount -= transaction.coins;
        }
    }
    stream.close();
    return userAmount;
}

bool ifUserDeletedChecker(const unsigned id) {
    return (id == 1) ? true : false;
}

double checkUserAmount(const int userID) {

    std::ifstream stream;
    stream.open("transactions.dat", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    if (userID == 0) {
        stream.close();
        return 99999.9;
    }
    else {
        Transaction transaction;
        double userAmount = 0.0;
        while (stream.read(reinterpret_cast<char*>(&transaction), sizeof(Transaction))) {
            if (transaction.receiver == userID) {
                userAmount += transaction.coins;
            }
            else if (transaction.sender == userID) {
                userAmount -= transaction.coins;
            }
        }
        stream.close();
        return userAmount;
    }
    return -1;
}

void sendCoins(const char* sendFromTO) {
    char coinsFrom[64];
    char coinsTo[64];

    seperateSenderFromReciever(sendFromTO, coinsFrom, coinsTo);


    if (strCompare("no_name?!", coinsFrom)) {
        std::cout << '\n' << "You should specify the sender and the reciver like this: [send-coins <sender> <reciver>]" << '\n' << '\n';
        return;
    }
    if (!checkIfUserExists(coinsFrom)) {
        std::cout << "User " << coinsFrom << " doesn't exist" << '\n';
        return;
    }
    if (!checkIfUserExists(coinsTo)) {
        std::cout << "User " << coinsTo << " doesn't exist!!!" << '\n';
        return;
    }


    double amount;
    std::cout << '\n' << "Amount: ";
    std::cin >> amount;
    std::cout << '\n';

    if (amount < 0) {
        std::cout << "Can't transafer negative values!!!" << "\n" << "\n";
        return;
    }

    double senderAmount = checkUserAmount(coinsFrom);

    if (senderAmount < amount) {
        std::cout << "Sender has only " << senderAmount << " OOPCoins which are not enogh!!!" << '\n';
        return;
    }

    createTransaction(getUserId(coinsFrom), getUserId(coinsTo), amount);

}

void checkTransactions() {
    std::ifstream binary;

    binary.open("blocks.dat");

    if (!binary.is_open()) {
        std::cout << "Open error of blocks.bin file!!!" << '\n';
    }

    TransactionBlock block;

    //read the first block with no prevHassh
    binary.read(reinterpret_cast<char*>(&block), sizeof(TransactionBlock));

    unsigned prevBlockHash = computeHash((const unsigned char*)(&block), sizeof(block));
    unsigned hash;

    while (binary.read(reinterpret_cast<char*>(&block), sizeof(TransactionBlock))) {

        hash = block.prevBlockHash;

        if (prevBlockHash != hash) {
            std::cout << '\n' << "Transaction fault. There is a corupted transaction!!!" << '\n';
            binary.close();
            return;
        }
        prevBlockHash = computeHash((const unsigned char*)(&block), sizeof(block));
    }

    std::cout << '\n' << "All transactions have connection!!!" << '\n' << '\n';
    binary.close();
}

bool userIdExistsInArr(const int userID, const int users[1024]) {
    for (int i = 0; i < 1024; i++) {
        if (users[i] == userID) {
            return true;
        }
    }
    return false;
}

void readTransactionInfo() {
    std::ifstream stream;
    stream.open("transactions.dat", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Open file fault";
    }

    Transaction trans;
    while (stream.read(reinterpret_cast<char*>(&trans), sizeof(Transaction))) {
        std::cout << '\n' << "Transaction sender: " << trans.sender << ". Transaction reciver: " << trans.receiver << '\n';
    }
    stream.close();
}


void sortUsers(int users[1024], const int length) {
    int temp;
    for (int i = 0; i < length; i++) {
        for (int j = i; j < length; j++)
        {
            if (checkUserAmount(users[j]) > checkUserAmount(users[i])) {
                temp = users[j];
                users[j] = users[i];
                users[i] = temp;
            }
        }
    }
}

User getUser(const int id) {
    std::ifstream stream ("users.dat", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Open error";
    }
    User user;
    while (stream.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        if (user.id == id) {
            stream.close();
            return user;
        }
    }
    std::cout << "Invalid ID";
    stream.close();
    return user;
}

void numbersIntoText(long long num, char* timeArr) {
    for (int i = 10 - 1; i >= 0; i--) {
        timeArr[i] = num % 10 + 48;
        num /= 10;
    }
    timeArr[10] = '\0';
}

void createFileName(char* resultArr) {
    long long num = secondsSince1970();

    // 10(num-s in a billion) + 1('\0') = 11  

    char timeArr[11];
    numbersIntoText(num, timeArr);
    strcat(resultArr, timeArr);
    strcat(resultArr, ".txt");
}

void printSaveWealthiestUsers(const int* userIDs, const int numUsers) {

    char fileName[16 + 1 + 10 + 4 + 1] = "wealthiest-users_";
    createFileName(fileName);

    std::ofstream stream(fileName);

    char userNamse[32];
    for (size_t i = 0; i < numUsers; i++) {
        if (userIDs[i] != 1)
        {
            getUserName(userIDs[i], userNamse);
            std::cout << '\n' << i + 1 << ". User name: " << userNamse << ", with id: " << userIDs[i] << ", and wealth: " << checkUserAmount(userIDs[i]) << '\n';
            stream << '\n' << i + 1 << ". User name: " << userNamse << ", with id: " << userIDs[i] << ", and wealth: " << checkUserAmount(userIDs[i]) << '\n';

        }
    }


    User usr;
    int countTheDeletedUsers = 0;
    for (size_t i = 0; i < numUsers; i++) {
        if (ifUserDeletedChecker(userIDs[i])) {
            countTheDeletedUsers++;
        }
    }
    std::cout << '\n' << "And there are " << countTheDeletedUsers << " deleted users" << '\n' << '\n';
    stream << '\n' << "And there are " << countTheDeletedUsers << " deleted users" << '\n' << '\n';

    std::cout << '\n' << "Generated a file: " << fileName << '\n' << '\n';

    stream.close();
}

void computeWealthiestUsers() {

    int numUsers;
    std::cout << '\n' << "Enter how big of a list of wealthiest users you want: ";

    std::cin >> numUsers;
    int* userWealth = new int[numUsers];

    User user;

    std::ifstream stream;
    stream.open("users.dat", std::ios::binary);

    int userIndex = 0;
    int users[1024];

    while (stream.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        users[userIndex] = user.id;

        ++userIndex;
    }

    if (numUsers > userIndex) {
        std::cout << '\n' << "The users are only " << userIndex << '\n';
        numUsers = userIndex;
    }

    sortUsers(users, userIndex);

    printSaveWealthiestUsers(users, numUsers);


    std::cin.ignore();
    stream.close();
}

short computeBlockArrSize(const int blockCounter) {
    std::ifstream binary;

    binary.open("transactions.dat", std::ios::binary | std::ios::in);

    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    Transaction transaction;
    int blockSize = 0;

    const unsigned numOfTransactionInBlock = 16;


    if (blockCounter == 1)
    {
        while (binary.read(reinterpret_cast<char*>(&transaction), sizeof(Transaction)) && blockSize < numOfTransactionInBlock) {
            ++blockSize;
        }
    }

    else {

        binary.seekg((blockCounter - 1) * numOfTransactionInBlock * sizeof(Transaction));
        while (binary.read(reinterpret_cast<char*>(&transaction), sizeof(Transaction)) && blockSize < numOfTransactionInBlock) {
            ++blockSize;
        }
    }

    binary.close();
    return blockSize;
}

int giveBlockTransactionArrIndex(const int blockID) {

    std::ifstream binary;

    binary.open("blocks.dat", std::ios::binary | std::ios::in);

    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    binary.seekg(binary.beg);

    int blockCounter = 0;

    TransactionBlock block;

    while (binary.read(reinterpret_cast<char*>(&block), sizeof(TransactionBlock))) {
        if (block.id == blockID) {
            binary.close();
            blockCounter++;

            return computeBlockArrSize(blockCounter);
        }
        blockCounter++;
    }

    binary.close();

    return -1;
}

double checkBlockAmount(const int blockID) {

    std::ifstream stream;

    stream.open("blocks.dat", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Opem error!!!";
    }

    TransactionBlock block;

    double blockAmount = 0;
    while (stream.read(reinterpret_cast<char*>(&block), sizeof(TransactionBlock))) {
        if (block.id == blockID) {
            //compute block size
            int blockTransactionArrIndex = giveBlockTransactionArrIndex(blockID);
            for (size_t i = 0; i < blockTransactionArrIndex; i++)
            {
                blockAmount += block.transactions[i].coins;
            }

        }
    }
    stream.close();
    return blockAmount;
}

void sortBlocks(int blocks[1024], const int length) {
    int temp;
    double blockAmount1;
    double blockAmount2;
    for (int i = 0; i < length; i++) {
        for (int j = i; j < length; j++)
        {
            blockAmount1 = checkBlockAmount(blocks[j]);
            blockAmount2 = checkBlockAmount(blocks[i]);
            if (blockAmount1 > blockAmount2) {
                temp = blocks[j];
                blocks[j] = blocks[i];
                blocks[i] = temp;
            }
        }

        std::cout << '\n' << i + 1 << ". Block id: " << blocks[i] << ", with amount: " << checkBlockAmount(blocks[i]) << '\n';

    }
}

void computeBiggestBlocks() {
    int numBlocks;
    std::cout << '\n' << "Enter how big of a list of blocks you want: ";

    std::cin >> numBlocks;


    TransactionBlock block;

    std::ifstream stream;
    stream.open("blocks.dat", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Open error!!!";
    }

    int blockIndex = 0;
    int blocks[1024];

    while (stream.read(reinterpret_cast<char*>(&block), sizeof(block))) {
        blocks[blockIndex] = block.id;

        ++blockIndex;
    }

    if (numBlocks > blockIndex) {
        std::cout << '\n' << "The blocks are only " << blockIndex << '\n';
        numBlocks = blockIndex;
    }

    sortBlocks(blocks, blockIndex);

    std::cout << '\n';

    std::cin.ignore();
    stream.close();
}


void commandSeperator(const char* enteredCommand, char* commandFor, char* commandTo) {

    if (strCompare(enteredCommand, "exit")) {
        commandFor[0] = '\0';
        commandTo[0] = '\0';
        return;
    }
    if (strCompare(enteredCommand, "")) {
        commandFor[0] = '\0';
        commandTo[0] = '\0';
        return;
    }
    if (strCompare(enteredCommand, "verify-transactions")) {
        stringCopy("verify-transactions", commandFor);

        return;
    }
    if (strCompare(enteredCommand, "wealthiest-users")) {
        stringCopy("wealthiest-users", commandFor);

        return;
    }
    if (strCompare(enteredCommand, "biggest-blocks")) {
        stringCopy("biggest-blocks", commandFor);

        return;
    }
    if (strCompare(enteredCommand, "help")) {
        stringCopy("help", commandFor);

        return;
    }

    size_t index = 0;

    for (index; enteredCommand[index] != ' ' && enteredCommand[index] != '\0'; ++index) {
        commandFor[index] = enteredCommand[index];
    }

    int j = index;

    if (enteredCommand[j + 1] == ' ' || enteredCommand[j] == '\0') {
        std::cout << '\n' << "Invalid command!!!" << '\n' << '\n';
        commandFor[0] = '\0';
        commandTo[0] = '\0';
        return;
    }

    commandFor[index] = '\0';
    ++index;

    size_t i;
    for (i = 0; enteredCommand[index] != '\0'; ++index, ++i) {

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
    for (i = 0; fromStr[i] != '\0'; i++) {
        toStr[i] = fromStr[i];
    }
    toStr[i] = '\0';
}


void printHelp() {
    std::cout << std::endl << "Available commands:" << std::endl << std::endl;
    std::cout << "  create-user <username> - creates a new user with the specified username" << std::endl;
    std::cout << "  remove-user <username> - removes the user with the specified username" << std::endl;
    std::cout << "  send-coins <send-from send-to> - sends coins from a user to a user" << std::endl;
    std::cout << "  verify-transactions - verifies that all transactions have connections through the hash's" << std::endl;
    std::cout << "  wealthiest-users - computes and prints the wealthiest users" << std::endl;
    std::cout << "  biggest-blocks - computes and prints the biggest blocks" << std::endl;
    std::cout << "  help - displays this help message" << std::endl;
    std::cout << "  exit - exits the program" << std::endl << std::endl;

}

void welcomeMessage() {

    std::cout << "               ***WELCOME TO OOPCoin***" << std::endl << std::endl;

    std::cout << "You can manipulate the people's money with these commands ->" << std::endl << std::endl;

    printHelp();

}
void runCommand(const char* commandFor, const char* commandTo) {

    if (strCompare(commandFor, "create-user")) {
        createUser(commandTo);
    }
    else if (strCompare(commandFor, "remove-user")) {
        removeUser(commandTo);
    }
    else if (strCompare(commandFor, "send-coins")) {
        sendCoins(commandTo);
        std::cin.ignore();
    }
    else if (strCompare(commandFor, "verify-transactions")) {
        checkTransactions();
    }
    else if (strCompare(commandFor, "wealthiest-users")) {
        computeWealthiestUsers();
    }
    else if (strCompare(commandFor, "biggest-blocks")) {
        computeBiggestBlocks();
    }
    else if (strCompare(commandFor, "help")) {
        printHelp();
    }
    else if (strCompare(commandFor, "exit") || strCompare(commandFor, "")) {
        return;
    }
    else {
        std::cout << '\n' << "Invalid command!!!" << '\n' << '\n';
    }
}

void run() {

    char enteredCommand[64];

    do {
        std::cout << "- ";
        std::cin.getline(enteredCommand, 64);

        char commandFor[32];
        char  commandTo[64];

        commandSeperator(enteredCommand, commandFor, commandTo);
        runCommand(commandFor, commandTo);

    } while (!strCompare(enteredCommand, "exit"));
}

int main() {

    welcomeMessage();
    run();

    return 0;
}
