#include "cmGlobalMPLABXGenerator.h"
#include "cmLocalMPLABXGenerator.h"

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
bool cmGlobalMPLABXGenerator::SetGeneratorPlatform(std::string const& p,
  cmMakefile* mf)
{
  return true;
}

namespace
{
  bool DefineNotSet(cmMakefile* mf, char const * define)
  {
    return !mf->GetDefinition(define) ||
      cmSystemTools::IsOff(mf->GetDefinition(define));
  }

  void AddPathDefineToCache(cmMakefile* mf, char const* define,
    char const * description = "")
  {
    std::string defineValue = mf->GetRequiredDefinition(define);
    // if there are spaces in the make program use short path
    // but do not short path the actual program name, as
    // this can cause trouble with VSExpress
    if (defineValue.find(' ') != defineValue.npos) {
      std::string dir;
      std::string file;
      cmSystemTools::SplitProgramPath(defineValue, dir, file);
      std::string saveFile = file;
      cmSystemTools::GetShortPath(defineValue, defineValue);
      cmSystemTools::SplitProgramPath(defineValue, dir, file);
      defineValue = dir;
      defineValue += "/";
      defineValue += saveFile;
      mf->AddCacheDefinition(define, defineValue.c_str(),
        description, cmStateEnums::FILEPATH);
    }
  }
}

bool cmGlobalMPLABXGenerator::SetGeneratorToolset(std::string const& ts,
  cmMakefile* mf)
{
  languageToolchainVersion = ts;

  mf->AddCacheDefinition("XC32_VERSION", ts.c_str(), "XC32 Version",
    cmStateEnums::STRING);

  if (DefineNotSet(mf, "CMAKE_C_COMPILER") ||
    DefineNotSet(mf, "CMAKE_CXX_COMPILER"))
  {
    std::string setCompiler =
      mf->GetModulesFile("CMakeMPLABXFindCompiler.cmake");

    if (!setCompiler.empty())
    {
      mf->ReadListFile(setCompiler.c_str());
    }
  }

  if (DefineNotSet(mf, "CMAKE_C_COMPILER"))
  {
    std::ostringstream err;
    err << "CMake was unable to find a build program corresponding to \"" <<
      this->GetName() << "\".  CMAKE_C_COMPILER is not set.  "
      "You probably need to select a different build tool.";
    cmSystemTools::Error(err.str().c_str());
    cmSystemTools::SetFatalErrorOccured();
    return false;
  }

  if (DefineNotSet(mf, "CMAKE_CXX_COMPILER"))
  {
    std::ostringstream err;
    err << "CMake was unable to find a build program corresponding to \"" <<
      this->GetName() << "\".  CMAKE_CXX_COMPILER is not set.  "
      "You probably need to select a different build tool.";
    cmSystemTools::Error(err.str().c_str());
    cmSystemTools::SetFatalErrorOccured();
    return false;
  }

  AddPathDefineToCache(mf, "CMAKE_C_COMPILER", "C Compiler");
  AddPathDefineToCache(mf, "CMAKE_CXX_COMPILER", "C++ Compiler");

  return true;
}

bool cmGlobalMPLABXGenerator::FindMakeProgram(cmMakefile* mf)
{
  if (!this->cmGlobalGenerator::FindMakeProgram(mf))
  {
    return false;
  }
  return true;
}

void cmGlobalMPLABXGenerator::Generate()
{
  this->cmGlobalGenerator::Generate();
}

void cmGlobalMPLABXGenerator::GenerateBuildCommand(
        std::vector<std::string>& makeCommand,
        const std::string& makeProgram,
        const std::string& projectName,
        const std::string& projectDir,
        const std::string& targetName,
        const std::string& config, bool fast, bool verbose,
        std::vector<std::string> const& makeOptions)
{
//  makeCommand.push_back(this->SelectMakeProgram(makeProgram));
//  makeCommand.push_back(targetName);

  makeCommand.push_back("true");
}

cmLocalGenerator* cmGlobalMPLABXGenerator::CreateLocalGenerator(
        cmMakefile* mf)
{
  return new cmLocalMPLABXGenerator(this, mf);
}