/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_WATCH_H
#define GENIE_WATCH_H

// ---------------------------------------------------------------------------------------------------------------------

#include <chrono>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
class Watch {
   private:
    std::chrono::high_resolution_clock::time_point start;  //!<
    double offset{};                                       //!<
    bool paused;                                           //!<

   public:
    /**
     *
     */
    Watch();

    /**
     *
     */
    void reset();

    /**
     *
     * @return
     */
    double check() const;

    /**
     *
     */
    void pause();

    /**
     *
     */
    void resume();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_WATCH_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------