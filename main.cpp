#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <string>
#include <dlfcn.h>
#include <stdexcept>
#include <random>

class StringArray {
private:
    std::vector<std::string> array;
    std::stack<std::vector<std::string>> historyStack;
    std::stack<std::vector<std::string>> redoStack;
    int consecutiveUndoCount;
    std::string clipboard;

public:
    StringArray() : consecutiveUndoCount(0) {
        historyStack.push(array);
    }

    std::vector<std::string> getStrings() const {
        return array;
    }

    void setStrings(const std::vector<std::string>& data) {
        array = data;
    }

    size_t getStringCount() const {
        return array.size();
    }

    void addString(const std::string& buffer) {
        if (!array.empty()) {
            array.back() += buffer;
        } else {
            array.push_back(buffer);
        }
        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }

    void addEmptyLine() {
        array.push_back("");
        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }

    void printStrings() {
        for (size_t i = 0; i < array.size(); i++) {
            std::cout << i + 1 << ": " << array[i] << std::endl;
        }
    }

    void deleteSubstring(int lineIndex, int position, int length) {
        if (lineIndex < 1 || static_cast<size_t>(lineIndex) > array.size()) {
            std::cerr << "Invalid line index." << std::endl;
            return;
        }

        std::string& line = array[lineIndex - 1];

        if (position < 0 || static_cast<size_t>(position) >= line.length()) {
            std::cerr << "Invalid position." << std::endl;
            return;
        }

        if (length < 0 || static_cast<size_t>(position + length) > line.length()) {
            std::cerr << "Invalid length." << std::endl;
            return;
        }

        clipboard = line.substr(position, length);
        line.erase(position, length);
        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }

    void undo() {
        if (historyStack.size() > 1 && consecutiveUndoCount < 3) {
            redoStack.push(array);
            historyStack.pop();
            array = historyStack.top();
            consecutiveUndoCount++;
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            historyStack.push(array);
            array = redoStack.top();
            redoStack.pop();
            consecutiveUndoCount = 0;
        }
    }

    void insertSubstring(int lineIndex, int position, const std::string& substring, bool replace = false) {
        if (lineIndex < 1 || static_cast<size_t>(lineIndex) > array.size()) {
            std::cerr << "Invalid line index." << std::endl;
            return;
        }

        if (position < 0 || static_cast<size_t>(position) > array[lineIndex - 1].length()) {
            std::cerr << "Invalid position." << std::endl;
            return;
        }

        if (replace) {
            int length = substring.length();
            array[lineIndex - 1].erase(position, length);
            array[lineIndex - 1].insert(position, substring);
        } else {
            array[lineIndex - 1].insert(position, substring);
        }

        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }

    void cut(int lineIndex, int position, int length) {
        if (lineIndex < 1 || static_cast<size_t>(lineIndex) > array.size()) {
            std::cerr << "Invalid line index." << std::endl;
            return;
        }

        std::string& line = array[lineIndex - 1];

        if (position < 0 || static_cast<size_t>(position) >= line.length()) {
            std::cerr << "Invalid position." << std::endl;
            return;
        }

        if (length < 0 || static_cast<size_t>(position + length) > line.length()) {
            std::cerr << "Invalid length." << std::endl;
            return;
        }

        clipboard = line.substr(position, length);
        line.erase(position, length);
        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }

    void copy(int lineIndex, int position, int length) {
        if (lineIndex < 1 || static_cast<size_t>(lineIndex) > array.size()) {
            std::cerr << "Invalid line index." << std::endl;
            return;
        }

        const std::string& line = array[lineIndex - 1];

        if (position < 0 || static_cast<size_t>(position) >= line.length()) {
            std::cerr << "Invalid position." << std::endl;
            return;
        }

        if (length < 0 || static_cast<size_t>(position + length) > line.length()) {
            std::cerr << "Invalid length." << std::endl;
            return;
        }

        clipboard = line.substr(position, length);
    }

    void paste(int lineIndex, int position) {
        if (lineIndex < 1 || static_cast<size_t>(lineIndex) > array.size()) {
            std::cerr << "Invalid line index." << std::endl;
            return;
        }

        if (position < 0 || static_cast<size_t>(position) > array[lineIndex - 1].length()) {
            std::cerr << "Invalid position." << std::endl;
            return;
        }

        array[lineIndex - 1].insert(position, clipboard);
        historyStack.push(array);
        redoStack = std::stack<std::vector<std::string>>();
        consecutiveUndoCount = 0;
    }
};

class SearchFunctions {
public:
    static void searchSubstringInArray(const std::vector<std::string>& array, const std::string& substring) {
        int foundCount = 0;

        for (size_t i = 0; i < array.size(); i++) {
            size_t found = array[i].find(substring);
            if (found != std::string::npos) {
                std::cout << "Substring found in line " << i + 1 << " at position " << found << ": " << substring << std::endl;
                foundCount++;
            }
        }

        if (foundCount == 0) {
            std::cout << "Substring not found in any line." << std::endl;
        }
    }
};

