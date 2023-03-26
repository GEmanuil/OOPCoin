#include <iostream>
#include <fstream>
#include <cstring>
#include <random>
#include <chrono>
#include <time.h>

//commands
const char* EXIT = "exit";
const char* CREATE_USER = "create-user";
const char* REMOVE_USER = "remove-user";
const char* SEND_COINS = "send-coins";
const char* VERIFY_TRANSACTIONS = "verify-transactions";
const char* WEALTHIEST_USERS = "wealthiest-users";



//def values
const char* ADMIN = "admin";
double DEFAULT_AMOUNT_TRANSACTION = 1500;
unsigned ADMIN_ID = 0;
const unsigned numOfTransactionInBlock = 3;

using namespace std::chrono;

long long secondsSince1970();
int randomSixDigitGenerator();
void commandSeperator(const char* enteredCommand, char* commandFor, char* commandTo);
bool strCompare(const char* arr1, const char* arr2);
void stringCopy(const char* fromStr, char* toStr);


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
    Transaction transactions[numOfTransactionInBlock];
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

    //int result = 0;
    //
    ////if rand() returns zero cycle it until it returns something else
    //while (result == 0) {
    //    result = rand() % 10;
    //}

    //int var = 0;

    //for (int i = 0; i < 5; i++) {

    //    //time(0) + i -> because if not (without "+ i") it will generate only 111111 or 222222 or ... or 999999

    //    srand(time(nullptr) + i);
    //    var = rand() % 10;

    //    result *= 10;
    //    result += var;
    //}


      // Seed the random number generator with the current time
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    int random_number = dis(gen);

    std::cout << '\n' << '\n' << "Generated random num: " << random_number << '\n' << '\n' << '\n';

    return random_number;
}

