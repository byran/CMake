#include "cmLocalMPLABXGenerator.h"
#include "cmLocalMPLABXConfigurationGenerator.h"
#include "cmLocalMPLABXProjectGenerator.h"


#include "cmGeneratorTarget.h"
#include "cmSourceFile.h"
#include "cmMakefile.h"
#include "cmGlobalMPLABXGenerator.h"
#include "cmGeneratedFileStream.h"
#include "cmSystemTools.h"

#include <vector>
#include <algorithm>

cmLocalMPLABXGenerator::cmLocalMPLABXGenerator(cmGlobalGenerator* gg,
  cmMakefile* makefile)
  :cmLocalGenerator(gg, makefile)
{
}

std::string cmLocalMPLABXGenerator::ConvertToFullPath(
  const std::string& localPath)
{
  std::string dir = this->GetCurrentBinaryDirectory();
  dir += "/";
  dir += localPath;
  return dir;
}

std::string cmLocalMPLABXGenerator::IncludePathsForTarget(
  cmGeneratorTarget* target, char const* language) const
{
  auto includeDirectories = target->GetIncludeDirectories("", language);
  std::string includePaths;
  for (auto& path : includeDirectories)
  {
    includePaths += path + ";";
  }
  return includePaths;
}

std::string cmLocalMPLABXGenerator::PreprocessorMacrosForTarget(
  cmGeneratorTarget* target, char const* language) const
{
  std::vector<std::string> defines;
  target->GetCompileDefinitions(defines, "", language);
  std::string macros;
  for(auto& define : defines)
  {
    macros += define + ";";
  }
  return macros;
}

namespace
{
  template<typename T>
  bool WriteToFile(T& object, std::string const& path,
    codecvt::Encoding encoding)
  {
    cmGeneratedFileStream fileStream(path.c_str(), false, encoding);
    if (!fileStream)
    {
      return false;
    }

    object.WriteToStream(fileStream);
    return true;
  }

  void AddMPLABXToolConfigurationFromDefinitions(
    cmGeneratorTarget* target,
    cmLocalMPLABXConfigurationGenerator& configuration)
  {
    auto definitions = target->Makefile->GetDefinitions();
    for(auto& definition : definitions)
    {
      if(cmSystemTools::StringStartsWith(definition.c_str(), "MPLABXTOOL"))
      {
        auto parts = cmSystemTools::SplitString(definition, ',');

        if (parts.size() == 3)
        {
          configuration.toolsConfiguration[parts[1]][parts[2]] =
            target->Makefile->GetRequiredDefinition(definition);
        }
        else
        {
          cmSystemTools::Error("Invalid MPLABXTOOL: ", definition.c_str());
        }
      }
    }
  }
}

namespace
{
    template<typename T>
    void AddToVectorIfNotPresent(std::vector<T>& v, T e)
    {
      if(std::find(v.begin(), v.end(), e) == v.end())
      {
        v.push_back(e);
      }
    }

    template<typename T>
    std::vector<T> WithoutDuplicatesLeavingLast(std::vector<T> const &v)
    {
      std::vector<T> unique;
      for(auto it = v.rbegin(); it != v.rend(); ++it)
      {
        AddToVectorIfNotPresent(unique, *it);
      }
      return {unique.rbegin(), unique.rend()};
    }

    template<typename T>
    void AddLibrary(T& library,
                    cmLocalMPLABXProjectGenerator &project,
                    cmLocalMPLABXConfigurationGenerator &configuration)
    {
      if (library.Target == nullptr)
      {
        configuration.libraries.push_back({false, "", library});

      }
      else
      {

        if (library.Target->GetType() !=
            cmStateEnums::TargetType::INTERFACE_LIBRARY)
        {
          configuration.libraries.push_back({true, library.Target->GetName(),
                                             library.Target->GetDirectory()});
          project.dependencies.push_back(library.Target->GetDirectory() + "/" +
                                         library.Target->GetName() + ".X");
        }
      }
    }

