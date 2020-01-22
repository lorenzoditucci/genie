/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-seq-conf-set.h"
#include <genie/core/parameter/descriptor_present/decoder.h>
#include <genie/core/parameter/descriptor_present/descriptor_present.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::GabacSeqConfSet() {
    const std::string DEFAULT_GABAC_CONF_JSON =
        "{"
        "\"word_size\": 4,"
        "\"sequence_transformation_id\": 0,"
        "\"sequence_transformation_parameter\": 0,"
        "\"transformed_sequences\":"
        "[{"
        "\"lut_transformation_enabled\": false,"
        "\"diff_coding_enabled\": false,"
        "\"binarization_id\": 0,"
        "\"binarization_parameters\":[32],"
        "\"context_selection_id\": 0"
        "}]"
        "}";

    // One configuration per subsequence
    for (const auto &desc : core::getDescriptors()) {
        conf.emplace_back();
        for (size_t i = 0; i < getDescriptor(desc.id).subseqs.size(); ++i) {
            conf.back().emplace_back(DEFAULT_GABAC_CONF_JSON);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const gabac::EncodingConfiguration &GabacSeqConfSet::getConfAsGabac(core::GenSubIndex sub) const {
    return conf[uint8_t(sub.first)][uint8_t(sub.second)];
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<GabacSeqConfSet::TransformSubseqParameters> GabacSeqConfSet::storeTransParams(
    const gabac::EncodingConfiguration &gabac_configuration) {
    using namespace entropy::paramcabac;

    // Build parameter
    auto mpeg_transform_id = TransformedParameters::TransformIdSubseq(gabac_configuration.sequenceTransformationId);
    auto trans_param = gabac_configuration.sequenceTransformationParameter;
    return util::make_unique<TransformedParameters>(mpeg_transform_id, trans_param);
}

// ---------------------------------------------------------------------------------------------------------------------

GabacSeqConfSet::TransformIdSubsym GabacSeqConfSet::storeTransform(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace entropy::paramcabac;

    SupportValues::TransformIdSubsym transform = SupportValues::TransformIdSubsym::NO_TRANSFORM;
    if (tSeqConf.lutTransformationEnabled && tSeqConf.diffCodingEnabled) {
        UTILS_THROW_RUNTIME_EXCEPTION("LUT and Diff core at the same time not supported");
    } else if (tSeqConf.lutTransformationEnabled) {
        transform = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    } else if (tSeqConf.diffCodingEnabled) {
        transform = SupportValues::TransformIdSubsym::DIFF_CODING;
    }
    return transform;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<GabacSeqConfSet::CabacBinarization> GabacSeqConfSet::storeBinarization(
    const gabac::TransformedSequenceConfiguration &tSeqConf) {
    using namespace entropy::paramcabac;

    auto bin_ID = BinarizationParameters::BinarizationId(tSeqConf.binarizationId);
    auto bin_params = util::make_unique<BinarizationParameters>(bin_ID, tSeqConf.binarizationParameters[0]);
    auto binarization = util::make_unique<Binarization>(bin_ID, std::move(bin_params));

    // Additional parameter for context adaptive modes
    if (tSeqConf.contextSelectionId != gabac::ContextSelectionId::bypass) {
        // TODO insert actual values when adaptive core ready
        auto context_params = util::make_unique<Context>(false, 3, 3, false);
        binarization->setContextParameters(std::move(context_params));
    }
    return binarization;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeSubseq(const gabac::EncodingConfiguration &gabac_configuration,
                                  DescriptorSubsequenceCfg &sub_conf) {
    using namespace entropy::paramcabac;

    size_t trans_seq_id = 0;
    for (const auto &tSeqConf : gabac_configuration.transformedSequenceConfigurations) {
        auto size = gabac::getTransformation(gabac_configuration.sequenceTransformationId).wordsizes[trans_seq_id] * 8;
        size = size ? size : gabac_configuration.wordSize * 8;

        auto transform = storeTransform(tSeqConf);
        auto binarization = storeBinarization(tSeqConf);
        auto supp_vals = util::make_unique<SupportValues>(size, size, 0, transform);
        auto subcfg = util::make_unique<TransformedSeq>(transform, std::move(supp_vals), std::move(binarization));
        sub_conf.setTransformSubseqCfg(trans_seq_id, std::move(subcfg));
        ++trans_seq_id;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::storeParameters(core::parameter::ParameterSet &parameterSet) const {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        auto descriptor_configuration = util::make_unique<core::parameter::desc_pres::DescriptorPresent>();

        if (desc.id == core::GenDesc::RNAME || desc.id == core::GenDesc::MSAR) {
            auto decoder_config = util::make_unique<DecoderTokenType>();
            fillDecoder(desc, decoder_config.get());
            descriptor_configuration->setDecoder(std::move(decoder_config));
        } else {
            auto decoder_config = util::make_unique<DecoderRegular>(desc.id);
            fillDecoder(desc, decoder_config.get());
            descriptor_configuration->setDecoder(std::move(decoder_config));
        }

        auto descriptor_container = core::parameter::DescriptorBox();
        descriptor_container.set(std::move(descriptor_configuration));

        parameterSet.setDescriptor(desc.id, std::move(descriptor_container));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const GabacSeqConfSet::DecoderConfigurationCabac &GabacSeqConfSet::loadDescriptorDecoderCfg(
    const GabacSeqConfSet::ParameterSet &parameterSet, core::GenDesc descriptor_id) {
    using namespace entropy::paramcabac;

    auto& curDesc = parameterSet.getDescriptor(descriptor_id);
    if (curDesc.isClassSpecific()) {
        UTILS_DIE("Class specific config not supported");
    }
    auto PRESENT = core::parameter::desc_pres::DescriptorPresent::PRESENT;
    auto base_conf = curDesc.get();
    if (base_conf->getPreset() != PRESENT) {
        UTILS_DIE("Config not present");
    }
    auto decoder_conf =
        reinterpret_cast<const core::parameter::desc_pres::DescriptorPresent &>(*base_conf).getDecoder();
    if (decoder_conf->getMode() != paramcabac::DecoderRegular::MODE_CABAC) {
        UTILS_DIE("Config is not paramcabac");
    }

    return reinterpret_cast<const DecoderRegular &>(*decoder_conf);
}

// ---------------------------------------------------------------------------------------------------------------------

gabac::TransformedSequenceConfiguration GabacSeqConfSet::loadTransformedSequence(
    const TransformSubseqCfg &transformedDesc) {
    using namespace entropy::paramcabac;
    gabac::TransformedSequenceConfiguration gabacTransCfg;

    const auto DIFF = SupportValues::TransformIdSubsym::DIFF_CODING;
    gabacTransCfg.diffCodingEnabled = transformedDesc.getTransformID() == DIFF;

    const auto LUT = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
    gabacTransCfg.lutTransformationEnabled = transformedDesc.getTransformID() == LUT;

    const auto MAX_BIN = BinarizationParameters::BinarizationId::SIGNED_TRUNCATED_EXPONENTIAL_GOLOMB;
    if (transformedDesc.getBinarization()->getBinarizationID() > MAX_BIN) {
        UTILS_DIE("Binarization unsupported");
    }

    const auto CUR_BIN = transformedDesc.getBinarization()->getBinarizationID();
    gabacTransCfg.binarizationId = gabac::BinarizationId(CUR_BIN);

    gabacTransCfg.binarizationParameters.push_back(32);  // TODO Remove hardcoded value
    if (transformedDesc.getBinarization()->getBypassFlag()) {
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId::bypass;
    } else {
        const auto CODING_ORDER = transformedDesc.getSupportValues()->getCodingOrder();
        gabacTransCfg.contextSelectionId = gabac::ContextSelectionId(CODING_ORDER + 1);
    }

    gabacTransCfg.lutOrder = 0;  // TODO Remove hardcoded value
    gabacTransCfg.lutBits = 0;   // TODO Remove hardcoded value

    return gabacTransCfg;
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacSeqConfSet::loadParameters(const core::parameter::ParameterSet &parameterSet) {
    using namespace entropy::paramcabac;

    for (const auto &desc : core::getDescriptors()) {
        auto &descConfig = loadDescriptorDecoderCfg(parameterSet, desc.id);
        for (const auto &subdesc : getDescriptor(desc.id).subseqs) {
            auto sub_desc = descConfig.getSubsequenceCfg(subdesc.id.second);
            auto sub_desc_id = desc.tokentype ? 0 : sub_desc->getDescriptorSubsequenceID();

            auto &gabac_conf = conf[uint8_t(desc.id)][sub_desc_id];

            gabac_conf.wordSize = 4;  // TODO Remove hardcoded value

            const auto TRANS_ID = sub_desc->getTransformParameters()->getTransformIdSubseq();
            gabac_conf.sequenceTransformationId = gabac::SequenceTransformationId(TRANS_ID);

            for (const auto &transformedDesc : sub_desc->getTransformSubseqCfgs()) {
                gabac_conf.transformedSequenceConfigurations.emplace_back();
                auto &gabacTransCfg = gabac_conf.transformedSequenceConfigurations.back();

                gabacTransCfg = loadTransformedSequence(*transformedDesc);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
