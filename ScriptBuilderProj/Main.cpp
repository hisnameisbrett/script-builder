
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "Shlwapi.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

using namespace rapidxml;
using cstring = const std::string;
using string = std::string;
using sstream = std::stringstream;

cstring gDataFilePath("..\\ScriptBuilderData.xml");
cstring gClassNameAlias("$(CLASS_NAME)");
cstring gValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-/\\.");
const unsigned gMaxFileBufferSize = 100000;

bool fileExists(cstring& path, WIN32_FIND_DATA* outFindData = nullptr)
{
    WIN32_FIND_DATA ffdata;
    HANDLE fhandle;
    fhandle = FindFirstFile(path.c_str(), &ffdata);
    if (fhandle != INVALID_HANDLE_VALUE)
    {
        if (outFindData)
            *outFindData = ffdata;
        FindClose(fhandle);
        return true;
    }
    return false;
}

std::string upperCase(cstring& str)
{
    std::string strUpper("", str.length());
    std::transform(str.begin(), str.end(), strUpper.begin(), ::toupper);
    return strUpper;
}

bool getAnyValidSubstrPos(size_t& outPos, std::string& outSubstr, cstring str,
    int offset = 0, cstring validChars = gValidCharacters)
{
    size_t pos = str.find_first_of(validChars, offset);
    if (pos != std::string::npos)
    {
        outSubstr = str.substr(pos, str.find_first_not_of(validChars, pos) - pos);
        outPos = pos;
        return true;
    }
    return false;
}

bool getSubstrPos(size_t& outPos, cstring str, cstring subStr, int offset = 0)
{
    size_t pos = str.find(subStr, offset);
    if (pos != std::string::npos)
    {
        outPos = pos;
        return true;
    }
    return false;
}

void searchAndReplace(std::ifstream& inFile, std::ofstream& outFile, cstring toReplace, cstring replaceWith)
{
    std::string line;
    while (std::getline(inFile, line))
    {
        size_t prev = 0;
        size_t pos;
        while ((pos = line.find(toReplace, prev)) != std::string::npos)
        {
            outFile << line.substr(prev, pos - prev);
            outFile << replaceWith;
            prev = pos + toReplace.length();
        }
        outFile << line.substr(prev, line.length() - prev);
        outFile << '\n';
    }
}

void duplicateAndReplaceFile(cstring& path_pattern, cstring& path_out, cstring& dir_out, cstring& className)
{
    std::string path_temp = path_out + ".temp";
    if (fileExists(dir_out) == false)
        if (CreateDirectory(dir_out.c_str(), NULL) == ERROR_PATH_NOT_FOUND)
            throw std::exception(("Could not find path: " + dir_out).c_str());

    if (CopyFile(path_pattern.c_str(), path_temp.c_str(), false))
    {
        std::ifstream tempFile(path_temp);
        std::ofstream replacedFile(path_out);
        if (tempFile.is_open() && replacedFile.is_open())
        {
            searchAndReplace(tempFile, replacedFile, gClassNameAlias, className);
            tempFile.close();
            replacedFile.close();
            DeleteFile(path_temp.c_str());
        }
    }
}

bool parseUserPattern(std::fstream& file, cstring& patternKeyStr, size_t& outPosition, std::string& outStr)
{
    cstring exitStr("</UserPattern>");
    std::string line;
    while (std::getline(file, line))
    {
        if (line.find(exitStr) != std::string::npos)
            return false;
        if (getSubstrPos(outPosition, upperCase(line), upperCase(patternKeyStr)))
        {
            outStr = line;
            return true;
        }
    }
    return false;
}

bool parsePath(std::fstream& file, std::string& outStr, cstring exitStr)
{
    std::string line, subStr;
    size_t pos;
    while (std::getline(file, line))
    {
        if (line.find(exitStr) != std::string::npos)
            return false;
        if (getAnyValidSubstrPos(pos, subStr, upperCase(line)))
        {
            outStr = line.substr(pos, subStr.length());
            return true;
        }
    }
    return false;
}

bool parseSolutionPath(std::fstream& file, std::string& outStr)
{
    return parsePath(file, outStr, "/SolutionDirPath");
}

bool parseProjectPath(std::fstream& file, std::string& outStr)
{
    return parsePath(file, outStr, "/ProjectFilePath");
}

void throwError(cstring& message)
{
    throw std::exception(message.c_str());
}

