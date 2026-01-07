#include "ort_core/ort_core.hpp"
#include "ort_blob_buffer.hpp"
#include <map>
namespace easy_deploy {

enum BlobType { kINPUT = 0, kOUTPUT = 1 };

static const std::unordered_map<ONNXTensorElementDataType, size_t> map_tensor_type_byte_size_{
    {ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, 4},
    {ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32, 4},
    {ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32, 4},
    {ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64, 8},
    {ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64, 8}};

class OrtInferCore : public BaseInferCore {
public:
  ~OrtInferCore() override = default;

  OrtInferCore(const std::string                                             onnx_path,
               const std::unordered_map<std::string, std::vector<uint64_t>> &input_blobs_shape,
               const std::unordered_map<std::string, std::vector<uint64_t>> &output_blobs_shape,
               const int                                                     num_threads    = 0,
               const int                                                     ort_infer_type = 0);

  OrtInferCore(const std::string onnx_path, const int num_threads = 0);

  std::unique_ptr<BlobsTensor> AllocBlobsBuffer() override;

  InferCoreType GetType()
  {
    return InferCoreType::ONNXRUNTIME;
  }

  std::string GetName()
  {
    return "ort_core";
  }

private:
  bool PreProcess(std::shared_ptr<IPipelinePackage> buffer) override;

  bool Inference(std::shared_ptr<IPipelinePackage> buffer) override;

  bool PostProcess(std::shared_ptr<IPipelinePackage> buffer) override;

private:
  std::unordered_map<std::string, std::vector<uint64_t>> ResolveModelInputInformation();

  std::unordered_map<std::string, std::vector<uint64_t>> ResolveModelOutputInformation();

  std::unordered_map<std::string, void *> map_blob2ptr_;

  std::shared_ptr<Ort::Env> ort_env_;

  std::shared_ptr<Ort::Session> ort_session_;

  std::unordered_map<std::string, std::vector<uint64_t>> map_input_blob_name2shape_;
  std::unordered_map<std::string, std::vector<uint64_t>> map_output_blob_name2shape_;
};

#ifdef _WIN32
#include <windows.h>
inline std::wstring to_ort_path(const std::string &path)
{
  // Windows 下 ORTCHAR_T = wchar_t
  if (path.empty())
    return std::wstring();
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, path.c_str(), (int)path.size(), &wstr[0], size_needed);
  return wstr;
}
#endif

OrtInferCore::OrtInferCore(
    const std::string                                             onnx_path,
    const std::unordered_map<std::string, std::vector<uint64_t>> &input_blobs_shape,
    const std::unordered_map<std::string, std::vector<uint64_t>> &output_blobs_shape,
    const int                                                     num_threads,
    const int                                                     ort_infer_type)
{
  // onnxruntime session initialization
  LOG_DEBUG("start initializing onnxruntime session with onnx model {%s} ...", onnx_path.c_str());
  ort_env_ = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, onnx_path.data());
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(num_threads);
  session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
  session_options.SetLogSeverityLevel(4);

  // 配置执行提供器，不设置的话默认使用CPU
  /* CUDA Provider */
  if (ort_infer_type == 1)
  {
    OrtCUDAProviderOptions cuda_options;
    cuda_options.device_id = 0;
    session_options.AppendExecutionProvider_CUDA(cuda_options); // 添加CUDA执行提供器
  } else if (ort_infer_type == 2)
  {
    /* TensorRT Provider Options */
    std::map<std::string, std::string> trt_options;
    trt_options["device_id"]               = std::to_string(0);
    trt_options["trt_engine_cache_enable"] = "1";
    trt_options["trt_engine_cache_path"] =
        "/home/paco/work_prjs/my_tb_works/hjzl_3dr/assets/models";
    trt_options["trt_fp16_enable"]        = "1";
    trt_options["trt_int8_enable"]        = "0";
    trt_options["trt_max_workspace_size"] = std::to_string(8ULL * 1024 * 1024 * 1024);
    std::vector<const char *> keys;
    std::vector<const char *> values;
    for (const auto &[key, value] : trt_options)
    {
      keys.push_back(key.c_str());
      values.push_back(value.c_str());
    }
    OrtTensorRTProviderOptionsV2 *trt_options_v2 = nullptr;
    OrtApi const                 *api            = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    api->CreateTensorRTProviderOptions(&trt_options_v2);
    Ort::ThrowOnError(api->UpdateTensorRTProviderOptions(trt_options_v2, keys.data(), values.data(),
                                                         keys.size()));
    session_options.AppendExecutionProvider_TensorRT_V2(*trt_options_v2); // 添加TensorRT执行提供器
    api->ReleaseTensorRTProviderOptions(trt_options_v2);
  }

  // 加载模型