void commandSeperator(const char* enteredCommand, char* commandFor, char* commandTo) {

    if (strCompare(enteredCommand, EXIT)) {
        return;
    }
    if (strCompare(enteredCommand, "")) {
        return;
    }
    if (strCompare(enteredCommand, VERIFY_TRANSACTIONS)) {
        stringCopy(VERIFY_TRANSACTIONS, commandFor);
        return;
    }
    if (strCompare(enteredCommand, WEALTHIEST_USERS)) {
        stringCopy(WEALTHIEST_USERS, commandFor);
        return;
    }
    if (strCompare(enteredCommand, "biggest-blocks")) {
        stringCopy("biggest-blocks", commandFor);
        return;
    }

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

short giveBlockTransactionArrIndex() {

    std::ifstream binary;

    binary.open("transactions.bin", std::ios::binary | std::ios::in);
    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    binary.seekg(binary.beg);
    //TODO change type
    int transactionCounter = 0;

    Transaction transaction;

    while (binary.read((char*)&transaction, sizeof(Transaction))) {
        transactionCounter++;
    }

    binary.close();

    return (transactionCounter % numOfTransactionInBlock);
}

void mentionTransaction(Transaction& transaction, bool fileExists) {

    std::ofstream binary;


    if (fileExists) {
        binary.open("transactions.bin", std::ios::binary | std::ios::app | std::ios::out);
        if (!binary.is_open()) {
            std::cout << "Transaction file open error!!! ";
        }
        binary.write((const char*)&transaction, sizeof(Transaction));
        std::cout << "\n" << "Saved a rtansaction with a reciver id = " << transaction.receiver << "\n";

    }
    else {
        binary.open("transactions.bin", std::ios::binary | std::ios::trunc | std::ios::out);
        if (!binary.is_open()) {
            std::cout << "Transaction file open error!!! ";
        }
        binary.write((const char*)&transaction, sizeof(Transaction));
        std::cout << "Saved a rtansaction with a reciver id = " << transaction.receiver;

    }
 
    binary.close();

}

TransactionBlock givePrevTransBlock(std::fstream& stream) {
    TransactionBlock result;

    std::streampos notChangedPosition = stream.tellg();

    stream.seekg(0, std::ios::end);
    stream.seekg(stream.tellg() - static_cast<std::streampos>(sizeof(TransactionBlock)));
    stream.read(reinterpret_cast<char*>(&result), sizeof(TransactionBlock));

    stream.seekg(notChangedPosition);
    return result;
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

        Transaction transaction{ to, from, amount, secondsSince1970() };

        mentionTransaction(transaction, false);

        TransactionBlock block;
        block.id = randomSixDigitGenerator();

        std::cout << '\n' << "!!!!CREATED A TRANSACTION BOCK with ID: " << block.id << '\n';

        block.prevBlockId = 0;
        block.prevBlockHash = 0;
        block.validTransactions = 0;
        block.transactions[0] = transaction;
        
        binary.write((const char*)&block, sizeof(block));

        binary.close();
    }

    else {

        if (!binary.is_open()) {
            std::cout << "Open error" << '\n';
        }
        binary.close();

        std::fstream binary2;
        binary2.open("blocks.bin", std::ios::binary | std::ios::ate | std::ios::in | std::ios::out);

        if (!binary2.is_open()) {
            std::cout << "Open error" << '\n';
        }

        Transaction transaction{ to, from, amount, secondsSince1970() };
        TransactionBlock block;

        if (giveBlockTransactionArrIndex() == 0) {
            //binary2.seekp(binary2.cur);

            std::cout << "\n" << "\n" << "Write position in binary file!!!: " << binary2.tellp() << "\n" << "\n";

            //TODO new block
            block.id = randomSixDigitGenerator();
            block.transactions[0] = transaction;
            
            TransactionBlock lastBlock;

            binary2.seekg(0, std::ios::end);
            binary2.seekg(binary2.tellg() - static_cast<std::streampos>(sizeof(TransactionBlock)));

            binary2.read((char*)(&lastBlock), sizeof(TransactionBlock));

            block.prevBlockHash = computeHash((const unsigned char*)(&lastBlock), sizeof(lastBlock));

            std::cout << '\n' << "Previous block hash:  " << block.prevBlockHash << "\n";

            block.prevBlockId = lastBlock.id;

            binary2.write((const char*)(&block), sizeof(block));

            std::cout << "\n" << "PrevBlock HASH: " << block.prevBlockHash << '\n';


            std::cout << "\n" << "Transactions in a block: " << block.transactions[0].receiver << " " << block.transactions[1].receiver << " " << block.transactions[2].receiver << '\n';

        }
        else {

            binary2.seekg(binary2.tellg() - static_cast<std::streampos>(sizeof(TransactionBlock)));
            binary2.read((char*)(&block), sizeof(block));

            //std::cout << "\n" << "Need to be smth meaningfull 1: " << block.transactions[0].receiver << '\n';

            block.transactions[giveBlockTransactionArrIndex()] = transaction;

            binary2.seekp(binary2.tellp() - static_cast<std::streampos>(sizeof(TransactionBlock)));


            binary2.write((char*)(&block), sizeof(block));
        }
        mentionTransaction(transaction, true);

        //checking if the block correct
        binary2.seekg(binary2.tellg() - static_cast<std::streampos>(sizeof(TransactionBlock)));
        binary2.read((char*)(&block), sizeof(block));


        std::cout << "\n" << "Transactions in a block: " << block.transactions[0].receiver << " " << block.transactions[1].receiver << " " << block.transactions[2].receiver << '\n';

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

        std::cout << "The User file does not exist. Trying to create one..." << '\n';
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

        //binary.seekg(0, std::ios::beg);

        //binary.read((char*)&user, sizeof(User));

        //std::cout << "User name from file: " << user.name << "; " << "User id: " << user.id << '\n';

        //binary.read((char*)&user, sizeof(User));

        std::cout << "Created User with a name: " << user.name << "\n" << "User id: " << user.id << '\n';

        binary.close();

    }

    else {

        binary.close();


        std::fstream binary2;
        //TODO check if works with std::ios::in | std::ios::out
        binary2.open("users.bin", std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
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

        std::cout << "Created a user with a name: " << user.name << " " << " and id: " << user.id << '\n';
        createTransaction(ADMIN_ID, user.id, DEFAULT_AMOUNT_TRANSACTION);



        //binary2.read((char*)&user, sizeof(User));

        //binary2.read((char*)&user, sizeof(User));

        std::cout << "Created User with a name: " << user.name << "\n" << "User id: " << user.id << '\n';

        binary2.close();
    }

}

void removeUser(const char* userToRemove) {
    std::fstream binary("users.bin", std::ios::binary | std::ios::in | std::ios::out);
    User user;

    while (binary.read((char*)&user, sizeof(User))) {
        if (strCompare(user.name, userToRemove)) {
            break;
        }
    }

    if (user.id == 0) {
        std::cout << "YOU CAN'T REMOVE THE SYS_USER!!!" << '\n';
        return;
    }
    if (user.id == 1) {
        std::cout << "User already deleted!!!" << '\n';
        return;
    }
    std::cout << "REMOVED User with a name: " << user.name << "\n" << "User id: " << user.id << '\n';

    user.id = 1; // = 1 => user is deleted and from now he is gonna be ignored

    binary.seekg(binary.tellg() - static_cast<std::streampos>(sizeof(User)));
    //binary.seekp(binary.tellp() - static_cast<std::streampos>(sizeof(User)));

    binary.write((const char*)&user, sizeof(User));
    binary.seekp(binary.tellp() - static_cast<std::streampos>(sizeof(User)));
    binary.read((char*)&user, sizeof(User));

    //TODO transfer all the money to admin


    if (user.id  == 1) {
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
    for ( i = 0; sendFromTo[i] != ' '; i++) {
        from[i] = sendFromTo[i];
    }
    from[i] = '\0';
    i++;
    for (i; sendFromTo[i] != '\0'; secIndex++, i++) {
        to[secIndex] = sendFromTo[i];
    }
    to[secIndex] = '\0';
}

bool checkIfUserExists(char* userName) {
    std::ifstream binary;
    binary.open("users.bin", std::ios::binary);

    User user;

    while (binary.read((char*)&user, sizeof(User))) {
        
        if (strCompare(user.name, userName) && user.id != 1) {
            return true;
        }
    }

    std::cout << "User " << user.name << " doesn't exist" << '\n';
    binary.close();
    return false;
}

int getUserId(const char* userName) {

    std::ifstream stream;
    stream.open("users.bin", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    User user;
    while (stream.read((char*)&user, sizeof(User))) {
        if (strCompare(user.name, userName)) {
            stream.close();
            return user.id;
        }
    }
    std::cout << '\n' << "SMth went wrong with getting the ID of a user!!" << '\n';
    stream.close();

    return -1;
}

void getUserName(const int ID, char* userName) {

    std::ifstream stream;
    stream.open("users.bin", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    User user;
    while (stream.read((char*)&user, sizeof(User))) {
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
    stream.open("transactions.bin", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    Transaction transaction;
    double userAmount = 0;
    while (stream.read((char*)&transaction, sizeof(Transaction))) {
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

double checkUserAmount(const int userID) {

    std::ifstream stream;
    stream.open("transactions.bin", std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "File error!!!";
    }

    if (userID == 0) {
        stream.close();
        return 99999.9;
    }
    else {
        Transaction transaction;
        double userAmount = 0;
        while (stream.read((char*)&transaction, sizeof(Transaction))) {
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

}

void sendCoins(const char* sendFromTO) {
    char coinsFrom[64];
    char coinsTo[64];

    double amount;
    std::cout << '\n' << "Amount: ";
    std::cin >> amount;
    std::cout << '\n';

    seperateSenderFromReciever(sendFromTO, coinsFrom, coinsTo);
    if (!checkIfUserExists(coinsFrom) || !checkIfUserExists(coinsTo)) {
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

    binary.open("blocks.bin");

    if (!binary.is_open()) {
        std::cout << "Open error of blocks.bin file!!!" << '\n';
    }

    TransactionBlock block;

    //read the first block with no prevHassh
    binary.read((char*)&block, sizeof(TransactionBlock));

    unsigned prevBlockHash = computeHash((const unsigned char*)(&block), sizeof(block));
    unsigned hash;

    while (binary.read((char*)&block, sizeof(TransactionBlock))) {

        hash = block.prevBlockHash;

        if (prevBlockHash != hash) {
            std::cout << '\n' << "Transaction fault. There is a corupted transaction!!!" << '\n';
            binary.close();
            return;
        }
        prevBlockHash = computeHash((const unsigned char*)(&block), sizeof(block));
    }

    std::cout << '\n' << "All transactions have connection!!!" << '\n';
    binary.close();
}

bool userIdExistsInArr(int userID, int users[1024]) {
    for (int i = 0; i < 1024; i++) {
        if (users[i] == userID) {
            return true;
        }
    }
    return false;
}

void readTransactionInfo() {
    std::ifstream stream;
    stream.open("transactions.bin", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Open file fault";
    }

    Transaction trans;
    while (stream.read((char*)&trans, sizeof(Transaction))) {
        std::cout << '\n' << "Transaction sender: " << trans.sender << ". Transaction reciver: " << trans.receiver << '\n';
    }
    stream.close();
}


void sortUsers(int users[1024], int length) {
    int temp;
    for (int i = 0; i < length; i++) {
        for (int j = i; j < length; j++)
        {
            if ( checkUserAmount(users[j]) > checkUserAmount(users[i]) ) {
                temp = users[j];
                users[j] = users[i];
                users[i] = temp;
            }
        }
    }
}



void computeWealthiestUsers() {

    int numUsers;
    std::cout << '\n' << "Enter how big of a list of wealthiest users you want: ";

    std::cin >> numUsers;
    int* userWealth = new int[numUsers];


    readTransactionInfo();


    User user;

    std::ifstream stream;
    stream.open("users.bin", std::ios::binary);

    int userIndex = 0;
    int users[1024];

    while(stream.read((char*)&user, sizeof(User))) {
        users[userIndex] = user.id;

        ++userIndex;
    }

    if (numUsers > userIndex) {
        std::cout << '\n' << "The users are only " << userIndex << '\n';
        numUsers = userIndex;
    }

    sortUsers(users, userIndex);


    //TODO in a function
    //print wealthiest users
    char userNamse[32];
    for (size_t i = 0; i < numUsers; i++) {
        getUserName(users[i], userNamse);
        std::cout << '\n' << i + 1 << ". User name: " << userNamse << ", with id: " << users[i] << ", with wealth: " << checkUserAmount(users[i]) << '\n';
    }
    std::cin.ignore();
    stream.close();
}









short computeBlockArrSize(int blockCounter) {
    std::ifstream binary;

    binary.open("transactions.bin", std::ios::binary | std::ios::in);

    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    Transaction transaction;
    int blockSize = 0;


    for (size_t i = 0; binary.read((char*)&transaction, sizeof(Transaction)) ; i++)
    {
        if (i == ((blockCounter - 1) * numOfTransactionInBlock) && blockCounter != 1) {
            blockSize = 0;
        }
        blockSize++;
    }

    binary.close();
    return blockSize % numOfTransactionInBlock;
}

short giveBlockTransactionArrIndex(int blockID) {

    std::ifstream binary;

    binary.open("blocks.bin", std::ios::binary | std::ios::in);

    if (!binary.is_open()) {
        std::cout << "Open error!";
    }

    binary.seekg(binary.beg);
    //TODO change type
    int blockCounter = 0;

    TransactionBlock block;

    while (binary.read((char*)&block, sizeof(TransactionBlock))) {
        if (block.id == blockID) {
            binary.close();
            blockCounter++;

            return computeBlockArrSize(blockCounter) - 1;
        }
        blockCounter++;
    }

    binary.close();

    return -1;
}

double checkBlockAmount(int blockID) {

    std::ifstream stream;

    stream.open("blocks.bin", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Opem error!!!";
    }

    TransactionBlock block;

    double blockAmount = 0;
    while (stream.read((char*)&block, sizeof(TransactionBlock))) {
        if (block.id == blockID) {
            //compute block size
            for (size_t i = 0; i < giveBlockTransactionArrIndex(blockID) ; i++)
            {
                blockAmount += block.transactions[i].coins;
            }

        }
    }
    stream.close();
    return blockAmount;
}

void sortBlocks(int blocks[1024], int length) {
    int temp;
    double blockAmount1;
    double blockAmount2;
    for (int i = 0; i < length; i++) {
        for (int j = i; j < length; j++)
        {

            if (checkBlockAmount(blocks[j]) > checkBlockAmount(blocks[i])) {
                temp = blocks[j];
                blocks[j] = blocks[i];
                blocks[i] = temp;
            }
        }
    }
}

void computeBiggestBlocks() {
    int numBlocks;
    std::cout << '\n' << "Enter how big of a list of wealthiest users you want: ";

    std::cin >> numBlocks;


    TransactionBlock block;

    std::ifstream stream;
    stream.open("blocks.bin", std::ios::binary);

    if (!stream.is_open()) {
        std::cout << "Open error!!!";
    }

    int blockIndex = 0;
    int blocks[1024];

    while (stream.read((char*)&block, sizeof(block))) {
        blocks[blockIndex] = block.id;

        ++blockIndex;
    }

    if (numBlocks > blockIndex) {
        std::cout << '\n' << "The blocks are only " << blockIndex << '\n';
        numBlocks = blockIndex;
    }

    sortBlocks(blocks, blockIndex);


    //TODO in a function
    //print wealthiest users
    char userNamse[32];
    for (size_t i = 0; i < numBlocks; i++) {
        std::cout << '\n' << i + 1 << ". Block id: " << blocks[i] << '\n';
    }
    std::cin.ignore();
    stream.close();
}

void runCommand(char* commandFor, char* commandTo) {

    if (strCompare(commandFor, CREATE_USER)) {
        createUser(commandTo);
    }
    else if (strCompare(commandFor, REMOVE_USER)) {
        removeUser(commandTo);
    }
    else if (strCompare(commandFor, SEND_COINS)) {
        sendCoins(commandTo);
    }
    else if (strCompare(commandFor, VERIFY_TRANSACTIONS)) {
        checkTransactions();
    }
    else if (strCompare(commandFor, WEALTHIEST_USERS)) {
        computeWealthiestUsers();
    }
    else if (strCompare(commandFor, "biggest-blocks")) {
        computeBiggestBlocks();
    }
    else {
        std::cout << "Command not valid!!! " << '\n';
    }
}

void run() {

    char enteredCommand[64];

    do {
        std::cin.getline(enteredCommand, 64);

        char commandFor[32];
        char  commandTo[64];

        commandSeperator(enteredCommand, commandFor, commandTo);
        runCommand(commandFor, commandTo);

    } while (!strCompare(enteredCommand, EXIT));
}

int main() {
    
    run();

    return 0;
}