// given two relative paths, returns new relative path from 'from' to 'to'
string getRelativePath(cstring relpath_from, DWORD fileType_from, cstring relpath_to, DWORD fileType_to)
{
    char buffer[MAX_PATH];
    std::string absPath_from;
    std::string absPath_to;

    GetFullPathName(relpath_from.c_str(), MAX_PATH, buffer, NULL);
    absPath_from = { buffer };
    GetFullPathName(relpath_to.c_str(), MAX_PATH, buffer, NULL);
    absPath_to = { buffer };
    PathRelativePathTo(buffer, absPath_from.c_str(), fileType_from, absPath_to.c_str(), fileType_to);

    return { buffer };
}

xml_node<>* assertFirstNode(xml_node<>* parentNode, cstring& nodeName)
{
    xml_node<>* node = parentNode->first_node(nodeName.c_str());
    if (node == nullptr)
        throwError("ERROR: '" + nodeName + "' not found in " + gDataFilePath);
    return node;
}

xml_node<>* findMatchingKey(xml_node<>* parentNode, cstring& keyNameUpper)
{
    xml_node<>* node = parentNode->first_node();
    while (node)
    {
        if (upperCase(assertFirstNode(node, "Key")->value()) == keyNameUpper)
            return node;
        node = node->next_sibling();
    }
    return nullptr;
}

