#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <iterator>

void SearchAndReplace(std::ifstream& input_file, std::ofstream& out_file, const std::string& to_replace, const std::string& rep_with)
{
    if (input_file.is_open())
    {
        if (out_file.is_open())
        {
            std::string line;
            while (std::getline(input_file, line))
            {
                size_t prev = 0;
                size_t pos;
                while ((pos = line.find_first_of(" ;.:(){}_<->\\//&*\"\'\n\t~", prev)) != std::string::npos)
                {
                    if (line.substr(prev, pos - prev) == to_replace)
                        out_file << rep_with << line.substr(pos, 1);
                    else
                        out_file << line.substr(prev, pos + 1 - prev);
                    prev = pos + 1;
                }
                out_file << line.substr(prev, line.length() - prev);
                out_file << '\n';
            }
            out_file.close();
        }
        input_file.close();
    }
}

int main(void)
{
    std::cout << "Enter new component name: " << std::endl;;
    std::string name_str;
    std::getline(std::cin, name_str);

    std::string comp_dir("..\\code\\subclasses\\components\\");
    std::string comp_path_str(comp_dir + name_str + "\\");

    if (CreateDirectory(comp_path_str.c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
    {
        std::string replace("ComponentPattern");

        std::string full_comp_path_h(comp_path_str + name_str + ".h");
        std::string full_comp_path_cpp(comp_path_str + name_str + ".cpp");

        ////////////////////////////////
        // copy header into directory

        std::string copied_file_header(comp_path_str + "\\" + name_str + "_copy.h");
        TCHAR copied_file_t[MAX_PATH];
        memset(copied_file_t, 0, MAX_PATH);
        std::copy(copied_file_header.begin(), copied_file_header.end(), copied_file_t);

        if (CopyFile("source\\ComponentPattern.h", copied_file_t, false))
        {

            std::ifstream copied_file(copied_file_t);
            std::ofstream replaced_file(comp_path_str + "\\" + name_str + ".h");

            SearchAndReplace(copied_file, replaced_file, replace, name_str);

            DeleteFile(copied_file_t);
        }

        ////////////////////////////
        // copy implementation file
        std::string copied_file_source(comp_path_str + "\\" + name_str + "_copy.cpp");
        memset(copied_file_t, 0, MAX_PATH);
        std::copy(copied_file_source.begin(), copied_file_source.end(), copied_file_t);

        if (CopyFile("source\\ComponentPattern.cpp", copied_file_t, false))
        {
            std::ifstream copied_file(copied_file_t);
            std::ofstream replaced_file(comp_path_str + "\\" + name_str + ".cpp");

            SearchAndReplace(copied_file, replaced_file, replace, name_str);

            DeleteFile(copied_file_t);
        }

        //////////////////////////////////
        // append path to "includes" header

        std::string comp_header("\"subclasses\\components\\" + name_str + "\\" + name_str + ".h\"");
        std::string includes_path("..\\code\\includes\\componentincludes.h");
        std::ofstream includes_file;
        includes_file.open(includes_path, std::ios::app);
        if (includes_file.is_open())
        {
            includes_file << "\n#include " << comp_header << "\n";
            includes_file.close();
        }

        ///////////////////////////////////
        // include in VS project
        //////////////////////////////////

        std::string proj_path("..\\project_vs2015\\LucidDreamEngine.vcxproj");
        std::fstream proj_file(proj_path);
        std::vector<std::string> file_vec;

        if (proj_file.is_open())
        {
            std::string h_key("ClInclude ");
            std::string cpp_key("ClCompile ");
            std::string h_line("    <ClInclude Include=\"" + full_comp_path_h + "\" />");
            std::string cpp_line("    <ClCompile Include=\"" + full_comp_path_cpp + "\" />");

            std::string line;
            while (std::getline(proj_file, line, '\n'))
                file_vec.push_back(line);

            // insert xml .h file path into vector of file lines
            for (auto it = file_vec.begin(); it != file_vec.end(); ++it)
            {
                line = *it;
                size_t pos = line.find(h_key);
                if (pos != std::string::npos)
                {
                    // alphabetical order
                    if (h_line < line)
                    {
                        // insert h_line one above
                        file_vec.insert(it, h_line);
                        break;
                    }
                }
            }

            // insert xml .cpp file path into vector of file lines
            for (auto it = file_vec.begin(); it != file_vec.end(); ++it)
            {
                line = *it;
                size_t pos = line.find(cpp_key);
                if (pos != std::string::npos)
                {
                    // alphabetical order
                    if (cpp_line < line)
                    {
                        // insert h_line one above
                        file_vec.insert(it, cpp_line);
                        break;
                    }
                }
            }

            proj_file.close();
        }

        // overwrite file with additional xml file paths
        std::ofstream proj_file_out(proj_path);
        if (proj_file_out.is_open())
        {
            std::ostream_iterator<std::string> output_iter(proj_file_out, "\n");
            std::copy(file_vec.begin(), file_vec.end(), output_iter);
            proj_file_out.close();
        }

        //////////////////////////////////////////
    }
    else
    {
        // Failed to create directory.
        std::cout << "Directory could not be created" << std::endl;
        system("pause");
    }

    return 0;
}