#ifdef _WIN32
  std::wstring onnx_path_w = to_ort_path(onnx_path);
  ort_session_ = std::make_shared<Ort::Session>(*ort_env_, onnx_path_w.c_str(), session_options);
#else
  ort_session_ = std::make_shared<Ort::Session>(*ort_env_, onnx_path.c_str(), session_options);
#endif
  
  LOG_DEBUG("successfully created onnxruntime session!");

  map_input_blob_name2shape_ =
      input_blobs_shape.empty() ? ResolveModelInputInformation() : input_blobs_shape;
  map_output_blob_name2shape_ =
      output_blobs_shape.empty() ? ResolveModelOutputInformation() : output_blobs_shape;

  // show info
  auto func_display_blobs_info =
      [](const std::unordered_map<std::string, std::vector<uint64_t>> &blobs_shape) {
        for (const auto &p_name_shape : blobs_shape)
        {
          std::string s_blob_shape;
          for (const auto dim : p_name_shape.second)
          {
            s_blob_shape += std::to_string(dim) + "\t";
          }
          LOG_DEBUG("blob name : %s, blob shape : %s", p_name_shape.first.c_str(),
                    s_blob_shape.c_str());
        }
      };

  func_display_blobs_info(input_blobs_shape);
  func_display_blobs_info(output_blobs_shape);

  BaseInferCore::Init();
}

std::unordered_map<std::string, std::vector<uint64_t>> OrtInferCore::ResolveModelInputInformation()
{
  std::unordered_map<std::string, std::vector<uint64_t>> ret;

  OrtAllocator *allocator    = nullptr;
  bool allocator_init_status = Ort::GetApi().GetAllocatorWithDefaultOptions(&allocator) == nullptr;
  CHECK_STATE_THROW(allocator_init_status, "[ort_core] Failed to get allocator!!!");

  const int input_blob_count = ort_session_->GetInputCount();

  for (int i = 0; i < input_blob_count; ++i)
  {
    const auto        blob_info       = ort_session_->GetInputTypeInfo(i);
    const auto        blob_type_shape = blob_info.GetTensorTypeAndShapeInfo();
    const auto        blob_shape      = blob_type_shape.GetShape();
    const auto        blob_name       = ort_session_->GetInputNameAllocated(i, allocator);
    const std::string s_blob_name     = std::string(blob_name.get());

    ret[s_blob_name]              = std::vector<uint64_t>();
    std::string s_blob_info       = std::string(blob_name.get()) + ":\t";
    size_t      blob_element_size = 1;
    for (size_t i = 0; i < blob_shape.size(); ++i)
    {
      if (blob_shape[i] < 0)
      {
        throw std::runtime_error(
            "auto resolve onnx model failed! \
                                        for blob shape < 0, please use explicit blob shape constructor!!");
      }
      s_blob_info += "\t" + std::to_string(blob_shape[i]);
      blob_element_size *= blob_shape[i];
      ret[s_blob_name].push_back(blob_shape[i]);
    }
    s_blob_info += "\ttotal elements: " + std::to_string(blob_element_size);
    LOG_DEBUG(s_blob_info.c_str());
  }

  return ret;
}

