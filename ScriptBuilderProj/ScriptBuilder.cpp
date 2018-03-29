/******************************************************************************
\file   ScriptBuilder.cpp
\author Brett Cunningham
\date   Mar 29, 2018
\brief  Automatically copies specified source and header files used as patterns
        for scripts (such as components or systems), renames them and replaces
        their class name instances with a given script name, adds them to a
        provided Visual Studio project, and optionally appends the script
        header path to a specified "includes" file.
******************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Shlwapi.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#pragma region GLOBALS

using namespace rapidxml;
using cstring = const std::string;
using string = std::string;
using sstream = std::stringstream;

cstring gDataFilePath("ScriptBuilderData.xml");
cstring gClassNameAlias("$(CLASS_NAME)");
const unsigned gMaxFileBufferSize = 100000;

#pragma endregion   // GLOBALS

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma region GENERIC_HELPERS

/*
\brief Throws a std::exception with given message.
\param message[in]: error message
*/
void throwError(cstring& message)
{
    throw std::exception(message.c_str());
}

/*
\brief Returns an uppercase copy of given string.
\param str[in]: string to make uppercase
\return uppercase copy of string
*/
std::string upperCase(cstring& str)
{
    std::string strUpper("", str.length());
    std::transform(str.begin(), str.end(), strUpper.begin(), ::toupper);
    return strUpper;
}

/*
\brief Attempts to find a file or directory.
\param path[in]: path of file
\param outFindData[out]: optional FIND_DATA structure
\return true if file exists, false otherwise
*/
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

/*
\brief Creates a new directory.
\param path[in]: destination path
*/
void createDir(cstring& path)
{
    if (CreateDirectory(path.c_str(), NULL) == ERROR_PATH_NOT_FOUND)
        throw std::exception(("ERROR: Could not create directory at " + path).c_str());
}

/*
\brief Outputs from inFile to outFile, replacing instances of given 'toReplace' string with 'replaceWith'.
\param inFile[in]: input file stream
\param outFile[in]: output file stream
\param toReplace[in]: string to be replaced
\param replaceWith[in]: string to insert
*/
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

/*
\brief Duplicates a file, renames to 'newName', replaces instances of 'nameAlias' to 'newName' within.
\param path_in[in]: path of file to copy
\param path_out[in]: path of output file
\param nameAlias[in]: string to replace
\param newName[in]: name of new script
\return true on success, false otherwise
*/
bool duplicateAndReplaceFile(cstring& path_in, cstring& path_out, cstring& nameAlias, cstring& newName)
{
    std::string path_temp = path_out + ".temp";
    if (CopyFile(path_in.c_str(), path_temp.c_str(), false))
    {
        std::ifstream tempFile(path_temp);
        std::ofstream replacedFile(path_out);
        if (tempFile.is_open() && replacedFile.is_open())
        {
            searchAndReplace(tempFile, replacedFile, nameAlias, newName);
            tempFile.close();
            replacedFile.close();
            DeleteFile(path_temp.c_str());
            return true;
        }
    }
    return false;
}

/*
\brief Given two relative paths, returns new relative path from 'from' to 'to'.
\param relpath_from[in]: path of start of new relative path
\param fileType_from[in]: flag for first file type
\param relpath_to[in]: path of end of new relative path
\param fileType_to[in]: flag for second file type
\return new relative path from 'from' to 'to' paths
*/
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

/*
\brief Fills a char buffer with contents of file stream.
\param filestream[in]: input file stream
\param buffer[out]: char buffer to store contents
\param buffer[in]: size of char buffer
*/
void ifStreamToBuffer(std::ifstream& fileStream, char* buffer, unsigned size)
{
    sstream contentStream;
    contentStream << fileStream.rdbuf();
    string contentStr = contentStream.str();
    memcpy_s(buffer, size, contentStr.c_str(), contentStr.length());
}

#pragma endregion   // GENERIC_HELPERS

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma region XML_HELPERS

/*
\brief Get specified first node from parent, throw error if not found.
\param parentNode[in]: node to base from
\param nodeName[in]: name of child node to find
\return found xml node, or nullptr
*/
xml_node<>* assertFirstNode(xml_node<>* parentNode, cstring& nodeName)
{
    xml_node<>* node = parentNode->first_node(nodeName.c_str());
    if (node == nullptr)
        throwError("ERROR: '" + nodeName + "' not found in " + gDataFilePath);
    return node;
}

/*
\brief Search child nodes of parent for "Key" node matching given string value.
\param parentNode[in]: node to base from
\param keyNameUpper[in]: name of key value to match
\return found xml node, or nullptr
*/
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

/*
\brief Allocates new xml node with name.
\param doc[in]: xml document
\param name[in]: name of new node
\return new xml node
*/
xml_node<>* createXmlNode(xml_document<>& doc, cstring& name)
{
    char* nodeName = doc.allocate_string(name.c_str());
    return doc.allocate_node(node_type::node_element, nodeName);
}