    template<typename T>
    void AddLinkInterfaceLibraries(cmGeneratorTarget *headTarget,
                           std::string const &config,
                           T& linkLibrary,
                           cmLocalMPLABXProjectGenerator &project,
                           cmLocalMPLABXConfigurationGenerator &configuration)
    {
      std::vector<cmLinkItem> linkInterfaceLibraries = linkLibrary.Target->
          GetLinkInterfaceLibraries(config, headTarget, true)->Libraries;

      for (auto interfaceLibrary : linkInterfaceLibraries)
      {
        AddLibrary(interfaceLibrary, project, configuration);

        if (interfaceLibrary.Target != nullptr)
        {
          AddLinkInterfaceLibraries(headTarget, config, interfaceLibrary,
                                    project, configuration);
        }
      }
    }

    void AddLinkLibraies(cmGeneratorTarget *target, std::string const &config,
                         cmLocalMPLABXProjectGenerator &project,
                         cmLocalMPLABXConfigurationGenerator &configuration)
    {
      std::vector<cmLinkImplItem> libraries = target->
          GetLinkImplementationLibraries(config)->Libraries;

      for (auto &library : libraries)
      {
        AddLibrary(library, project, configuration);

        if (library.Target != nullptr)
          AddLinkInterfaceLibraries(target, config, library, project,
                                    configuration);
      }

      // Remove any duplicates that may have been added during the
      // creation of the dependencies
      project.dependencies = WithoutDuplicatesLeavingLast(
          project.dependencies);
      configuration.libraries = WithoutDuplicatesLeavingLast(
          configuration.libraries);
    }
}
extern const std::string makefileContents;

void cmLocalMPLABXGenerator::Generate()
{
  std::vector<cmGeneratorTarget*> targets = this->GetGeneratorTargets();
  std::string config = this->Makefile->GetSafeDefinition("CMAKE_BUILD_TYPE");
  auto projectTool =
    this->Makefile->GetDefinition("MPLABX_PROJECT_MAKEFILES_GENERATOR");
  auto fullProject = this->Makefile->GetDefinition("MPLABX_FULL_PROJECT");

  for(auto target : targets)
  {
    cmLocalMPLABXProjectGenerator project;
    cmLocalMPLABXConfigurationGenerator configuration;
    configuration.targetType = target->GetType();
    configuration.languageToolchainVersion =
        static_cast<cmGlobalMPLABXGenerator*>(GlobalGenerator)->
            languageToolchainVersion;

    if(configuration.targetType == cmStateEnums::TargetType::INTERFACE_LIBRARY)
    {
      continue;
    }

    AddMPLABXToolConfigurationFromDefinitions(target, configuration);

    if(this->Makefile->IsDefinitionSet("MPLABX_PIC"))
    {
      configuration.targetDevice = this->Makefile->GetDefinition("MPLABX_PIC");
    }

    configuration.sourceRoots.push_back(this->GetCurrentSourceDirectory());

    project.name = target->GetName();

    configuration.toolsConfiguration["C32CPP"]["extra-include-directories"] =
      IncludePathsForTarget(target, "CXX");
    configuration.toolsConfiguration["C32"]["extra-include-directories"] =
      IncludePathsForTarget(target, "C");;

    configuration.toolsConfiguration["C32CPP"]["preprocessor-macros"] =
      PreprocessorMacrosForTarget(target, "CXX");
    configuration.toolsConfiguration["C32"]["preprocessor-macros"] =
      PreprocessorMacrosForTarget(target, "C");

    std::vector<cmSourceFile const*> sourceFiles;
    target->GetObjectSources(sourceFiles, config);
    for(auto file : sourceFiles)
    {
      configuration.rootFolder.subFolders["SourceFiles"].
        files.push_back(file->GetFullPath());
    }

    std::vector<cmSourceFile const*> extraSources;
    target->GetExtraSources(extraSources, config);
    for(auto file : extraSources)
    {
      if(cmSystemTools::LowerCase(file->GetExtension()) == "ld")
      {
        configuration.rootFolder.subFolders["LinkerScript"].
            files.push_back(file->GetFullPath());
      }
    }

    std::vector<cmSourceFile const*> headerFiles;
    target->GetHeaderSources(headerFiles, "");
    for (auto file : headerFiles)
    {
      configuration.rootFolder.subFolders["HeaderFiles"].
        files.push_back(file->GetFullPath());
    }

    if(target->GetType() !=
            cmStateEnums::TargetType::STATIC_LIBRARY)
    {
      AddLinkLibraies(target, config, project, configuration);
    }

    configuration.AddSimulatorConfiguration();

    std::string nbprojectPath = this->ConvertToFullPath(
      target->GetName() + ".X/nbproject");
    cmSystemTools::MakeDirectory(nbprojectPath);

    std::string configurationFullPath = nbprojectPath + "/configurations.xml";
    WriteToFile(configuration, configurationFullPath,
      this->GlobalGenerator->GetMakefileEncoding());

    std::string projectFullPath = nbprojectPath + "/project.xml";
    WriteToFile(project, projectFullPath,
      this->GlobalGenerator->GetMakefileEncoding());

    std::string makefilePath = target->GetName() + ".X/Makefile";
    std::string makefileFullPath = this->ConvertToFullPath(makefilePath);
    cmGeneratedFileStream makefileFileStream(makefileFullPath.c_str(), false,
      this->GlobalGenerator->GetMakefileEncoding());
    if (!makefileFileStream)
    {
      return;
    }

    makefileFileStream << makefileContents;

    if(fullProject && projectTool)
    {
      std::string message = "Creating makefiles for " + target->GetName() +
              " MPLABX project";
      this->GetCMakeInstance()->UpdateProgress(message.c_str(), -1);

      std::string command = std::string("\"") + projectTool + "\" \"" +
        this->ConvertToFullPath(target->GetName() + ".X") + "\"";

      if(!cmSystemTools::RunSingleCommand(command.c_str()))
      {
        cmSystemTools::Error("Project build failed: ", command.c_str());
      }
    }
  }
}

