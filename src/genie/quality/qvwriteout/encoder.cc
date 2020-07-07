/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <core/record/alignment_split/same-rec.h>
#include <util/watch.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvwriteout {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::setUpParameters(const core::record::Chunk& rec, paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
    paramqv1::ParameterSet set;

    auto codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
    set.addCodeBook(std::move(codebook));

    desc.add(core::AccessUnit::Subsequence(4, core::GenSub::QV_PRESENT));
    desc.add(core::AccessUnit::Subsequence(4, core::GenSub::QV_CODEBOOK));
    desc.add(core::AccessUnit::Subsequence(4, core::GenSub::QV_STEPS_0));
    if (rec.getData().front().getClassID() == core::record::ClassType::CLASS_I ||
        rec.getData().front().getClassID() == core::record::ClassType::CLASS_HM) {
        desc.add(core::AccessUnit::Subsequence(4, core::GenSub::QV_STEPS_1));

        codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));
    }

    param.setQvps(std::move(set));
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeAlignedSegment(const core::record::Segment& s, const std::string& ecigar,
                                   core::AccessUnit::Descriptor& desc) {
    for (const auto& q : s.getQualities()) {
        core::CigarTokenizer::tokenize(
            ecigar, core::getECigarInfo(),
            [&desc, &q](uint8_t cigar, const util::StringView& bs, const util::StringView&) -> bool {
                auto qvs = bs.deploy(q.data());
                uint8_t codebook = core::getECigarInfo().lut_step_ref[cigar] ||
                                           core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)
                                       ? 2
                                       : desc.getSize() - 1;
                for (const auto& c : qvs) {
                    UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
                    desc.get(codebook).push(c - 33);
                }
                return true;
            });
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeUnalignedSegment(const core::record::Segment& s, core::AccessUnit::Descriptor& desc) {
    for (const auto& q : s.getQualities()) {
        for (const auto& c : q) {
            UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
            desc.get(desc.getSize() - 1).push(c - 33);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

core::QVEncoder::QVCoded Encoder::process(const core::record::Chunk& rec) {
    util::Watch watch;
    auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    setUpParameters(rec, *param, desc);

    for (const auto& r : rec.getData()) {
        size_t num_aligned_segs = r.getAlignments().empty() ? 0 : 1;
        if (num_aligned_segs) {
            num_aligned_segs += r.getAlignments().front().getAlignmentSplits().size();
        }

        size_t ctr = 0;
        for (const auto& s : r.getSegments()) {
            bool aligned = ctr < num_aligned_segs;
            if (aligned) {
                const std::string& cigar = ctr ? reinterpret_cast<const core::record::alignment_split::SameRec&>(
                                                     *r.getAlignments().front().getAlignmentSplits().at(ctr - 1))
                                                     .getAlignment()
                                                     .getECigar()
                                               : r.getAlignments().front().getAlignment().getECigar();
                encodeAlignedSegment(s, cigar, desc);
            } else {
                encodeUnalignedSegment(s, desc);
            }
            ctr++;
        }
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qv1writeout", watch.check());
    return {std::move(param), std::move(desc), stats};
}

// ---------------------------------------------------------------------------------------------------------------------

core::QVEncoder::QVCoded NoneEncoder::process(const core::record::Chunk&) {
    auto param = util::make_unique<paramqv1::QualityValues1>(paramqv1::QualityValues1::QvpsPresetId::ASCII, false);
    core::AccessUnit::Descriptor desc(core::GenDesc::QV);

    return {std::move(param), std::move(desc), core::stats::PerfStats()};
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvwriteout
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------