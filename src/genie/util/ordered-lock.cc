/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "ordered-lock.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

void OrderedLock::wait(size_t id) {
    std::unique_lock<std::mutex> lock(m);
    if (id == counter) {
        return;
    }
    cond_var.wait(lock, [&]() -> bool { return id == counter; });
}

// ---------------------------------------------------------------------------------------------------------------------

void OrderedLock::finished(size_t length) {
    {
        std::unique_lock<std::mutex> lock(m);
        counter += length;
    }
    cond_var.notify_all();
}

// ---------------------------------------------------------------------------------------------------------------------

OrderedLock::OrderedLock() : counter(0) {}

void OrderedLock::reset() { counter = 0; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------