class FilesSL {
public:
    static void saveToFile(const std::string& fileName, const std::vector<std::string>& data) {
        std::ofstream file(fileName);
        if (file.is_open()) {
            for (const std::string& line : data) {
                file << line << std::endl;
            }
            file.close();
            std::cout << "Array saved to " << fileName << std::endl;
        } else {
            std::cerr << "Error opening the file." << std::endl;
        }
    }

    static std::vector<std::string> loadFromFile(const std::string& fileName) {
        std::vector<std::string> loadedData;
        std::ifstream file(fileName);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                loadedData.push_back(line);
            }
            file.close();
            std::cout << "Array loaded from " << fileName << std::endl;
        } else {
            std::cerr << "Error opening the file." << std::endl;
        }
        return loadedData;
    }
};

class IReader {
public:
    virtual ~IReader() {}
    virtual std::string Read(const std::string& filePath) = 0;
};

class FileReader : public IReader {
public:
    std::string Read(const std::string& filePath) override {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("File not found.");
        }

        std::string content;
        char buffer[128];
        while (!file.eof()) {
            file.read(buffer, sizeof(buffer));
            content.append(buffer, file.gcount());
        }
        return content;
    }
};

class IWriter {
public:
    virtual ~IWriter() {}
    virtual void Write(const std::string& filePath, const std::string& content) = 0;
};

class FileWriter : public IWriter {
public:
    void Write(const std::string& filePath, const std::string& content) override {
        std::ifstream file(filePath);
        if (file.good()) {
            throw std::runtime_error("File already exists.");
        }

        std::ofstream output(filePath, std::ios::binary);

        if (!output.is_open()) {
            throw std::runtime_error("Unable to open the file for writing.");
        }

        const size_t chunkSize = 128;
        for (size_t i = 0; i < content.size(); i += chunkSize) {
            size_t chunkEnd = std::min(i + chunkSize, content.size());
            output.write(content.c_str() + i, chunkEnd - i);
        }
    }
};

class EncryptionLibrary {
public:
    EncryptionLibrary(const std::string &libraryPath) {
        library_ = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!library_) {
            std::cerr << "Failed to load the library: " << dlerror() << std::endl;
            return;
        }

        encrypt_ = (EncryptFunction)dlsym(library_, "encrypt");
        decrypt_ = (DecryptFunction)dlsym(library_, "decrypt");

        if (!encrypt_ || !decrypt_) {
            std::cerr << "Failed to load functions: " << dlerror() << std::endl;
        }
    }

    ~EncryptionLibrary() {
        if (library_) {
            dlclose(library_);
        }
    }

    std::string encrypt(const std::string &inputText, const std::string &keyString) {
        if (encrypt_) {
            return encrypt_(inputText, keyString);
        } else {
            return "Encryption function not loaded";
        }
    }

    std::string decrypt(const std::string &inputText, const std::string &keyString) {
        if (decrypt_) {
            return decrypt_(inputText, keyString);
        } else {
            return "Decryption function not loaded";
        }
    }

private:
    void *library_;
    typedef std::string (*EncryptFunction)(const std::string &inputText, const std::string &keyString);
    typedef std::string (*DecryptFunction)(const std::string &inputText, const std::string &keyString);
    EncryptFunction encrypt_;
    DecryptFunction decrypt_;
};

class RandomKeyGenerator {
public:
    RandomKeyGenerator() {
        std::random_device rd;
        generator_ = std::mt19937(rd());
        distribution_ = std::uniform_int_distribution<int>(1, 100);
    }

    int GenerateRandomKey() {
        return distribution_(generator_);
    }

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<int> distribution_;
};

class EncryptionHandler {
public:
    EncryptionHandler(const char* libraryPath) : encryptionLibrary(libraryPath) {}

