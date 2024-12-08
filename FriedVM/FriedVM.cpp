// FriedVM.cpp : Defines the entry point for the application.
//

#include "FriedVM.h"

using namespace std;

int main(int argc, char* argv[])
{
    VMInstance instance;

#ifdef DEBUG
    // In debug mode: Load from the hardcoded bytecode array
    cout << "Running in DEBUG mode: Loading predefined bytecode." << endl;
    instance.bytecode.assign(debugBytecode, debugBytecode + sizeof(debugBytecode));

#else
    // In release mode: Load bytecode from a file
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <file_path>" << endl;
        return 1;
    }

    string filePath = argv[1];
    ifstream inputFile(filePath, ios::binary);
    if (!inputFile)
    {
        cerr << "Error: Unable to open file " << filePath << endl;
        return 1;
    }

    vector<uint8_t> fileContents((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    cout << "File loaded successfully. " << fileContents.size() << " bytes read." << endl;
    instance.bytecode = fileContents;
#endif

    // Instantiate VMCore and parse the bytecode
    VMCore vmCore(instance);
    vmCore.Parse();

    return 0;
}
