
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Shlwapi.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

using namespace rapidxml;
using cstring = const std::string;
using string = std::string;
using sstream = std::stringstream;

cstring gDataFilePath("..\\ScriptBuilderData.xml");
cstring gClassNameAlias("$(CLASS_NAME)");
const unsigned gMaxFileBufferSize = 100000;

void throwError(cstring& message)
{
    throw std::exception(message.c_str());
}

std::string upperCase(cstring& str)
{
    std::string strUpper("", str.length());
    std::transform(str.begin(), str.end(), strUpper.begin(), ::toupper);
    return strUpper;
}

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

void ifStreamToBuffer(std::ifstream& fileStream, char* buffer, unsigned size)
{
    sstream contentStream;
    contentStream << fileStream.rdbuf();
    string contentStr = contentStream.str();
    memcpy_s(buffer, size, contentStr.c_str(), contentStr.length());
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

xml_node<>* createXmlNode(xml_document<>& doc, cstring& name)
{
    char* nodeName = doc.allocate_string(name.c_str());
    return doc.allocate_node(node_type::node_element, nodeName);
}

xml_attribute<>* createXmlAttrib(xml_document<>& doc, cstring& name, cstring& value)
{
    char* attribName = doc.allocate_string(name.c_str());
    char* attribValue = doc.allocate_string(value.c_str());
    return doc.allocate_attribute(attribName, attribValue);
}

void appendNewXmlNodeAndAttribs(xml_document<>& doc, xml_node<>* node, cstring& nodeName, cstring& attribName, cstring& attribValue)
{
    xml_node<>* newNode = createXmlNode(doc, nodeName);
    xml_attribute<>* newAttrib = createXmlAttrib(doc, attribName, attribValue);
    newNode->append_attribute(newAttrib);
    node->append_node(newNode);
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

    try
    {
        std::cout << "Enter pattern key: " << std::endl;
        std::cin >> patternKeyStr;
        patternKeyUpperStr = upperCase(patternKeyStr);

        std::cout << "Enter new script name: " << std::endl;
        std::cin >> scriptName;

        char dataContentsArr[gMaxFileBufferSize];   // contents of data file as char array
        memset(dataContentsArr, 0, gMaxFileBufferSize);

        std::ifstream dataFile(gDataFilePath, std::ios::in);
        if (dataFile.is_open())
        {
            ifStreamToBuffer(dataFile, dataContentsArr, gMaxFileBufferSize);
            dataFile.close();
        }
        else
        {
            throwError("ERROR: Could not open file for read: " + gDataFilePath);
        }

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
        // load project file into a char buffer
        std::ifstream projFileIn(relpath_project, std::ios::in);
        if (projFileIn.is_open())
        {
            char projContentArr[gMaxFileBufferSize];
            memset(projContentArr, 0, gMaxFileBufferSize);

            ifStreamToBuffer(projFileIn, projContentArr, gMaxFileBufferSize);
            projFileIn.close();
            docProj.parse<0>(projContentArr);
        }
        // append new files to project file
        std::ofstream projFileOut(relpath_project, std::ios::out);
        if (projFileOut.is_open())
        {
            bool headerIncludesFound = false;
            bool sourceIncludesFound = false;
            xml_node<>* rootNode = docProj.first_node();
            xml_node<>* itemGroupNode = rootNode->first_node("ItemGroup");
            while (itemGroupNode)
            {
                if (headerIncludesFound == false)
                {
                    xml_node<>* includeNode = itemGroupNode->first_node("ClInclude");
                    if (includeNode)
                    {
                        appendNewXmlNodeAndAttribs(docProj, itemGroupNode, "ClInclude", "Include", path_projToNewHeader);
                        headerIncludesFound = true;
                    }
                }
                else if (sourceIncludesFound == false)
                {
                    xml_node<>* sourceNode = itemGroupNode->first_node("ClCompile");
                    if (sourceNode)
                    {
                        appendNewXmlNodeAndAttribs(docProj, itemGroupNode, "ClCompile", "Include", path_projToNewSource);
                        sourceIncludesFound = true;
                    }
                }
                else
                {
                    break;
                }
                itemGroupNode = itemGroupNode->next_sibling("ItemGroup");
            }
            if (headerIncludesFound == false)
            {
                // no files yet, add them
                xml_node<>* newItemGroupNode = createXmlNode(docProj, "ItemGroup");
                appendNewXmlNodeAndAttribs(docProj, newItemGroupNode, "ClInclude", "Include", path_projToNewHeader);
                rootNode->append_node(newItemGroupNode);
            }
            if (sourceIncludesFound == false)
            {
                // no files yet, add them
                xml_node<>* newItemGroupNode = createXmlNode(docProj, "ItemGroup");
                appendNewXmlNodeAndAttribs(docProj, newItemGroupNode, "ClCompile", "Include", path_projToNewSource);
                rootNode->append_node(newItemGroupNode);
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
