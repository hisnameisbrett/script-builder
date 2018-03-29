File:     README.txt
Program:  ScriptBuilder.exe
Author:   Brett Cunningham
Date:     Mar. 29, 2018

Brief:    

  ***Note: Tested on Visual Studio 2017 using .c, .cpp, .h, and .hpp file types***
  
  The purpose of this program is to automate the process of creating boiler-plate
  C++ scripts for a Visual Studio project. 

  What it does:
    1. Copies specified header and source files used as models, or "patterns", 
        for how similar files should be structured (such as components).
    2. Renames copied scripts with given script name.
    3. Replaces all occurrences of $(CLASS_NAME) in copied scripts with given script name.
    4. Outputs new scripts to specified directory.
    5. Adds new script files to specified Visual Studio project.
    6. Optionally appends path of new script header file to specified "includes" file.

  How to use:
    1.  Modify the ScriptBuilderData.xml file:
          I.  <ProjectFilePath>: Define path from ScriptBuilder.exe to .vcxproj 
                file of your Visual Studio project.
          II. <Pattern>: Define as many pattern templates as you want.
                a). <Key>: Name shortcut for accessing pattern.
                b). <PatternDir>: Path from ScriptBuilder.exe to directory of your pattern files.
                c). <PatternName>: Name of pattern file, without extensions.
                d). <CreateSubDir>: True = Nests scripts under subdirectory of given script name.
                e). <OutputDir>: Path from ScriptBuilder.exe to directory to output new scripts.
                f). <IncludeFile>: OPTIONAL Path from ScriptBuilder.exe to a header 'includes' 
                      file to append to.
          Example:
          
            <Root>
              <ProjectFilePath>SampleVSProj\SampleProject\SampleProject.vcxproj</ProjectFilePath>
              <PatternsRoot>
                <Pattern>
                  <Key>COMPONENT</Key>
                  <PatternDir>ScriptPatterns\Component\</PatternDir>
                  <PatternName>ComponentPattern</PatternName>
                  <CreateSubDir>true</CreateSubDir>
                  <OutputDir>SampleVSProj\SampleSource\Components\</OutputDir>
                  <IncludeFile>SampleVSProj\SampleSource\ComponentIncludes.h</IncludeFile>
              </PatternsRoot>
            </Root>
          
    2.  Run ScriptBuilder.exe
          I.  Enter key for type of pattern to use (such as "component" or "system").
          II. Enter name of new script(s).

    