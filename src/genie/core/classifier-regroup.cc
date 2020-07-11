/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "classifier-regroup.h"
#include <genie/util/watch.h>
#include "cigar-tokenizer.h"
#include "constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

ClassifierRegroup::ClassifierRegroup(size_t _auSize, ReferenceManager* rfmgr, RefMode mode)
    : refMgr(rfmgr), auSize(_auSize), refMode(mode) {
    currentChunks.resize(2);
    for (auto& c : currentChunks) {
        c.resize(2);
        for (auto& c2 : c) {
            c2.resize((uint8_t)record::ClassType::CLASS_U);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::Chunk ClassifierRegroup::getChunk() {
    if (refMode == RefMode::FULL) {
        auto seqvec = refMgr->getSequences();
        while (true) {
            if (refModeFullSeqID != seqvec.size()) {
                auto seq = seqvec.at(refModeFullSeqID);
                auto cov_vec = refMgr->getCoverage(seq);
                auto cov = cov_vec.at(refModeFullCovID);
                size_t chunkOffset = cov.first / refMgr->getChunkSize();

                if (!isWritten(seq, chunkOffset + refModeFullChunkID)) {
                    core::record::Chunk refChunk;

                    if (RAW_REFERENCE) {
                        refChunk.addRefToWrite((chunkOffset + refModeFullChunkID) * refMgr->getChunkSize(),
                                               (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize());
                        refChunk.getRef() =
                            refMgr->load(seq, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize(),
                                         (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize());
                        refChunk.setRefID(refMgr->ref2ID(seq));
                    } else {
                        refChunk.setReferenceOnly(true);
                        refChunk.setRefID(refMgr->ref2ID(seq));
                        refChunk.getRef() =
                            refMgr->load(seq, (chunkOffset + refModeFullChunkID) * refMgr->getChunkSize(),
                                         (chunkOffset + refModeFullChunkID + 1) * refMgr->getChunkSize());
                        core::record::Record rec(1, core::record::ClassType::CLASS_U, "", "", 0);
                        std::string ref_seq =  *refChunk.getRef().getChunkAt((chunkOffset + refModeFullChunkID) * refMgr->getChunkSize());
                        if(ref_seq.empty()) {
                            std::cerr << "empty" << std::endl;
                        }
                        core::record::Segment segment(std::move(ref_seq));
                        rec.addSegment(std::move(segment));
                        refChunk.getData().push_back(std::move(rec));
                    }

                    refModeFullChunkID++;
                    if (refModeFullChunkID > ((cov.second - 1)  / refMgr->getChunkSize())) {
                        refModeFullCovID++;
                        refModeFullChunkID = 0;
                    }
                    if (refModeFullCovID == cov_vec.size()) {
                        refModeFullCovID = 0;
                        refModeFullSeqID++;
                    }

                    return refChunk;
                } else {
                    refModeFullChunkID++;
                    if (refModeFullChunkID > ((cov.second - 1) / refMgr->getChunkSize())) {
                        refModeFullCovID++;
                        refModeFullChunkID = 0;
                    }
                    if (refModeFullCovID == cov_vec.size()) {
                        refModeFullCovID = 0;
                        refModeFullSeqID++;
                    }
                }
            } else {
                break;
            }
        }
    }

    auto ret = record::Chunk();
    if (finishedChunks.empty()) {
        return ret;
    }

    ret = std::move(finishedChunks.front());
    finishedChunks.erase(finishedChunks.begin());
    return ret;
}

record::ClassType ClassifierRegroup::fineClassifierECigar(const std::string& ref, const std::string& seq,
                                                          const std::string& ecigar) {
    record::ClassType classtype = record::ClassType::CLASS_P;

    auto classChecker = [&classtype, &ref, &seq](uint8_t cigar, const util::StringView& _bs,
                                                 const util::StringView& _rs) -> bool {
        auto bs = _bs.deploy(seq.data());
        auto rs = _rs.deploy(ref.data());
        // Possible mismatches not encoded in ecigar
        if (cigar == '=') {
            for (size_t i = 0; i < bs.length(); ++i) {
                // Mismatch
                if (*(bs.begin() + i) != *(rs.begin() + i)) {
                    if (*(bs.begin() + i) == 'N') {
                        classtype = record::ClassType::CLASS_N;
                    } else {
                        // Abort, class must be M
                        classtype = record::ClassType::CLASS_M;
                        return false;
                    }
                }
            }
            // Mismatch already encoded in ecigar
        } else if (core::getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar)) {
            if (cigar != 'N') {
                classtype = record::ClassType::CLASS_M;
                return false;
            }
            classtype = record::ClassType::CLASS_N;
            return true;
        }
        return true;
    };

    core::CigarTokenizer::tokenize(ecigar, core::getECigarInfo(), classChecker);
    return classtype;
}

record::ClassType ClassifierRegroup::fineClassifierRecord(ReferenceManager::ReferenceExcerpt& record_reference,
                                                          const core::record::Record& rec, bool loadonly) {
    record::ClassType ret = record::ClassType::CLASS_P;
    auto segment_it = rec.getSegments().begin();

    auto pos_s = rec.getPosition(0, 0);
    auto pos_e = pos_s + rec.getMappedLength(0, 0);

    if (record_reference.getRefName().empty()) {
        record_reference = ReferenceManager::ReferenceExcerpt(currentSeq, pos_s, pos_e);
    } else {
        record_reference.extend(pos_e);
    }

    record_reference.mapSection(pos_s, pos_e, refMgr);

    std::string ref = record_reference.getString(pos_s, pos_e);

    if (!loadonly) {
        ret = std::max(ret, fineClassifierECigar(ref, segment_it->getSequence(),
                                                 rec.getAlignments().front().getAlignment().getECigar()));
    }

    for (size_t i = 0; i < rec.getAlignments().front().getAlignmentSplits().size(); ++i) {
        if (ret == core::record::ClassType::CLASS_M) {
            return ret;
        }
        segment_it++;

        pos_s = rec.getPosition(0, i + 1);
        pos_e = pos_s + rec.getMappedLength(0, i + 1);

        record_reference.extend(pos_e);

        record_reference.mapSection(pos_s, pos_e, refMgr);

        ref = record_reference.getString(pos_s, pos_e);

        if (!loadonly) {
            ret = std::max(ret, fineClassifierECigar(ref, segment_it->getSequence(),
                                                     dynamic_cast<core::record::alignment_split::SameRec&>(
                                                         *rec.getAlignments().front().getAlignmentSplits()[i])
                                                         .getAlignment()
                                                         .getECigar()));
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::add(record::Chunk&& c) {
    record::Chunk chunk = std::move(c);

    // New reference
    if (chunk.getRef().getRefName() != currentSeq) {
        currentSeq = chunk.getRef().getRefName();
        currentSeqCoverage = refMgr->getCoverage(currentSeq);
        for (auto& refblock : currentChunks) {
            for (auto& pairblock : refblock) {
                for (auto& classblock : pairblock) {
                    if (classblock.getData().empty() ||
                        classblock.getData().front().getClassID() == record::ClassType::CLASS_U) {
                        continue;
                    }
                    queueFinishedChunk(classblock);
                }
            }
        }
    }

    for (auto& r : chunk.getData()) {
        auto classtype = coarseClassify(r);  // Only look at the ecigar for first classification
        bool paired = r.getNumberOfTemplateSegments() > 1;
        bool refBased = false;

        ReferenceManager::ReferenceExcerpt record_reference;

        // Unaligned reads can't be ref based, otherwise check if reference available
        if (classtype != core::record::ClassType::CLASS_U) {
            refBased = isCovered(r);
        }
        if (refBased) {
            // Load reference and do detailed classification
            if (classtype == record::ClassType::CLASS_M) {
                classtype = fineClassifierRecord(record_reference, r, false);
            } else {
                fineClassifierRecord(record_reference, r, true);
            }
        }
        r.setClassType(classtype);

        currentChunks[refBased][paired][(uint8_t)classtype - 1].getData().push_back(r);
        currentChunks[refBased][paired][(uint8_t)classtype - 1].getRef().merge(record_reference);
        if (currentChunks[refBased][paired][(uint8_t)classtype - 1].getData().size() == auSize) {
            auto& classblock = currentChunks[refBased][paired][(uint8_t)classtype - 1];
            queueFinishedChunk(classblock);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ClassifierRegroup::flush() {
    for (auto& refblock : currentChunks) {
        for (auto& pairblock : refblock) {
            for (auto& classblock : pairblock) {
                if (classblock.getData().empty() ||
                    classblock.getData().front().getClassID() == record::ClassType::CLASS_U) {
                    continue;
                }
                queueFinishedChunk(classblock);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifyECigar(const std::string& cigar) {
    auto highest = record::ClassType::CLASS_P;
    for (const auto& c : cigar) {
        if (c == '+' || c == '-' || c == ')' || c == ']') {
            return record::ClassType::CLASS_I;
        }
        if (getAlphabetProperties(AlphabetID::ACGTN).isIncluded(c)) {
            highest = record::ClassType::CLASS_M;
        }
    }
    return highest;
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::classifySeq(const std::string& seq) {
    for (const auto& c : seq) {
        if (c == 'N') {
            return record::ClassType::CLASS_N;
        }
    }
    return record::ClassType::CLASS_P;
}

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType ClassifierRegroup::coarseClassify(const record::Record& r) {
    if (r.getAlignments().empty()) {
        return record::ClassType::CLASS_U;
    }
    if (r.getAlignments().front().getAlignmentSplits().size() + 1 < r.getSegments().size()) {
        return record::ClassType::CLASS_HM;
    }

    record::ClassType highestClass = record::ClassType::CLASS_M;
    highestClass = std::max(highestClass, classifyECigar(r.getAlignments().front().getAlignment().getECigar()));
    if (highestClass == record::ClassType::CLASS_I) {
        return highestClass;
    }
    for (const auto& a : r.getAlignments().front().getAlignmentSplits()) {
        if (a->getType() != record::AlignmentSplit::Type::SAME_REC) {
            continue;
        }
        auto& split = dynamic_cast<const record::alignment_split::SameRec&>(*a);
        highestClass = std::max(highestClass, classifyECigar(split.getAlignment().getECigar()));
        if (highestClass == record::ClassType::CLASS_I) {
            return highestClass;
        }
    }

    if (highestClass >= record::ClassType::CLASS_M) {
        return highestClass;
    }

    for (const auto& s : r.getSegments()) {
        highestClass = std::max(highestClass, classifySeq(s.getSequence()));
        if (highestClass >= record::ClassType::CLASS_N) {
            return highestClass;
        }
    }

    return highestClass;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------