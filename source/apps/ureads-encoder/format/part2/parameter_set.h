#ifndef GENIE_PARAMETER_SET_H
#define GENIE_PARAMETER_SET_H

/* ----------------------------------------------------------------------------------------------------------- */

#include <cstdint>
#include <memory>
#include <vector>

#include "data_unit.h"
#include "descriptor.h"
#include "qv_coding_config.h"
#include "gabac/gabac.h"

class BitWriter;
struct Parameter_set_crps {
    /*  enum class Cr_alg_ID : uint8_t {
          reserved = 0,
          RefTransform = 1,
          PushIn = 2,
          Local_Assembly = 3,
          Global_Assembly = 4
      };

      Cr_alg_ID cr_alg_ID;
      std::vector<Cr_info> cr_info;*/

    virtual void write(BitWriter &writer);
};

/**
* ISO 23092-2 Section 3.3.1 table 6 + 3.3.2 table 7 (fused for simplification)
*/
class Parameter_set : public Data_unit {
public:

    /**
    * ISO 23092-2 Section 5.1 table 34
    */
    enum class AlphabetID : uint8_t {
        ACGTN = 0, //!< Line 1
        ACGTRYSWKMBDHVN_ = 1 //!< Line 2
    };

    Parameter_set(
            uint8_t _parameter_set_ID,
            uint8_t _parent_parameter_set_ID,
            Dataset_type _dataset_type,
            AlphabetID _alphabet_id,
            uint32_t _read_length,
            bool _paired_end,
            bool _pos_40_bits_flag,
            uint8_t _qv_depth,
            uint8_t _as_depth,
            bool _multiple_alignments_flag,
            bool _spliced_reads_flag
    );

    void setCrps(std::unique_ptr<Parameter_set_crps> parameter_set_crps);

    void addClass(AU_type class_id, std::unique_ptr<Qv_coding_config> conf);

    void setDescriptor(uint8_t index, std::unique_ptr<Descriptor> descriptor);

    void addGroup(std::unique_ptr<std::string> rgroup_id);

    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    void write(BitWriter *writer) override;

private:

    /**
     * Incorporated (Simplification): ISO 23092-2 Section 3.1 table 3
     *
     * ------------------------------------------------------------------------------------------------------------ */

    uint16_t reserved : 10; //!< Line 8
    uint32_t data_unit_size : 22; //!< Line 9

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
    * ISO 23092-2 Section 3.3.2 table 6
    *
    * ------------------------------------------------------------------------------------------------------------- */

    uint8_t parameter_set_ID : 8; //!< Line 2
    uint8_t parent_parameter_set_ID : 8; //!< Line 3

    /** ----------------------------------------------------------------------------------------------------------- */

    /**
    * ISO 23092-2 Section 3.3.2 table 7
    *
    * ------------------------------------------------------------------------------------------------------------- */

    Dataset_type dataset_type; //!< : 4; Line 2
    AlphabetID alphabet_ID : 8; //!< Line 3
    uint32_t read_length : 24; //!< Line 4
    uint8_t number_of_template_segments_minus1 : 2; //!< Line 5
    uint8_t reserved_2 : 6; //!< Line 6
    uint32_t max_au_data_unit_size : 29; //!< Line 7
    uint8_t pos_40_bits_flag : 1; //!< Line 8
    uint8_t qv_depth : 3; //!< Line 9
    uint8_t as_depth : 3; //!< Line 10
    uint8_t num_classes : 4; //!< Line 11
    std::vector<AU_type> class_IDs; //!< : 4; For loop Lines 12 + 13
    std::vector<std::unique_ptr<Descriptor>> descriptors; //!< For loop lines 14 - 22
    uint16_t num_groups : 16; //!< Line 23
    std::vector<std::unique_ptr<std::string>> rgroup_IDs; //!< For Loop lines 24 + 25
    uint8_t multiple_alignments_flag : 1; //!< Line 26
    uint8_t spliced_reads_flag : 1; //!< Line 27
    uint32_t multiple_signature_base : 31; //!< Line 28
    std::unique_ptr<uint8_t> u_signature_size; //!< : 6; Line 29 + 30
    std::vector<std::unique_ptr<Qv_coding_config>> qv_coding_configs; //!< For loop lines 31 to 41
    uint8_t crps_flag : 1; //!< Line 42
    std::unique_ptr<Parameter_set_crps> parameter_set_crps; //!< Lines 43 + 44

    /** Padding in write() //!< Line 45 + 46 */

    /** ----------------------------------------------------------------------------------------------------------- */

    uint64_t internalBitCounter; // TODO: get rid of this
    void addSize(uint64_t bits);
};

/* ----------------------------------------------------------------------------------------------------------- */

#endif //GENIE_PARAMETER_SET_H
