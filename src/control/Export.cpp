#include "hlcontrol/internal/Export.h"

#include <fstream>
#include <string>

#include <QDir>

/**
 * Construct a new instance of the Export class.
 *
 * @param The target directory of the file
 */
Export::Export(string targetDirectory)
{
    this->targetDirectory = targetDirectory;
}

/**
 * Gets the system temporary directory
 *
 * @return string the absolute path to the system temp directory
 */
std::string Export::getTempPath()
{
    return QDir::toNativeSeparators(QDir::tempPath()).toStdString();
}

/**
 * Copies the data from the temp file
 *
 * @param The input file directory to copy from
 */
void Export::copyData(string inputFileDirectory)
{
    // opens the files
    ifstream iFile(inputFileDirectory, ios::binary);
    if (iFile.fail())
    {
        // TODO: Handle error
        cerr << "Error opening file: " << inputFileDirectory << endl;
    }
    ofstream oFile(this->targetDirectory, ios::binary);
    if (oFile.fail())
    {
        // TODO: Handle error
        cerr << "Error opening file: " << this->targetDirectory << endl;
    }

    // copies the file
    oFile << iFile.rdbuf();

    // close the file
    oFile.close();
    iFile.close();
}

/**
 * Destroys the instance of Export
 */
Export::~Export()
{

}