    void process() {
        int mode = 0;
        std::cout << "Choose mode: 1 for Normal mode, 2 for Secret mode: ";
        std::cin >> mode;

        std::string inputFilePath, outputFilePath, key;

        // Normal mode
        if (mode == 1) {
            std::string operation;
            std::cout << "Choose operation: 'encrypt' or 'decrypt': ";
            std::cin >> operation;

            std::cout << "Enter input file path: ";
            std::cin >> inputFilePath;

            std::cout << "Enter output file path: ";
            std::cin >> outputFilePath;

            std::cout << "Enter the key: ";
            std::cin >> key;

            try {
                IReader *reader = new FileReader();
                IWriter *writer = new FileWriter();
                std::string content = reader->Read(inputFilePath);

                if (operation == "encrypt") {
                    content = encryptionLibrary.encrypt(content, key);
                } else if (operation == "decrypt") {
                    content = encryptionLibrary.decrypt(content, key);
                } else {
                    std::cerr << "Invalid operation. Please choose 'encrypt' or 'decrypt'." << std::endl;
                }

                writer->Write(outputFilePath, content);
                std::cout << "Operation completed successfully." << std::endl;

                delete reader;
                delete writer;
                encryptionLibrary.~EncryptionLibrary();
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        // Secret mode
        } else if (mode == 2) {
            RandomKeyGenerator keyGenerator;

            std::cout << "Enter input file path: ";
            std::cin >> inputFilePath;

            std::cout << "Enter output file path: ";
            std::cin >> outputFilePath;

            try {
                IReader *reader = new FileReader();
                IWriter *writer = new FileWriter();
                std::string content = reader->Read(inputFilePath);

                int randomKey = keyGenerator.GenerateRandomKey();

                content = encryptionLibrary.encrypt(content, std::to_string(randomKey));

                writer->Write(outputFilePath, content);
                std::cout << "Operation completed successfully." << std::endl;

                delete reader;
                delete writer;
                encryptionLibrary.~EncryptionLibrary();
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

private:
    EncryptionLibrary encryptionLibrary;
};

class Processes{
private:
    StringArray stringArray;
    std::string fileName;
public:
    void append(){
        std::string buffer;
        std::cout << "Write text to append: ";
        std::getline(std::cin, buffer);
        stringArray.addString(buffer);
    }

    void emptyLine(){
        stringArray.addEmptyLine();
    }

    void show(){
        stringArray.printStrings();
    }

    void save(){
        std::cout << "Write file name to SAVE: ";
        std::cin >> fileName;
        FilesSL::saveToFile(fileName, stringArray.getStrings());
    }

    void load(){
        std::cout << "Write file name to LOAD: ";
        std::cin >> fileName;
        stringArray.setStrings(FilesSL::loadFromFile(fileName));
    }

    void search(){
        std::string substring;

        std::cout << "Enter substring to search for: ";
        std::cin >> substring;

        SearchFunctions::searchSubstringInArray(stringArray.getStrings(), substring);
    }

    void insert(){
        int lineIndex, position;
        std::string substring;
        bool replaceMode;

        std::cout << "Enter line index for insertion: ";
        std::cin >> lineIndex;

        std::cout << "Enter position for insertion (0-" << stringArray.getStringCount() << "): ";
        std::cin >> position;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Enter substring to insert: ";
        std::getline(std::cin, substring);

        std::cout << "Replace existing text (1 for yes, 0 for no): ";
        std::cin >> replaceMode;

        stringArray.insertSubstring(lineIndex, position, substring, replaceMode);
    }

    void dalete(){
        int lineIndex, position, length;

        std::cout << "Choose line, index, and number of symbols to delete: ";
        std::cin >> lineIndex >> position >> length;
        stringArray.deleteSubstring(lineIndex, position, length);
    }

    void undo(){
        stringArray.undo();
    }

    void redo(){
        stringArray.undo();
    }

    void cut(){
        int cutLine, cutPos, cutLen;
        std::cout << "Choose line, position, and length to cut: ";
        std::cin >> cutLine >> cutPos >> cutLen;
        stringArray.cut(cutLine, cutPos, cutLen);
    }

    void copy(){
        int copyLine, copyPos, copyLen;
        std::cout << "Choose line, position, and length to copy: ";
        std::cin >> copyLine >> copyPos >> copyLen;
        stringArray.copy(copyLine, copyPos, copyLen);
    }

    void paste(){
        int pasteLine, pastePos;
        std::cout << "Choose line and position to paste: ";
        std::cin >> pasteLine >> pastePos;
        stringArray.paste(pasteLine, pastePos);
    }

    void encryptor(){
        const char* relativePath = "./encryption.dylib";
        EncryptionHandler handler(relativePath);
        handler.process();
    }
};

int main() {
    Processes processes;

    int command = 0;
    StringArray stringArray;
    std::string fileName;
    std::cout << "Commands:\n"
                 "1 - Append text\n"
                 "2 - Add empty line\n"
                 "3 - Print all text\n"
                 "4 - Save to file\n"
                 "5 - Load from file\n"
                 "6 - Search\n"
                 "7 - Insert\n"
                 "8 - Delete\n"
                 "9 - Undo\n"
                 "10 - Redo\n"
                 "11 - Cut\n"
                 "12 - Copy\n"
                 "13 - Paste\n"
                 "14 - Encryptor\n";

    while (true) {
        std::cout << "Write command 1-14: ";
        std::cin >> command;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (command) {
            case 1: {
                processes.append();

                break;
            }
            case 2: {
                processes.emptyLine();

                break;
            }
            case 3: {
                processes.show();

                break;
            }
            case 4: {
                processes.save();

                break;
            }
            case 5: {
                processes.load();

                break;
            }
            case 6: {
                processes.search();

                break;
            }
            case 7: {
                processes.insert();

                break;
            }
            case 8: {
                processes.dalete();

                break;
            }
            case 9: {
                processes.undo();

                break;
            }
            case 10: {
                processes.redo();

                break;
            }
            case 11: {
                processes.cut();

                break;
            }
            case 12: {
                processes.copy();

                break;
            }
            case 13: {
                processes.paste();

                break;
            }
            case 14: {
                processes.encryptor();

                break;
            }
            default: {
                if (command < 0 || command > 14) {
                    std::cout << "The command is not implemented." << std::endl;
                }
                break;
            }
        }
    }
}