std::unordered_map<std::string, std::vector<uint64_t>> OrtInferCore::ResolveModelOutputInformation()
{
  std::unordered_map<std::string, std::vector<uint64_t>> ret;

  OrtAllocator *allocator    = nullptr;
  bool allocator_init_status = Ort::GetApi().GetAllocatorWithDefaultOptions(&allocator) == nullptr;
  CHECK_STATE_THROW(allocator_init_status, "[ort_core] Failed to get allocator!!!");

  const int output_blob_count = ort_session_->GetOutputCount();

  for (int i = 0; i < output_blob_count; ++i)
  {
    const auto        blob_info       = ort_session_->GetOutputTypeInfo(i);
    const auto        blob_type_shape = blob_info.GetTensorTypeAndShapeInfo();
    const auto        blob_shape      = blob_type_shape.GetShape();
    const auto        blob_name       = ort_session_->GetOutputNameAllocated(i, allocator);
    const std::string s_blob_name     = std::string(blob_name.get());

    ret[s_blob_name]              = std::vector<uint64_t>();
    std::string s_blob_info       = std::string(blob_name.get()) + ":\t";
    size_t      blob_element_size = 1;
    for (size_t i = 0; i < blob_shape.size(); ++i)
    {
      if (blob_shape[i] < 0)
      {
        throw std::runtime_error(
            "auto resolve onnx model failed! \
                                        for blob shape < 0, please use explicit blob shape constructor!!");
      }
      s_blob_info += "\t" + std::to_string(blob_shape[i]);
      blob_element_size *= blob_shape[i];
      ret[s_blob_name].push_back(blob_shape[i]);
    }
    s_blob_info += "\ttotal elements: " + std::to_string(blob_element_size);
    LOG_DEBUG(s_blob_info.c_str());
  }

  return ret;
}

std::unique_ptr<BlobsTensor> OrtInferCore::AllocBlobsBuffer()
{
  OrtAllocator *allocator    = nullptr;
  bool allocator_init_status = Ort::GetApi().GetAllocatorWithDefaultOptions(&allocator) == nullptr;
  CHECK_STATE_THROW(allocator_init_status, "[ort_core] Failed to get allocator!!!");

  std::unordered_map<std::string, std::unique_ptr<ITensor>> tensor_map;

  // input blobs
  const int input_blob_count = map_input_blob_name2shape_.size();
  for (int i = 0; i < input_blob_count; ++i)
  {
    auto tensor = std::make_unique<OrtTensor>();

    const auto        blob_name   = ort_session_->GetInputNameAllocated(i, allocator);
    const std::string s_blob_name = std::string(blob_name.get());
    const auto       &blob_shape  = map_input_blob_name2shape_[s_blob_name];

    auto tensor_type =
        ort_session_->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetElementType();
    CHECK_STATE_THROW(
        map_tensor_type_byte_size_.find(tensor_type) != map_tensor_type_byte_size_.end(),
        "[ort_core] Got invalid tensor type : %d", static_cast<uint32_t>(tensor_type));

    tensor->name_                      = s_blob_name;
    tensor->byte_size_per_element_     = map_tensor_type_byte_size_.at(tensor_type);
    tensor->current_shape_             = blob_shape;
    tensor->default_shape_             = blob_shape;
    tensor->self_maintain_buffer_host_ = std::make_unique<u_char[]>(tensor->GetTensorByteSize());
    tensor->buffer_on_host_            = tensor->self_maintain_buffer_host_.get();
    tensor->tensor_data_type_          = tensor_type;

    tensor_map.emplace(s_blob_name, std::move(tensor));
  }

  // output blobs
  const int output_blob_count = map_output_blob_name2shape_.size();
  for (int i = 0; i < output_blob_count; ++i)
  {
    auto tensor = std::make_unique<OrtTensor>();

    const auto        blob_name   = ort_session_->GetOutputNameAllocated(i, allocator);
    const std::string s_blob_name = std::string(blob_name.get());
    const auto       &blob_shape  = map_output_blob_name2shape_[s_blob_name];

    auto tensor_type =
        ort_session_->GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetElementType();
    CHECK_STATE_THROW(
        map_tensor_type_byte_size_.find(tensor_type) != map_tensor_type_byte_size_.end(),
        "[ort_core] Got invalid tensor type : %d", static_cast<uint32_t>(tensor_type));

    tensor->name_                      = s_blob_name;
    tensor->byte_size_per_element_     = map_tensor_type_byte_size_.at(tensor_type);
    tensor->current_shape_             = blob_shape;
    tensor->default_shape_             = blob_shape;
    tensor->self_maintain_buffer_host_ = std::make_unique<u_char[]>(tensor->GetTensorByteSize());
    tensor->buffer_on_host_            = tensor->self_maintain_buffer_host_.get();
    tensor->tensor_data_type_          = tensor_type;

    tensor_map.emplace(s_blob_name, std::move(tensor));
  }

  return std::make_unique<BlobsTensor>(std::move(tensor_map));
}

