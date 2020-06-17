/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SPRING_ENCODER_H
#define GENIE_SPRING_ENCODER_H

#include <genie/core/read-encoder.h>
#include <genie/core/stats/perf-stats.h>
#include "preprocess.h"

namespace genie {
namespace read {
namespace spring {

class Encoder : public genie::core::ReadEncoder {
   private:
    Preprocessor preprocessor;

   public:
    explicit Encoder(const std::string& working_dir, size_t num_thr, bool paired_end) {
        preprocessor.setup(working_dir, num_thr, paired_end);
    }
    void flowIn(genie::core::record::Chunk&& t, const util::Section& id) override;

    void flushIn(size_t& pos) override;

    void skipIn(const util::Section& id) override {
        preprocessor.skip(id);
        skipOut(id);
    }
};
}  // namespace spring
}  // namespace read
}  // namespace genie
#endif  // GENIE_ENCODER_H
