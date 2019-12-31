/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ALIGNMENT_H
#define GENIE_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

/**
 *
 */
class Alignment {
    std::string ecigar_string;           //!<
    uint8_t reverse_comp;                //!<
    std::vector<int32_t> mapping_score;  //!<

   public:
    /**
     *
     * @param _ecigar_string
     * @param _reverse_comp
     */
    Alignment(std::string&& _ecigar_string, uint8_t _reverse_comp);

    /**
     *
     * @param as_depth
     * @param reader
     */
    Alignment(uint8_t as_depth, util::BitReader& reader);

    /**
     *
     */
    Alignment();

    /**
     *
     * @param score
     */
    void addMappingScore(int32_t score);

    /**
     *
     * @return
     */
    const std::vector<int32_t>& getMappingScores() const;

    /**
     *
     * @return
     */
    const std::string& getECigar() const;

    /**
     *
     * @return
     */
    uint8_t getRComp() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ALIGNMENT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------