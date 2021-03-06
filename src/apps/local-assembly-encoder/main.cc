#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "encoding.h"
#include "program-options.h"

#include <util/exceptions.h>
#include <util/log.h>

static void writeCommandLine(int argc, char *argv[]) {
    std::vector<std::string> args(argv, (argv + argc));
    std::stringstream commandLine;
    for (const auto &arg : args) {
        commandLine << arg << " ";
    }
    LOG_INFO << "Command line: " << commandLine.str();
}

static int lae_main(int argc, char *argv[]) {
    try {
        lae::ProgramOptions programOptions(argc, argv);
        writeCommandLine(argc, argv);
        lae::encode(programOptions);
    } catch (const util::RuntimeException &e) {
        LOG_ERROR << "Internal runtime error: " << e.msg();
        return -1;
    } catch (const std::exception &e) {
        LOG_ERROR << "Standard library error: " << e.what();
        return -1;
    } catch (...) {
        LOG_ERROR << "Unknown error occurred";
        return -1;
    }

    return 0;
}

extern "C" void handleSignal(int sig) {
    // Ignore the signal
    std::signal(sig, SIG_IGN);

    // Get signal string and log it
    std::string signalString;
    switch (sig) {
        case SIGABRT:
            signalString = "SIGABRT";
            break;
        case SIGFPE:
            signalString = "SIGFPE";
            break;
        case SIGILL:
            signalString = "SIGILL";
            break;
        case SIGINT:
            signalString = "SIGINT";
            break;
        case SIGSEGV:
            signalString = "SIGSEGV";
            break;
        case SIGTERM:
            signalString = "SIGTERM";
            break;
        default:
            signalString = "unknown";
            break;
    }
    LOG_WARNING << "Caught signal: " << sig << " (" << signalString << ")";

    // Invoke the default signal action
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

int main(int argc, char *argv[]) {
    // Install signal handler for the following signal types:
    //   SIGABRT  abnormal termination condition, as is e.g. initiated by
    //            std::abort()
    //   SIGFPE   erroneous arithmetic operation such as divide by
    //            zero
    //   SIGILL   invalid program image, such as invalid instruction
    //   SIGINT   external interrupt, usually initiated by the user
    //   SIGSEGV  invalid memory access (segmentation fault)
    //   SIGTERM  termination request, sent to the program
    std::signal(SIGABRT, handleSignal);
    std::signal(SIGFPE, handleSignal);
    std::signal(SIGILL, handleSignal);
    std::signal(SIGINT, handleSignal);
    std::signal(SIGSEGV, handleSignal);
    std::signal(SIGTERM, handleSignal);

    // Fire up main method
    int rc = lae_main(argc, argv);
    if (rc != 0) {
        LOG_FATAL << "Failed to run";
    }

    // The C standard makes no guarantees as to when output to stdout or stderr
    // (standard error) is actually flushed. If e.g. stdout is directed to a
    // file and an error occurs while flushing the data (after program
    // termination), then the output may be lost. Thus we explicitly flush
    // stdout and stderr. On failure, we notify the operating system by
    // returning with EXIT_FAILURE.
    if (fflush(stdout) == EOF) {
        return EXIT_FAILURE;
    }
    if (fflush(stderr) == EOF) {
        return EXIT_FAILURE;
    }

    // Return to the operating system
    int osRc = (rc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    LOG_TRACE << "Return code: " << osRc;
    return osRc;
}
