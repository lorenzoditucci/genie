/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FASTQ_EXPORTER_H
#define GENIE_FASTQ_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <util/drain.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace fastq {
/**
 * @brief Module to export MPEG-G record to fastq files
 */
class FastqExporter : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
    std::vector<std::ostream *> file;  //!< @brief Support for paired output files

   public:
    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit FastqExporter(std::ostream *_file_1);

    /**
     * @brief Paired mode
     * @param _file_1 Output file #1
     * @param _file_2 Output file #2
     */
    FastqExporter(std::ostream *_file_1, std::ostream *_file_2);

    /**
     * @brief Process one chunk of MPEGG records
     * @param records Input records
     * @param id Block identifier (for multithreading)
     */
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> records, size_t id) override;

    /**
     * @brief Accepts end of file marker
     */
    void dryIn() override;
};

}  // namespace fastq
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FASTQ_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------