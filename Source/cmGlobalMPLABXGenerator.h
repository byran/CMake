#ifndef cmGlobalMPLABXGenerator_h
#define cmGlobalMPLABXGenerator_h

#include "cmGlobalCommonGenerator.h"
#include "cmGlobalGeneratorFactory.h"

class cmGlobalMPLABXGenerator : public cmGlobalCommonGenerator
{
public:
  cmGlobalMPLABXGenerator(cmake* cm);

  static cmGlobalGeneratorFactory* NewFactory()
  {
    return new cmGlobalGeneratorSimpleFactory<cmGlobalMPLABXGenerator>();
  }

  ~cmGlobalMPLABXGenerator() CM_OVERRIDE {}

  static std::string GetActualName() { return "MPLABX"; }

  static void GetDocumentation(cmDocumentationEntry& entry);

  /**
 * Utilized by the generator factory to determine if this generator
 * supports toolsets.
 */
  static bool SupportsToolset() { return false; }

  /**
   * Utilized by the generator factory to determine if this generator
   * supports platforms.
   */
  static bool SupportsPlatform() { return false; }

  virtual bool SetGeneratorPlatform(std::string const& p,
    cmMakefile* mf) CM_OVERRIDE;

  virtual bool SetGeneratorToolset(std::string const& ts,
    cmMakefile* mf) CM_OVERRIDE;

  virtual std::string GetName() const CM_OVERRIDE
  {
    return cmGlobalMPLABXGenerator::GetActualName();
  }

  virtual bool FindMakeProgram(cmMakefile* mf) CM_OVERRIDE;

  virtual void Generate() CM_OVERRIDE;

  virtual void GenerateBuildCommand(
                            std::vector<std::string>& makeCommand,
                            const std::string& makeProgram,
                            const std::string& projectName,
                            const std::string& projectDir,
                            const std::string& targetName,
                            const std::string& config, bool fast, bool verbose,
                            std::vector<std::string> const& makeOptions =
                            std::vector<std::string>()) CM_OVERRIDE;

  virtual cmLocalGenerator* CreateLocalGenerator(
          cmMakefile* mf) CM_OVERRIDE;
};

#endif // ! cmGlobalMPLABXGenerator_h
