#ifndef GENIE_PROGRAM_OPTIONS_H_
#define GENIE_PROGRAM_OPTIONS_H_

#include <string>
#include <vector>

namespace ureads_encoder {

class ProgramOptions {
   public:
    ProgramOptions(int argc, char *argv[]);
    ~ProgramOptions();

   public:
    std::string inputFilePath;
    std::string pairFilePath;
    std::string outputFilePath;
    std::string outputPairFilePath;

   private:
    void processCommandLine(int argc, char *argv[]);
};

}  // namespace ureads_encoder

#endif  // GENIE_PROGRAM_OPTIONS_H_