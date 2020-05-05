/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_RLE_CODING_H_
#define GABAC_RLE_CODING_H_

#include <cstdint>
#include <genie/entropy/paramcabac/subsequence.h>
#include <genie/util/data-block.h>

namespace genie {
namespace entropy {
namespace gabac {

void transformRleCoding(const paramcabac::Subsequence& subseqCfg, std::vector<util::DataBlock> *const transformedSubseqs);

void inverseTransformRleCoding(const paramcabac::Subsequence& subseqCfg, std::vector<util::DataBlock> *const transformedSubseqs);
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_RLE_CODING_H_