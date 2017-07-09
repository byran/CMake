#ifndef cmLocalMPLABXGenerator_h
#define cmLocalMPLABXGenerator_h

#include "cmLocalGenerator.h"

#include <string>

class cmLocalMPLABXGenerator : public cmLocalGenerator
{
public:
  cmLocalMPLABXGenerator(cmGlobalGenerator* gg, cmMakefile* makefile);

  virtual void Generate() CM_OVERRIDE;

protected:
  std::string cmLocalMPLABXGenerator::ConvertToFullPath(
    const std::string& localPath);

  std::string IncludePathsForTarget(cmGeneratorTarget* target,
    char const* language) const;

  std::string PreprocessorMacrosForTarget(cmGeneratorTarget* target,
    char const* language) const;
};

#endif
