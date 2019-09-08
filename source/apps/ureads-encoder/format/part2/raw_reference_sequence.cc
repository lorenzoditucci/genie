#include <ureads-encoder/exceptions.h>
#include "raw_reference_sequence.h"
#include "make_unique.h"
#include "bitwriter.h"

namespace format {
    RawReferenceSequence::RawReferenceSequence(
            uint16_t _sequence_ID,
            uint64_t _seq_start,
            uint64_t _seq_end,
            std::unique_ptr<std::string> _ref_sequence
    ) : sequence_ID(_sequence_ID),
        seq_start(_seq_start),
        seq_end(_seq_end),
        ref_sequence(std::move(_ref_sequence)) {
        if(ref_sequence->length() != seq_end - seq_start) {
            GENIE_THROW_RUNTIME_EXCEPTION("Invalid reference length");
        }
        if(seq_end <= seq_start) {
            GENIE_THROW_RUNTIME_EXCEPTION("Reference end before reference start");
        }
    }

    std::unique_ptr<RawReferenceSequence> RawReferenceSequence::clone() const {
        return make_unique<RawReferenceSequence>(sequence_ID, seq_start, seq_end, make_unique<std::string>(*ref_sequence));
    }

    void RawReferenceSequence::write(BitWriter *writer) const{
        writer->write(sequence_ID, 16);
        writer->write(seq_start, 40);
        writer->write(seq_end, 40);
        for(const auto &a : *ref_sequence) {
            writer->write(a, 8);
        }
        writer->write('\0', 8);
    }

    bool RawReferenceSequence::isIdUnique(const RawReferenceSequence* s) const {
        return sequence_ID != s->sequence_ID;
    }

    uint64_t RawReferenceSequence::getTotalSize() const {
        uint64_t HEADER_SIZE = (16 + 40 + 40)/8; // sequence_ID, seq_start, seq_end
        return (ref_sequence->length() + 1) + HEADER_SIZE; // Including \0
    }
}