int main(void)
{
    string relpath_output;
    string relpath_patternDir;
    string relpath_newSource;  // relative path to new source file destination
    string relpath_newHeader;  // relative path to new header file destination
    string relpath_patternSource;  // relative path to new source file destination
    string relpath_patternHeader;  // relative path to new header file destination
    string relpath_project;    // relative path to .vcxproj project file
    string relpath_include;

    string path_projToNewSource;
    string path_projToNewHeader;

    string patternKeyStr;      // key for looking up pattern paths in data file
    string patternKeyUpperStr; // uppercase key for comparison
    string scriptName;         // name of new script
    string patternName;         // name of pattern file

    sstream dataContentStream;        // contents of data file as stream
    string dataContentStr;                 // contents of data file as string      
    char dataContentsArr[gMaxFileBufferSize];   // contents of data file as char array


    try
    {
        std::cout << "Enter pattern key: " << std::endl;
        std::cin >> patternKeyStr;
        patternKeyUpperStr = upperCase(patternKeyStr);

        std::cout << "Enter new script name: " << std::endl;
        std::cin >> scriptName;

        std::ifstream dataFile(gDataFilePath, std::ios::in);
        if (dataFile.is_open())
        {
            dataContentStream << dataFile.rdbuf();
            dataFile.close();
        }
        else
        {
            throwError("ERROR: Could not open file for read: " + gDataFilePath);
        }

        dataContentStr = dataContentStream.str();
        memset(dataContentsArr, 0, gMaxFileBufferSize);
        memcpy_s(dataContentsArr, gMaxFileBufferSize, dataContentStr.c_str(), dataContentStr.length());

        // parse xml data file
        xml_document<> doc;
        doc.parse<0>(dataContentsArr);
        // get root node
        xml_node<>* rootNode = doc.first_node();
        // get relative path of vs project file
        relpath_project = assertFirstNode(rootNode, "ProjectFilePath")->value();
        // start of user pattern definitions
        xml_node<>* patternsRootNode = assertFirstNode(rootNode, "PatternsRoot");

        xml_node<>* patternNode = findMatchingKey(patternsRootNode, upperCase(patternKeyStr));
        if (patternNode == nullptr)
            throwError("ERROR: Could not find matching key '" + patternKeyStr + "' in " + gDataFilePath);

        relpath_patternDir = assertFirstNode(patternNode, "PatternDir")->value();
        patternName = assertFirstNode(patternNode, "PatternName")->value();
        relpath_output = assertFirstNode(patternNode, "OutputDir")->value();
        bool createSubDir = upperCase(assertFirstNode(patternNode, "CreateSubDir")->value()) == "TRUE";
        if (createSubDir)
            relpath_output += scriptName + "\\";

        xml_node<>* includeNode = patternNode->first_node("IncludeFile");
        if (includeNode)
            relpath_include = includeNode->value();

        // compose destination paths for new script(s)
        relpath_patternSource = relpath_patternDir + patternName + ".cpp";
        relpath_patternHeader = relpath_patternDir + patternName + ".hpp";

        relpath_newSource = relpath_output + scriptName + ".cpp";
        if (fileExists(relpath_patternHeader))
        {
            relpath_newHeader = relpath_output + scriptName + ".hpp";
        }
        else
        {
            relpath_newHeader = relpath_output + scriptName + ".h";
            relpath_patternHeader = relpath_patternDir + patternName + ".h";
        }

        duplicateAndReplaceFile(relpath_patternSource, relpath_newSource, relpath_output, scriptName);
        duplicateAndReplaceFile(relpath_patternHeader, relpath_newHeader, relpath_output, scriptName);

        // get relative path from project to script source/header
        path_projToNewSource = getRelativePath(relpath_project, FILE_ATTRIBUTE_NORMAL,
            relpath_newSource, FILE_ATTRIBUTE_NORMAL);
        path_projToNewHeader = getRelativePath(relpath_project, FILE_ATTRIBUTE_NORMAL,
            relpath_newHeader, FILE_ATTRIBUTE_NORMAL);

        // insert into includes file if requested
        if (includeNode)
        {
            relpath_include = includeNode->value();
            string includesContentStr;
            std::fstream includesFile(relpath_include, std::ios::app);
            if (includesFile.is_open())
            {
                string path_includesToNewHeader(getRelativePath(relpath_include, FILE_ATTRIBUTE_NORMAL,
                    relpath_newHeader, FILE_ATTRIBUTE_NORMAL));
                includesFile << "\n#include \"" + path_includesToNewHeader + "\"";
                includesFile.close();
            }
        }

        ///////////////////////////////////
        // include in VS project
        //////////////////////////////////

        xml_document<> docProj;
        std::fstream projFileIn(relpath_project, std::ios::in);
        if (projFileIn.is_open())
        {
            sstream projFileStream;
            projFileStream << projFileIn.rdbuf();
            string projContentStr = projFileStream.str();
            projFileIn.close();

            char projContentArr[gMaxFileBufferSize];
            memset(projContentArr, 0, gMaxFileBufferSize);
            memcpy_s(projContentArr, gMaxFileBufferSize, projContentStr.c_str(), projContentStr.length());

            docProj.parse<0>(projContentArr);
            projFileIn.close();
        }

        std::fstream projFileOut(relpath_project, std::ios::out);
        if (projFileOut.is_open())
        {
            xml_node<>* rootNode = docProj.first_node();
            xml_node<>* siblingNode = rootNode->first_node("ItemGroup");
            while (siblingNode)
            {
                xml_node<>* childHeaderNode = siblingNode->first_node("ClInclude");
                if (childHeaderNode)
                {
                    char* nodeName = docProj.allocate_string("ClInclude");
                    xml_node<>* newHeaderNode = docProj.allocate_node(node_type::node_element, nodeName);
                    char* attribName = docProj.allocate_string("Include");
                    char* attribValue = docProj.allocate_string(path_projToNewHeader.c_str());
                    xml_attribute<>* newAttribNode = docProj.allocate_attribute(attribName, attribValue);

                    newHeaderNode->append_attribute(newAttribNode);
                    siblingNode->append_node(newHeaderNode);
                    break;
                }
                siblingNode = siblingNode->next_sibling("ItemGroup");
            }
            if (siblingNode == nullptr)
            {
                // no files yet, add them

                char* nodeName = docProj.allocate_string("ItemGroup");
                xml_node<>* newItemGroupNode = docProj.allocate_node(node_type::node_element, nodeName);

                nodeName = docProj.allocate_string("ClInclude");
                xml_node<>* newHeaderNode = docProj.allocate_node(node_type::node_element, nodeName);
                char* attribName = docProj.allocate_string("Include");
                char* attribValue = docProj.allocate_string(path_projToNewHeader.c_str());
                xml_attribute<>* newAttribNode = docProj.allocate_attribute(attribName, attribValue);

                newHeaderNode->append_attribute(newAttribNode);
                newItemGroupNode->append_node(newHeaderNode);
                rootNode->append_node(newItemGroupNode);
            }

            siblingNode = rootNode->first_node("ItemGroup");
            while (siblingNode)
            {
                xml_node<>* childHeaderNode = siblingNode->first_node("ClCompile");
                if (childHeaderNode)
                {
                    char* nodeName = docProj.allocate_string("ClCompile");
                    xml_node<>* newHeaderNode = docProj.allocate_node(node_type::node_element, nodeName);
                    char* attribName = docProj.allocate_string("Include");
                    char* attribValue = docProj.allocate_string(path_projToNewSource.c_str());
                    xml_attribute<>* newAttribNode = docProj.allocate_attribute(attribName, attribValue);

                    newHeaderNode->append_attribute(newAttribNode);
                    siblingNode->append_node(newHeaderNode);
                    break;
                }
                siblingNode = siblingNode->next_sibling("ItemGroup");
            }
            projFileOut << docProj;
        }
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        system("pause");
        return -1;
    }

    return 0;
}