/*
\brief Allocates new xml attribute with name and value.
\param doc[in]: xml document
\param name[in]: name of new attribute
\param value[in]: name of new attribute value
\return new xml attribute
*/
xml_attribute<>* createXmlAttrib(xml_document<>& doc, cstring& name, cstring& value)
{
    char* attribName = doc.allocate_string(name.c_str());
    char* attribValue = doc.allocate_string(value.c_str());
    return doc.allocate_attribute(attribName, attribValue);
}

/*
\brief Creates new node and attribute, appends as child under given parent node.
\param doc[in]: xml document
\param node[in]: parent node to base from
\param nodeName[in]: name of new node
\param attribName[in]: name of new attribute
\param attribValue[in]: name of new attribute value
*/
void appendNewXmlNodeAndAttribs(xml_document<>& doc, xml_node<>* node, cstring& nodeName,
    cstring& attribName, cstring& attribValue)
{
    xml_node<>* newNode = createXmlNode(doc, nodeName);
    xml_attribute<>* newAttrib = createXmlAttrib(doc, attribName, attribValue);
    newNode->append_attribute(newAttrib);
    node->append_node(newNode);
}

#pragma endregion   // XML_HELPERS

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma region MAIN

int main(void)
{
    string relpath_outputDir;       // relative path of new script output directory
    string relpath_patternDir;      // relative path of pattern files directory
    string relpath_newSource;       // relative path of new script source file destination
    string relpath_newHeader;       // relative path of new script header file destination
    string relpath_patternSource;   // relative path of pattern source file destination
    string relpath_patternHeader;   // relative path of pattern header file destination
    string relpath_project;         // relative path of .vcxproj project file
    string relpath_include;         // relative path of optional 'includes' file

    string path_projToNewSource;    // path from project file to new script source destination
    string path_projToNewHeader;    // path from project file to new script header destination

    string patternKeyStr;           // key for looking up pattern paths in data file
    string scriptName;              // name of new script
    string patternName;             // name of pattern file(s)

    try
    {
        ///////////////////////////////////
        // user input
        //////////////////////////////////

        std::cout << "Enter pattern key: " << std::endl;
        std::cin >> patternKeyStr;

        std::cout << "Enter new script name: " << std::endl;
        std::cin >> scriptName;

        ///////////////////////////////////
        // parse data file
        //////////////////////////////////

        xml_document<> docData;
        // open data file
        char dataContentsArr[gMaxFileBufferSize];
        memset(dataContentsArr, 0, gMaxFileBufferSize);
        std::ifstream dataFile(gDataFilePath, std::ios::in);
        if (dataFile.is_open())
        {
            // load contents of data file into memory
            ifStreamToBuffer(dataFile, dataContentsArr, gMaxFileBufferSize);
            dataFile.close();
            docData.parse<0>(dataContentsArr);
        }
        else
        {
            throwError("ERROR: Could not open file for read: " + gDataFilePath);
        }

        // get root node
        xml_node<>* rootNode = docData.first_node();
        // get relative path of vs project file
        relpath_project = assertFirstNode(rootNode, "ProjectFilePath")->value();
        // get start of user pattern definitions
        xml_node<>* patternsRootNode = assertFirstNode(rootNode, "PatternsRoot");
        // find pattern key matching given key
        xml_node<>* patternNode = findMatchingKey(patternsRootNode, upperCase(patternKeyStr));
        if (patternNode == nullptr)
            throwError("ERROR: Could not find matching key '" + patternKeyStr + "' in " + gDataFilePath);

        // extract pattern path information
        relpath_patternDir = assertFirstNode(patternNode, "PatternDir")->value();
        patternName = assertFirstNode(patternNode, "PatternName")->value();
        relpath_outputDir = assertFirstNode(patternNode, "OutputDir")->value();

        // create subdirectories as needed
        if (fileExists(relpath_outputDir) == false)
        {
            createDir(relpath_outputDir);
        }
        bool createSubDir = upperCase(assertFirstNode(patternNode, "CreateSubDir")->value()) == "TRUE";
        if (createSubDir)
        {
            relpath_outputDir += scriptName + "\\";
            createDir(relpath_outputDir);
        }

        // check for 'includes' file definition
        xml_node<>* includeNode = patternNode->first_node("IncludeFile");
        if (includeNode)
            relpath_include = includeNode->value();

        // compose destination paths for pattern scripts
        relpath_patternSource = relpath_patternDir + patternName + ".cpp";
        relpath_patternHeader = relpath_patternDir + patternName + ".hpp";

        // choose between ".cpp" or ".c" extensions for source files
        if (fileExists(relpath_patternSource))
        {
            relpath_newSource = relpath_outputDir + scriptName + ".cpp";
        }
        else
        {
            relpath_newSource = relpath_outputDir + scriptName + ".c";
            relpath_patternSource = relpath_patternDir + patternName + ".c";
        }

        // choose between ".hpp" or ".h" extensions for source files
        if (fileExists(relpath_patternHeader))
        {
            relpath_newHeader = relpath_outputDir + scriptName + ".hpp";
        }
        else
        {
            relpath_newHeader = relpath_outputDir + scriptName + ".h";
            relpath_patternHeader = relpath_patternDir + patternName + ".h";
        }

        // do not allow duplicates
        if (fileExists(relpath_newSource))
            throwError("ERROR: File " + relpath_newSource + " already exists");
        if (fileExists(relpath_newHeader))
            throwError("ERROR: File " + relpath_newHeader + " already exists");

        ///////////////////////////////////
        // create new script files
        //////////////////////////////////

        // copy pattern files and replace script names
        bool newSourceExists = duplicateAndReplaceFile(relpath_patternSource, relpath_newSource, gClassNameAlias, scriptName);
        bool newHeaderExists = duplicateAndReplaceFile(relpath_patternHeader, relpath_newHeader, gClassNameAlias, scriptName);

        // get relative path from project to script source/header
        path_projToNewSource = getRelativePath(relpath_project, FILE_ATTRIBUTE_NORMAL,
            relpath_newSource, FILE_ATTRIBUTE_NORMAL);
        path_projToNewHeader = getRelativePath(relpath_project, FILE_ATTRIBUTE_NORMAL,
            relpath_newHeader, FILE_ATTRIBUTE_NORMAL);

        // insert into includes file if requested
        if (includeNode && newHeaderExists)
        {
            relpath_include = includeNode->value();
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
        char projContentArr[gMaxFileBufferSize];
        memset(projContentArr, 0, gMaxFileBufferSize);
        std::ifstream projFileIn(relpath_project, std::ios::in);
        if (projFileIn.is_open())
        {
            ifStreamToBuffer(projFileIn, projContentArr, gMaxFileBufferSize);
            projFileIn.close();
            docProj.parse<rapidxml::parse_non_destructive>(projContentArr);
        }
        else
            throwError("ERROR: Could not open project file " + relpath_project);

        // append new files to project file
        std::ofstream projFileOut(relpath_project, std::ios::out);
        if (projFileOut.is_open())
        {
            try
            {
                bool headerIncludesFound = false;
                bool sourceIncludesFound = false;
                xml_node<>* rootNode = docProj.first_node();
                xml_node<>* itemGroupNode = rootNode->first_node("ItemGroup");
                while (itemGroupNode)
                {
                    // find item group node with Header file definitions
                    if (newHeaderExists && headerIncludesFound == false)
                    {
                        xml_node<>* includeNode = itemGroupNode->first_node("ClInclude");
                        if (includeNode)
                        {
                            // append new script header 
                            appendNewXmlNodeAndAttribs(docProj, itemGroupNode, "ClInclude", "Include", path_projToNewHeader);
                            headerIncludesFound = true;
                        }
                    }
                    // find item group node with Source file definitions
                    else if (newSourceExists && sourceIncludesFound == false)
                    {
                        xml_node<>* sourceNode = itemGroupNode->first_node("ClCompile");
                        if (sourceNode)
                        {
                            // append new script source 
                            appendNewXmlNodeAndAttribs(docProj, itemGroupNode, "ClCompile", "Include", path_projToNewSource);
                            sourceIncludesFound = true;
                        }
                    }
                    else
                    {
                        break;
                    }
                    // advance to next Item Group node
                    itemGroupNode = itemGroupNode->next_sibling("ItemGroup");
                }
                // if no header files included, create Item Group and append new script header
                if (newHeaderExists && headerIncludesFound == false)
                {
                    xml_node<>* newItemGroupNode = createXmlNode(docProj, "ItemGroup");
                    appendNewXmlNodeAndAttribs(docProj, newItemGroupNode, "ClInclude", "Include", path_projToNewHeader);
                    rootNode->append_node(newItemGroupNode);
                }
                // if no source files included, create Item Group and append new script source
                if (newSourceExists && sourceIncludesFound == false)
                {
                    xml_node<>* newItemGroupNode = createXmlNode(docProj, "ItemGroup");
                    appendNewXmlNodeAndAttribs(docProj, newItemGroupNode, "ClCompile", "Include", path_projToNewSource);
                    rootNode->append_node(newItemGroupNode);
                }
                // write modified xml document back to project file
                projFileOut << docProj;
                projFileOut.close();
            }
            catch (...)
            {
                // catch specific project file write errors
                // restore file to original contents
                projFileOut.clear();
                projFileOut << projContentArr;
                projFileOut.close();
                throwError("Error occured while writing to project file!");
            }
        }
        else
        {
            throwError("ERROR: Could not open project file " + relpath_project);
        }
    }
    catch (std::exception& e)
    {
        // output general exception details
        std::cout << e.what() << std::endl;
        system("pause");
        return -1;
    }

    std::cout << "Success!" << std::endl;
    system("pause");
    return 0;
}

#pragma endregion   // MAIN

