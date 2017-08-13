#ifndef cmLocalMPLABXConfigurationGenerator_h
#define cmLocalMPLABXConfigurationGenerator_h

#include "cmStateTypes.h"

#include <string>
#include <vector>
#include <map>
#include <ostream>

class cmXMLWriter;

namespace MLPABXConfiguration
{
  struct logicalFolder
  {
    std::string displayName;
    bool projectFiles;
    std::vector<std::string> files;
    std::map<std::string, logicalFolder> subFolders;
  };

  struct library
  {
    std::string name;
    std::string path;
  };
}

class cmLocalMPLABXConfigurationGenerator
{
private:
  mutable cmXMLWriter* xml;
public:
  MLPABXConfiguration::logicalFolder rootFolder;
  std::vector<std::string> sourceRoots;
  std::vector<MLPABXConfiguration::library> libraries;
  cmStateEnums::TargetType targetType{cmStateEnums::TargetType::EXECUTABLE};
  std::string targetDevice{ "PIC32MX575F512H" };
  std::string languageToolchain{ "XC32" };
  std::string languageToolchainVersion{ "1.40" };

  std::map<std::string, std::map<std::string, std::string> >
    toolsConfiguration;

  cmLocalMPLABXConfigurationGenerator();

  void WriteToStream(std::ostream& output) const;

private:
  void WriteElementAndContent(const char *elementName,
    const char *content) const;

  void WriteLogicalFolders(char const * name,
    MLPABXConfiguration::logicalFolder const& folder) const;
  void WriteSourceRoots() const;
  void WriteProjectMakeFile() const;
  void WriteConfigurations() const;
  void WriteToolsSet() const;
  void WriteCompileType() const;
  void WriteLinkerLibrary(MLPABXConfiguration::library const& library) const;
  void WriteMakeCustomizationType() const;
  void WriteToolsConfigurations() const;

  void AddC32Configuration();
  void AddC32_ARConfiguration();
  void AddC32_LDConfiguration();
  void AddC32_CPPConfiguration();
  void AddC32GlobalConfiguration();

public:
  void AddRealICEConfiguration();
  void AddSimulatorConfiguration();
};

#endif
