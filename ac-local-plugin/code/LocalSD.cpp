// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
// #include "WhisperModelSchema.hpp"

// #include <ac/whisper/Instance.hpp>
// #include <ac/whisper/Init.hpp>
// #include <ac/whisper/Model.hpp>

// #include <ac/local/Instance.hpp>
// #include <ac/local/Model.hpp>
// #include <ac/local/ModelLoader.hpp>

// #include <astl/move.hpp>
// #include <astl/move_capture.hpp>
// #include <astl/iile.h>
// #include <astl/throw_stdex.hpp>
// #include <astl/workarounds.h>

// #include "aclp-whisper-version.h"
// #include "aclp-whisper-interface.hpp"

// namespace ac::local {

// namespace {

// class WhisperInstance final : public Instance {
//     std::shared_ptr<whisper::Model> m_model;
//     whisper::Instance m_instance;
// public:
//     using Schema = ac::local::schema::Whisper::InstanceGeneral;

//     WhisperInstance(std::shared_ptr<whisper::Model> model)
//         : m_model(astl::move(model))
//         , m_instance(*m_model, {})
//     {}

//     Dict run(Dict& params) {
//         Schema::OpTranscribe::Params schemaParams(params);

//         const auto& blob = schemaParams.audio.getValue();

//         auto pcmf32 = reinterpret_cast<const float*>(blob.data());
//         auto pcmf32Size = blob.size() / sizeof(float);

//         Dict ret;
//         Schema::OpTranscribe::Return result(ret);

//         result.result.setValue(m_instance.transcribe(std::span{pcmf32, pcmf32Size}));
//         return ret;
//     }

//     virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
//         switch (Schema::getOpIndexById(op)) {
//         case Schema::opIndex<Schema::OpTranscribe>:
//             return run(params);
//         default:
//             throw_ex{} << "whisper: unknown op: " << op;
//             MSVC_WO_10766806();
//         }
//     }
// };

// class WhisperModel final : public Model {
//     std::shared_ptr<whisper::Model> m_model;
// public:
//     using Schema = ac::local::schema::Whisper;

//     WhisperModel(const std::string& gguf, whisper::Model::Params params)
//         : m_model(std::make_shared<whisper::Model>(gguf.c_str(), astl::move(params)))
//     {}

//     virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict) override {
//         switch (Schema::getInstanceById(type)) {
//         case Schema::instanceIndex<Schema::InstanceGeneral>:
//             return std::make_unique<WhisperInstance>(m_model);
//         default:
//             throw_ex{} << "whisper: unknown instance type: " << type;
//             MSVC_WO_10766806();
//         }
//     }
// };

// class WhisperModelLoader final : public ModelLoader {
// public:
//     virtual const Info& info() const noexcept override {
//         static Info i = {
//             .name = "ac whisper.cpp",
//             .vendor = "Alpaca Core",
//             .inferenceSchemaTypes = {"whisper"},
//         };
//         return i;
//     }
//     virtual ModelPtr loadModel(ModelDesc desc, Dict /*params*/, ProgressCb /*progressCb*/) override {
//         if (desc.assets.size() != 1) throw_ex{} << "whisper: expected exactly one local asset";
//         auto& bin = desc.assets.front().path;
//         whisper::Model::Params modelParams;
//         return std::make_shared<WhisperModel>(bin, modelParams);
//     }
// };
// }

// } // namespace ac::local

// namespace ac::whisper {

// void init() {
//     initLibrary();
// }

// std::vector<ac::local::ModelLoaderPtr> getLoaders() {
//     std::vector<ac::local::ModelLoaderPtr> ret;
//     ret.push_back(std::make_unique<local::WhisperModelLoader>());
//     return ret;
// }

// local::PluginInterface getPluginInterface() {
//     return {
//         .label = "ac whisper.cpp",
//         .desc = "whisper.cpp plugin for ac-local",
//         .vendor = "Alpaca Core",
//         .version = astl::version{
//             ACLP_whisper_VERSION_MAJOR, ACLP_whisper_VERSION_MINOR, ACLP_whisper_VERSION_PATCH
//         },
//         .init = init,
//         .getLoaders = getLoaders,
//     };
// }

// } // namespace ac::whisper

