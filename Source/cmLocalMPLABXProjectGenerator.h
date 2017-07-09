#ifndef cmLocalMPLABXProjectGenerator_h
#define cmLocalMPLABXProjectGenerator_h

#include <string>
#include <vector>
#include <ostream>

class cmLocalMPLABXProjectGenerator
{
public:
  std::string name{ "" };
  std::vector<std::string> dependencies;

  void WriteToStream(std::ostream& output) const;
};

#endif
