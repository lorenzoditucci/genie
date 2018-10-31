//
// Created by daniel on 23/10/18.
//

#ifndef MPEGG_REFERENCE_SW_ENCODINGPARAMETERS_H
#define MPEGG_REFERENCE_SW_ENCODINGPARAMETERS_H

#include <stdint-gcc.h>
#include <stdbool.h>
#include "DataStructures/BitStreams/OutputBitstream.h"

typedef enum {
    SubSeq_NO_TRANSFORM = 0,
    SubSeq_EQUALITY_CODING = 1,
    SubSeq_MATCH_CODING = 2,
    SubSeq_RLE_CODING = 3,
    SubSeq_RLE_QV_CODING = 4
} TransformSubSeqIdEnum;

typedef enum {
    SubSym_NO_TRANSFORM = 0,
    SubSym_LUT_TRANSFORM = 1,
    SubSym_DIFF_CODING = 2,
} TransformSubSymIdEnum;

typedef enum {
    BinarizationID_BinaryCoding = 0,
    BinarizationID_TruncatedUnary = 1,
    BinarizationID_ExponentialGolomb = 2,
    BinarizationID_SignedExponentialGolomb = 3,
    BinarizationID_TruncatedExponentialGolomb = 4,
    BinarizationID_SignedTruncatedExponentialGolomb = 5,
    BinarizationID_SplitUnitWiseTruncatedUnary = 6,
    BinarizationID_SignedSplitUnitWiseTruncatedUnary = 7,
    BinarizationID_DoubleTruncatedUnary = 8,
    BinarizationID_SignedDoubleTruncatedUnary = 9
} BinarizationIdEnum;

typedef enum {
    SUCCESS = 0,
    NULL_PTR = -1,
    FIELDs_EXISTANCE_CONDITIONS_NOT_MET = -2,
    OUT_OF_BOUNDERIES = -3
} EncodingParametersRC;

typedef enum {
    CrAlg_NoComp = 0,
    CrAlg_RefTransform = 1,
    CrAlg_PushIn = 2,
    CrAlg_LocalAssembly = 3,
    CrAlg_GlobalAssembly = 4
} CrAlgEnum;

typedef struct{
    TransformSubSeqIdEnum transform_ID_subseq;
    uint16_t match_coding_buffer_size;
    uint8_t rle_coding_guard;
} Transform_subseq_parametersType;

//METHODS START

Transform_subseq_parametersType* constructNoTransformSubseqParameters();
Transform_subseq_parametersType* constructEqualityCodingTransformSubseqParameters();
Transform_subseq_parametersType* constructMatchCodingSubseqTransformSubseqParameters(uint16_t match_coding_buffer_size);
Transform_subseq_parametersType* constructRLECodingSubseqTransformSubseqParameters(uint8_t rle_coding_guard);
Transform_subseq_parametersType* constructRLEQVCodingSubseqTransformSubseqParameters();
EncodingParametersRC getTransformSubSeqId(
        Transform_subseq_parametersType* transformSubseqParameters,
        TransformSubSeqIdEnum* output
);
EncodingParametersRC getMatchCodingBufferSize(
        Transform_subseq_parametersType* transformSubseqParameters,
        uint16_t* output
);
EncodingParametersRC getRLECodingGuard(
        Transform_subseq_parametersType* transformSubseqParameters,
        uint8_t* output
);
//------------------ until here tested---------------------------

typedef struct {
    uint8_t output_symbol_size;
    uint8_t coding_symbol_size;
    uint8_t coding_order;
    bool share_subsym_lut_flag;
    bool share_subsym_prv_flag;
} Support_valuesType;

typedef struct {
    bool bIFullCtxMode;
    uint8_t cTruncExpGolParam;
    uint8_t cMaxDTU;
    uint8_t splitUnitSize;
} Cabac_binarization_parametersType;

typedef struct {
    bool adaptive_mode_flag;
    uint16_t num_contexts;
    uint8_t* context_initialization_value;
    bool share_sub_sym_ctx_flag;
    bool segment_ctx_flag;
    uint16_t segment_ctx_len;
    uint16_t segment_ctx_offset;
} Cabac_context_parametersType;