bool OrtInferCore::PreProcess(std::shared_ptr<IPipelinePackage> pipeline_unit)
{
  return true;
}

bool OrtInferCore::Inference(std::shared_ptr<IPipelinePackage> pipeline_unit)
{
  // 获取内存缓存
  CHECK_STATE(pipeline_unit != nullptr, "[ort_core] Inference got invalid pipeline_unit!");
  auto blobs_tensor = pipeline_unit->GetInferBuffer();
  CHECK_STATE(blobs_tensor != nullptr, "[ort_core] Inference got invalid blobs_tensor!");

  // 构造推理接口参数
  auto mem_info =
      Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtDeviceAllocator, OrtMemType::OrtMemTypeCPU);

  std::vector<const char *> input_blob_names;
  std::vector<const char *> output_blob_names;
  std::vector<Ort::Value>   input_blob_values;
  std::vector<Ort::Value>   output_blob_values;
  for (const auto &p_name_shape : map_input_blob_name2shape_)
  {
    const auto &blob_name = p_name_shape.first;
    auto        tensor    = dynamic_cast<OrtTensor *>(blobs_tensor->GetTensor(blob_name));
    CHECK_STATE(tensor != nullptr, "[ort_core] Inference got invalid tensor : %s",
                blob_name.c_str());

    input_blob_names.push_back(tensor->GetName().c_str());
    input_blob_values.push_back(
        Ort::Value::CreateTensor(mem_info, tensor->RawPtr(), tensor->GetTensorByteSize(),
                                 reinterpret_cast<const int64_t *>(tensor->GetShape().data()),
                                 tensor->GetShape().size(), tensor->tensor_data_type_));
  }

  for (const auto &p_name_shape : map_output_blob_name2shape_)
  {
    const auto &blob_name = p_name_shape.first;
    auto        tensor    = dynamic_cast<OrtTensor *>(blobs_tensor->GetTensor(blob_name));
    CHECK_STATE(tensor != nullptr, "[ort_core] Inference got invalid tensor : %s",
                blob_name.c_str());

    output_blob_names.push_back(tensor->GetName().c_str());
    output_blob_values.push_back(
        Ort::Value::CreateTensor(mem_info, tensor->RawPtr(), tensor->GetTensorByteSize(),
                                 reinterpret_cast<const int64_t *>(tensor->GetShape().data()),
                                 tensor->GetShape().size(), tensor->tensor_data_type_));
  }

  // 执行推理
  ort_session_->Run(Ort::RunOptions{nullptr}, input_blob_names.data(), input_blob_values.data(),
                    input_blob_names.size(), output_blob_names.data(), output_blob_values.data(),
                    output_blob_names.size());

  return true;
}

bool OrtInferCore::PostProcess(std::shared_ptr<IPipelinePackage> buffer)
{
  return true;
}

std::shared_ptr<BaseInferCore> CreateOrtInferCore(
    const std::string                                             onnx_path,
    const std::unordered_map<std::string, std::vector<uint64_t>> &input_blobs_shape,
    const std::unordered_map<std::string, std::vector<uint64_t>> &output_blobs_shape,
    const int                                                     num_threads,
    const int                                                     ort_infer_type)
{
  return std::make_shared<OrtInferCore>(onnx_path, input_blobs_shape, output_blobs_shape,
                                        num_threads, ort_infer_type);
}

} // namespace easy_deploy
