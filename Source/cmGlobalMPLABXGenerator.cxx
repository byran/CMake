#include "cmGlobalMPLABXGenerator.h"

#include "cmDocumentationEntry.h"
#include "cmMakefile.h"

cmGlobalMPLABXGenerator::cmGlobalMPLABXGenerator(cmake* cm)
  : cmGlobalCommonGenerator(cm)
{
  this->FindMakeProgramFile = "CMakeMPLABXFindMake.cmake";
}

/* Static methods */

void cmGlobalMPLABXGenerator::GetDocumentation(cmDocumentationEntry& entry)
{
  entry.Name = cmGlobalMPLABXGenerator::GetActualName();
  entry.Brief = "Generates MPLABX Project files.";
}

/* Overridden methods */

/*bool cmGlobalMPLABXGenerator::FindMakeProgram(cmMakefile* mf)
{
  if (!this->cmGlobalGenerator::FindMakeProgram(mf))
  {
    return false;
  }
  return true;
}*/

void cmGlobalMPLABXGenerator::GenerateBuildCommand(
        std::vector<std::string>& makeCommand,
        const std::string& makeProgram,
        const std::string& projectName,
        const std::string& projectDir,
        const std::string& targetName,
        const std::string& config, bool fast, bool verbose,
        std::vector<std::string> const& makeOptions)
{
  makeCommand.push_back(this->SelectMakeProgram(makeProgram));
  makeCommand.push_back(
          CurrentMakefile->GetRequiredDefinition("MPLABX_VERSION"));
  makeCommand.push_back(
          CurrentMakefile->GetRequiredDefinition("CMAKE_CXX_STANDARD"));
  auto def = this->GetCurrentMakefile()
          ->GetCMakeInstance()->GetProperty("MPLABX_VERSION");
}

cmLocalGenerator* cmGlobalMPLABXGenerator::CreateLocalGenerator(
        cmMakefile* mf)
{
  return cmGlobalGenerator::CreateLocalGenerator(mf);
}