EncodingParametersRC getAdaptiveModeFlag(
    Cabac_context_parametersType* cabac_context_parameters,
    bool* adaptive_mode_flag
);
EncodingParametersRC getContextInitializationValues(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* num_contexts,
        uint8_t** context_initialization_value
);
EncodingParametersRC getSegmentCtxFlag(
        Cabac_context_parametersType* cabac_context_parameters,
        bool* segment_ctx_flag
);
EncodingParametersRC getSegmentCtxLen(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* segment_ctx_len
);
EncodingParametersRC getSegmentCtxOffset(
        Cabac_context_parametersType* cabac_context_parameters,
        uint16_t* segment_ctx_offset
);

typedef struct {
    BinarizationIdEnum binarization_ID;
    bool bypass_flag;
    Cabac_binarization_parametersType* cabac_binarization_parameters;
    Cabac_context_parametersType* cabac_context_parameters;
} Cabac_binarizationsType;
EncodingParametersRC getBinarizationId(Cabac_binarizationsType* cabac_binarizations, uint8_t* binarizationId);
EncodingParametersRC getBypassFlag(Cabac_binarizationsType* cabac_binarizations, bool* bypassFlag);
EncodingParametersRC getBIFullCtxMode(
        Cabac_binarizationsType* cabac_binarizations,
        bool* biFullCtxMode
);
EncodingParametersRC getCTruncExpGolParam(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* cTruncExpGolParam
);
EncodingParametersRC getcMaxDTU(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* cMaxDTU
);
EncodingParametersRC getSplitUnitSize(
        Cabac_binarizationsType* cabac_binarizations,
        uint8_t* splitUnitSize
);
EncodingParametersRC getCabacContextParameters(
        Cabac_binarizationsType* cabac_binarizations,
        Cabac_context_parametersType** cabac_context_parameters
);
Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_NotBypass(
        bool BIFullCtxMode,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationBinaryCoding_Bypass(
        bool BIFullCtxMode
);
Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationTruncatedUnary_Bypass();
Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationExponentialGolomb_Bypass();
Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_NotBypass(
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationSignedExponentialGolomb_Bypass();
Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_NotBypass(
        uint8_t cTruncExpGolParam,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationTruncatedExponentialGolomb_Bypass(
        uint8_t cTruncExpGolParam
);
Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_NotBypass(
        uint8_t cTruncExpGolParam,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationSignedTruncatedExponentialGolomb_Bypass(
        uint8_t cTruncExpGolParam
);
Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_NotBypass(
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationSplitUnitwiseTruncatedUnary_Bypass(
        uint8_t splitUnitSize
);
Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_NotBypass(
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationSignedSplitUnitwiseTruncatedUnary_Bypass(
        uint8_t splitUnitSize
);
Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_NotBypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationDoubleTruncatedUnary_Bypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
);
Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_NotBypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize,
        bool adaptive_mode_flag,
        uint16_t num_contexts,
        uint8_t* context_initialization_value,
        bool share_sub_sym_ctx_flag,
        bool segment_ctx_flag,
        uint16_t segment_ctx_len,
        uint16_t segment_ctx_offset
);
Cabac_binarizationsType* constructCabacBinarizationSignedDoubleTruncatedUnary_Bypass(
        uint8_t cMaxDTU,
        uint8_t splitUnitSize
);

typedef struct {
    uint8_t num_descriptor_subsequence_cfgs_minus1;
    uint16_t* descriptor_subsequence_ID; //todo correct writter, add getter
    Transform_subseq_parametersType** transform_subseq_parameters; //13.1.2.4
    TransformSubSymIdEnum** transform_id_subsym; //13.1.2.4
    Support_valuesType*** support_values; //13.1.2.2
    Cabac_binarizationsType*** cabac_binarizations; //13.1.2.3
} DecoderConfigurationTypeCABAC;



EncodingParametersRC getNumDescriptorSubsequenceCfgsMinus1(
    DecoderConfigurationTypeCABAC* decoderConfigurationCABAC,
    uint8_t* num_descriptor_subsequence_cfgs_minus1
);
EncodingParametersRC getDescriptorSubsequenceId(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t desciptorSubsequence_i,
        uint16_t* desciptorSubsequenceId
);
EncodingParametersRC getTransformSubseqParameters(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequence_i,
        Transform_subseq_parametersType** transform_subseq_parameter
);
EncodingParametersRC getTransformSubseqCounter(
        DecoderConfigurationTypeCABAC *decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint8_t *transformSubseqCounter
);
EncodingParametersRC getTransformIdSubSym(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        TransformSubSymIdEnum* transformIdSubSym
);
EncodingParametersRC getCABACBinarizations(
        DecoderConfigurationTypeCABAC *decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        Cabac_binarizationsType **cabacBinarizations
);
EncodingParametersRC getSupportValueOutputSymbolSize(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        uint8_t* outputSymbolSize
);
EncodingParametersRC getSupportValueCodingSymbolSize(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        uint8_t* codingSymbolSize
);
EncodingParametersRC getSupportValueCodingOrder(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        uint8_t* codingOrder
);
EncodingParametersRC getSupportValueShareSubsymLUTFlag(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        bool* share_subsym_lut_flag
);
EncodingParametersRC getSupportValueShareSubsymPRVFlag(
        DecoderConfigurationTypeCABAC* decoderConfigurationTypeCABAC,
        uint16_t descriptorSubsequenceId,
        uint16_t transformSubseqIndex,
        bool* share_subsym_prv_flag
);


typedef struct {
    uint8_t output_symbol_size;
    uint8_t coding_symbols_numbers;
    uint8_t coding_symbol_size[64];
    Support_valuesType* support_values;
    Cabac_binarizationsType* cabac_binarizations;
    TransformSubSymIdEnum transformSubSym;
} Decoder_configuration_tokentype_cabac;

EncodingParametersRC getOutputSymbolSizeToken(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
    uint8_t* output_symbol_size
);
EncodingParametersRC getCodingSymbolsNumbers(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
    uint8_t* coding_symbols_numbers
);
EncodingParametersRC getCodingSymbolSizes(
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac,
    uint8_t* coding_symbols_numbers,
    uint8_t** coding_symbol_size
);
EncodingParametersRC getCabacBinarizationsToken(
        Decoder_configuration_tokentype_cabac *decoder_configuration_tokentype_cabac,
        Cabac_binarizationsType **cabacBinarizations
);

typedef struct {
    uint8_t rle_guard_tokentype;
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac0;
    Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac1;
} Decoder_configuration_tokentype;
EncodingParametersRC getRLEGuardTokenType(
    Decoder_configuration_tokentype* decoder_configuration_tokentype,
    uint8_t* rle_guard_tokentype
);
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order0(
    Decoder_configuration_tokentype* decoder_configuration_tokentype,
    Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac
);
EncodingParametersRC getDecoderConfigurationTokentypeCabac_order1(
        Decoder_configuration_tokentype* decoder_configuration_tokentype,
        Decoder_configuration_tokentype_cabac** decoder_configuration_tokentype_cabac
);

typedef struct {
    uint8_t qv_num_codebooks_total;
    uint8_t* qv_num_codebook_entries;
    uint8_t** qv_recon;
} Parameter_set_qvpsType;
EncodingParametersRC getQVNumCodebooksTotal(
    Parameter_set_qvpsType* parameter_set_qvps,
    uint8_t* qv_num_codebooks_total
);
EncodingParametersRC getQVNumCodebooksEntries(
    Parameter_set_qvpsType* parameter_set_qvps,
    uint8_t qv_num_codebook_index,
    uint8_t* qv_num_codebook_entries,
    uint8_t** qv_recon
);

typedef struct {
    CrAlgEnum cr_alg_ID;
    uint8_t cr_pad_size;
    uint32_t cr_buf_max_size;
} Parameter_set_crpsType;

EncodingParametersRC getCrAlgId(
    Parameter_set_crpsType* parameterSetCrps,
    CrAlgEnum* cr_alg_ID
);
EncodingParametersRC getCrPadSize(
    Parameter_set_crpsType* parameterSetCrps,
    uint8_t* cr_pad_size
);
EncodingParametersRC getCrBufMaxSize(
    Parameter_set_crpsType* parameterSetCrps,
    uint32_t* cr_buf_max_size
);

typedef struct {
    uint8_t dataset_type;
    uint8_t alphabet_ID;
    uint32_t reads_length;
    uint8_t number_of_template_segments_minus1;
    uint32_t max_au_data_unit_size;
    bool pos_40_bits;
    uint8_t qv_depth;
    uint8_t as_depth;
    uint8_t num_classes;
    uint8_t* classID;
    bool class_specific_dec_cfg_flag[18];
    bool** dec_cfg_flag;
    uint8_t** encoding_mode_id;
    void*** decoderConfiguration; //13.1.2 //todo correct getter
    uint16_t num_groups;
    char** rgroup_ID;
    bool spliced_reads_flag;
    bool multiple_signature_flag;
    uint32_t multiple_signature_base;
    uint8_t U_signature_size;
    bool multiple_alignments_flag;
    uint8_t mscore_mantissa;
    uint8_t* qv_coding_mode;
    bool* qvps_flag;
    Parameter_set_qvpsType** parameter_set_qvps; //8.1
    uint8_t* default_qvps_ID;
    bool crps_flag;
    Parameter_set_crpsType* parameter_set_crps; //8.2
} Encoding_ParametersType;

EncodingParametersRC getDatasetTypeParameters(Encoding_ParametersType* encodingParameters, uint8_t* datasetType);
EncodingParametersRC getAlphabetID(Encoding_ParametersType* encodingParameters, uint8_t* alphabetId);
EncodingParametersRC getReadsLength(Encoding_ParametersType* encodingParameters, uint32_t* readsLength);
EncodingParametersRC getNumberOfTemplateSegmentsMinus1(
    Encoding_ParametersType* encodingParameters,
    uint8_t* numberOfTemplateSegmentsMinus1
);
EncodingParametersRC getMaxAUDataUnitSize(Encoding_ParametersType* encodingParameters, uint32_t* maxAUDataUnitSize);
EncodingParametersRC getPos40Bits(Encoding_ParametersType* encodingParameters, bool* pos40Bits);
EncodingParametersRC getQVDepth(Encoding_ParametersType* encodingParameters, uint8_t* qv_depth);
EncodingParametersRC getASDepth(Encoding_ParametersType* encodingParameters, uint8_t* as_depth);
EncodingParametersRC getNumClasses(Encoding_ParametersType* encodingParameters, uint8_t* numClasses);
EncodingParametersRC getClassIds(Encoding_ParametersType* encodingParameters, uint8_t* numClasses, uint8_t** classID);
EncodingParametersRC getCABACDecoderConfiguration(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    uint8_t descriptorID,
    DecoderConfigurationTypeCABAC** decoder_configuration_cabac
);
EncodingParametersRC getCABACDecoderConfigurationTokenType(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    uint8_t descriptorID,
    Decoder_configuration_tokentype** decoderConfigurationTokentypeCABAC
);
EncodingParametersRC getNumGroups(Encoding_ParametersType* encodingParameters, uint16_t* numGroups);
EncodingParametersRC getReadGroupId(Encoding_ParametersType* encodingParameters, uint16_t groupIndex, char** rgroupID);
EncodingParametersRC getSplicedReadsFlag(Encoding_ParametersType* encodingParameters, bool* splicedReadsFlag);
EncodingParametersRC getMultipleSignatureFlag(
    Encoding_ParametersType* encodingParameters,
    bool* multipleSignatureFlag
);
EncodingParametersRC getMultipleSignatureBaseParameters(
    Encoding_ParametersType* encodingParameters,
    uint32_t* multiple_signature_base
);
EncodingParametersRC getSignatureSize(
    Encoding_ParametersType* encodingParameters,
    uint8_t* u_signature_size
);
EncodingParametersRC getMultipleAlignments_flag(
    Encoding_ParametersType* encodingParameters,
    bool* multiple_alignments_flag
);
EncodingParametersRC getMscoreMantissa(
    Encoding_ParametersType* encodingParameters,
    uint8_t* mscore_mantissa
);
EncodingParametersRC getQVCodingMode(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    uint8_t* qv_coding_mode
);
EncodingParametersRC getQVPSflag(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    bool* qvps_flag
);
EncodingParametersRC getParameterSetQvps(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    Parameter_set_qvpsType** pParameter_set_qvpsType
);
EncodingParametersRC getDefaultQvpsId(
    Encoding_ParametersType* encodingParameters,
    uint8_t classIndex,
    uint8_t* defaultQvpsId
);
EncodingParametersRC getCrpsFlag(
    Encoding_ParametersType* encodingParameters,
    bool* crps_flag
);
EncodingParametersRC getParameterSetCrps(
    Encoding_ParametersType* encodingParameters,
    Parameter_set_crpsType** ParameterSetCrps
);
bool writeEncoding_parameters(
    Encoding_ParametersType* encodingParameters,
    OutputBitstream* outputBitstream
);
Encoding_ParametersType* constructEncodingParametersSingleAlignmentNoComputed(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size
);
Encoding_ParametersType* constructEncodingParametersMultipleAlignmentNoComputed(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        uint8_t mscore_mantissa
);
Encoding_ParametersType* constructEncodingParametersSingleAlignmentComputedRef(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        Parameter_set_crpsType* parameter_set_crps
);
Encoding_ParametersType* constructEncodingParametersMultipleAlignmentComputedRef(
        uint8_t datasetType,
        uint8_t alphabetId,
        uint32_t reads_length,
        uint8_t number_of_template_segments_minus1,
        uint32_t max_au_data_unit_size,
        bool pos40bits,
        uint8_t qv_depth,
        uint8_t as_depth,
        uint8_t numClasses,
        uint8_t* classID,
        uint16_t numGroups,
        char** rgroupId,
        bool splicedReadsFlag,
        bool multipleSignatureFlag,
        uint32_t multipleSignatureBase,
        uint8_t U_signature_size,
        uint8_t mscore_mantissa,
        Parameter_set_crpsType* parameter_set_crps
);



Support_valuesType* constructSupportValues(
    uint8_t output_symbol_size,
    uint8_t coding_symbol_size,
    uint8_t coding_order,
    bool share_subsym_lut_flag,
    bool share_subsym_prv_flag
);
DecoderConfigurationTypeCABAC* constructDecoderConfigurationCABAC(
        uint8_t num_descriptor_subsequence_cfgs_minus1,
        uint16_t* descriptor_subsequence_ID,
        Transform_subseq_parametersType** transform_subseq_parameters,
        TransformSubSymIdEnum** transform_id_subsym,
        Support_valuesType*** support_values,
        Cabac_binarizationsType*** cabac_binarizations
);

Decoder_configuration_tokentype_cabac* contructDecoder_configuration_tokentypeCABAC(
        uint8_t output_symbol_size,
        uint8_t coding_symbols_numbers,
        const uint8_t coding_symbol_size[64],
        TransformSubSymIdEnum transformSubSymId,
        Support_valuesType* support_values,
        Cabac_binarizationsType* cabac_binarizations
);
Decoder_configuration_tokentype* constructDecoderConfigurationTokentype(
        uint8_t rle_guard_tokentype,
        Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac0,
        Decoder_configuration_tokentype_cabac* decoder_configuration_tokentype_cabac1
);
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportOutputSymbolSize(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* outputSymbolSize
);
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingSymbolSize(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* outputSymbolSize
);
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportCodingOrder(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        uint8_t* codingOrder
);
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymLutFlag(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        bool* shareSubsymLutFlag
);
EncodingParametersRC getDecoderConfigurationTokentypeCabacSupportShareSubsymPrvFlag(
        Decoder_configuration_tokentype_cabac* decoderConfigurationTokentypeCabac,
        bool* shareSubsymPrvFlag
);


#endif //MPEGG_REFERENCE_SW_ENCODINGPARAMETERS_H