const std::string makefileContents =
R"(#
#  There exist several targets which are by default empty and which can be
#  used for execution of your targets. These targets are usually executed
#  before and after some main targets. They are:
#
#     .build-pre:              called before 'build' target
#     .build-post:             called after 'build' target
#     .clean-pre:              called before 'clean' target
#     .clean-post:             called after 'clean' target
#     .clobber-pre:            called before 'clobber' target
#     .clobber-post:           called after 'clobber' target
#     .all-pre:                called before 'all' target
#     .all-post:               called after 'all' target
#     .help-pre:               called before 'help' target
#     .help-post:              called after 'help' target
#
#  Targets beginning with '.' are not intended to be called on their own.
#
#  Main targets can be executed directly, and they are:
#
#     build                    build a specific configuration
#     clean                    remove built files from a configuration
#     clobber                  remove all built files
#     all                      build all configurations
#     help                     print help mesage
#
#  Targets .build-impl, .clean-impl, .clobber-impl, .all-impl, and
#  .help-impl are implemented in nbproject/makefile-impl.mk.
#
#  Available make variables:
#
#     CND_BASEDIR                base directory for relative paths
#     CND_DISTDIR                default top distribution directory
#                                (build artifacts)
#     CND_BUILDDIR               default top build directory
#                                (object files, ...)
#     CONF                       name of current configuration
#     CND_ARTIFACT_DIR_${CONF}   directory of build artifact
#                                (current configuration)
#     CND_ARTIFACT_NAME_${CONF}  name of build artifact (current configuration)
#     CND_ARTIFACT_PATH_${CONF}  path to build artifact (current configuration)
#     CND_PACKAGE_DIR_${CONF}    directory of package (current configuration)
#     CND_PACKAGE_NAME_${CONF}   name of package (current configuration)
#     CND_PACKAGE_PATH_${CONF}   path to package (current configuration)
#
# NOCDDL


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib


# build
build: .build-post

.build-pre:
# Add your pre 'build' code here...

.build-post: .build-impl
# Add your post 'build' code here...


# clean
clean: .clean-post

.clean-pre:
# Add your pre 'clean' code here...
# WARNING: the IDE does not call this target since it takes a long time to
# simply run make. Instead, the IDE removes the configuration directories
# under build and dist directly without calling make.
# This target is left here so people can do a clean when running a clean
# outside the IDE.

.clean-post: .clean-impl
# Add your post 'clean' code here...


# clobber
clobber: .clobber-post

.clobber-pre:
# Add your pre 'clobber' code here...

.clobber-post: .clobber-impl
# Add your post 'clobber' code here...


# all
all: .all-post

.all-pre:
# Add your pre 'all' code here...

.all-post: .all-impl
# Add your post 'all' code here...


# help
help: .help-post

.help-pre:
# Add your pre 'help' code here...

.help-post: .help-impl
# Add your post 'help' code here...



# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
)";
