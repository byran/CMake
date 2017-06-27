/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmExtraMPLABXGenerator.h"

#include <map>
#include <ostream>
#include <string.h>
#include <utility>

#include "cmAlgorithms.h"
#include "cmGeneratedFileStream.h"
#include "cmGeneratorTarget.h"
#include "cmGlobalGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmStateTypes.h"
#include "cmSystemTools.h"
#include "cmXMLWriter.h"
#include "cmake.h"

/* Some useful URLs:
Homepage:
http://www.codeblocks.org

File format docs:
http://wiki.codeblocks.org/index.php?title=File_formats_description
http://wiki.codeblocks.org/index.php?title=Workspace_file
http://wiki.codeblocks.org/index.php?title=Project_file

Discussion:
http://forums.codeblocks.org/index.php/topic,6789.0.html
*/

cmExtraMPLABXGenerator::cmExtraMPLABXGenerator()
  : cmExternalMakefileProjectGenerator()
{
}

cmExternalMakefileProjectGeneratorFactory*
cmExtraMPLABXGenerator::GetFactory()
{
  static cmExternalMakefileProjectGeneratorSimpleFactory<
          cmExtraMPLABXGenerator>
          factory("MPLABX", "Generates MPLABX project files.");

  if (factory.GetSupportedGlobalGenerators().empty())
  {
#if defined(_WIN32)
    factory.AddSupportedGlobalGenerator("MinGW Makefiles");
    factory.AddSupportedGlobalGenerator("NMake Makefiles");
    factory.AddSupportedGlobalGenerator("NMake Makefiles JOM");
// disable until somebody actually tests it:
// this->AddSupportedGlobalGenerator("MSYS Makefiles");
#endif
    factory.AddSupportedGlobalGenerator("Ninja");
    factory.AddSupportedGlobalGenerator("Unix Makefiles");
  }

  return &factory;
}

void cmExtraMPLABXGenerator::Generate()
{
  // for each sub project in the project create a codeblocks project
  for (std::map<std::string, std::vector<cmLocalGenerator*> >::const_iterator
               it = this->GlobalGenerator->GetProjectMap().begin();
       it != this->GlobalGenerator->GetProjectMap().end(); ++it)
  {
    // create a project file
    this->CreateProjectFile(it->second);
  }
}

/* create the project file */
void cmExtraMPLABXGenerator::CreateProjectFile(
        const std::vector<cmLocalGenerator*>& lgs)
{
  std::string outputDir = lgs[0]->GetCurrentBinaryDirectory();
  std::string projectName = lgs[0]->GetProjectName();

  std::string filename = outputDir + "/";
  filename += projectName + ".xml";

  this->CreateNewProjectFile(lgs, filename);
}

namespace
{
    class XMLElement
    {
    private:
        cmXMLWriter& xml;
    public:
        XMLElement(cmXMLWriter& xml, char const* name)
                : xml{xml}
        {
          xml.StartElement(name);
        }

        XMLElement(XMLElement const &) = delete;
        XMLElement& operator=(XMLElement const &) = delete;

        ~XMLElement()
        {
            xml.EndElement();
        }
    };
}


void cmExtraMPLABXGenerator::CreateNewProjectFile(
        const std::vector<cmLocalGenerator*>& lgs, const std::string& filename)
{
  const cmMakefile *mf = lgs[0]->GetMakefile();
  cmGeneratedFileStream fout(filename.c_str());
  if (!fout)
  {
    return;
  }

  // figure out the compiler
  std::string make = mf->GetRequiredDefinition("CMAKE_MAKE_PROGRAM");
  const std::string makeArgs =
          mf->GetSafeDefinition("CMAKE_CODEBLOCKS_MAKE_ARGUMENTS");

  cmXMLWriter xml(fout);
  xml.StartDocument();

  {
    XMLElement rootElement(xml, "configurationDescriptor");
    xml.Attribute("version", "62");

    for (auto it = lgs.cbegin(); it != lgs.cend(); ++it)
    {
      std::string const buildType = (*it)->
              GetMakefile()->GetSafeDefinition("CMAKE_BUILD_TYPE");

      auto &targets = (*it)->GetGeneratorTargets();

      for (auto ti = targets.cbegin(); ti != targets.cend(); ++ti)
      {
        switch ((*ti)->GetType())
        {
          case cmStateEnums::EXECUTABLE:
          case cmStateEnums::STATIC_LIBRARY:
          case cmStateEnums::SHARED_LIBRARY:
          case cmStateEnums::MODULE_LIBRARY:
          case cmStateEnums::OBJECT_LIBRARY:
            XMLElement project(xml, "Project");
            xml.Attribute("value", (*ti)->GetName().c_str());

            auto libraries = (*ti)->GetLinkImplementationLibraries(buildType);
            for (auto li = libraries->Libraries.cbegin();
                 li != libraries->Libraries.cend(); ++li)
            {
              XMLElement includeDir(xml, "Library");
              xml.Attribute("value", (*li).c_str());
            }


            std::vector<std::string> includeDirs;
            (*it)->GetIncludeDirectories(includeDirs, (*ti), "C", buildType);
            for (auto iit = includeDirs.cbegin();
                 iit != includeDirs.cend(); ++iit)
            {
              XMLElement includeDir(xml, "IncluldeDir");
              xml.Attribute("value", (*iit).c_str());
            }

            std::vector<cmSourceFile *> sources;
            cmMakefile *makefile = lgs[0]->GetMakefile();

            (*ti)->GetSourceFiles(sources,
                           makefile->GetSafeDefinition("CMAKE_BUILD_TYPE"));

            for (auto si = sources.cbegin(); si != sources.cend(); ++si)
            {
              XMLElement file(xml, "File");
              xml.Attribute("value", (*si)->GetFullPath().c_str());
            }
            break;
        }
      }
    }

    XMLElement rootLogicalFolder(xml, "logicalFolder");
    xml.Attribute("name", "root");
    xml.Attribute("displayName", "root");
    xml.Attribute("projectFiles", "true");

    {
      XMLElement headerFilesLogicalFolder(xml, "logicalFolder");
      xml.Attribute("name", "HeaderFiles");
      xml.Attribute("displayName", "Header Files");
      xml.Attribute("projectFiles", "true");
    }

    {
      XMLElement headerFilesLogicalFolder(xml, "logicalFolder");
      xml.Attribute("name", "LinkerFiles");
      xml.Attribute("displayName", "Linker Files");
      xml.Attribute("projectFiles", "true");
    }

    {
      XMLElement headerFilesLogicalFolder(xml, "logicalFolder");
      xml.Attribute("name", "SourceFiles");
      xml.Attribute("displayName", "Source Files");
      xml.Attribute("projectFiles", "true");
    }

    {
      XMLElement headerFilesLogicalFolder(xml, "logicalFolder");
      xml.Attribute("name", "ExternalFiles");
      xml.Attribute("displayName", "Important Files");
      xml.Attribute("projectFiles", "false");
    }

  }
  xml.EndDocument();
}
