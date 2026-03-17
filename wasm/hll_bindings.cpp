#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "hll.hpp"

namespace {

using emscripten::allow_raw_pointers;
using emscripten::class_;
using emscripten::enum_;
using emscripten::optional_override;
using emscripten::val;

std::vector<std::uint8_t> from_uint8_array(const val& input) {
  const auto length = input["length"].as<unsigned>();
  std::vector<std::uint8_t> bytes(length);
  if (length > 0) {
    val memory_view = val(emscripten::typed_memory_view(length, bytes.data()));
    memory_view.call<void>("set", input);
  }
  return bytes;
}

template<typename ByteVector>
val to_uint8_array(const ByteVector& bytes) {
  val output = val::global("Uint8Array").new_(bytes.size());
  if (!bytes.empty()) {
    output.call<void>("set", val(emscripten::typed_memory_view(bytes.size(), bytes.data())));
  }
  return output;
}

class HllSketchBinding {
public:
  HllSketchBinding(std::uint8_t lg_config_k, datasketches::target_hll_type target_type, bool start_full_size)
    : sketch_(lg_config_k, target_type, start_full_size) {}

  explicit HllSketchBinding(datasketches::hll_sketch sketch)
    : sketch_(std::move(sketch)) {}

  void reset() { sketch_.reset(); }

  void update_string(const std::string& value) { sketch_.update(value); }
  void update_uint64(std::uint64_t value) { sketch_.update(value); }
  void update_uint32(std::uint32_t value) { sketch_.update(value); }
  void update_int64(std::int64_t value) { sketch_.update(value); }
  void update_int32(std::int32_t value) { sketch_.update(value); }
  void update_double(double value) { sketch_.update(value); }
  void update_float(float value) { sketch_.update(value); }

  void update_bytes(const val& input) {
    const auto bytes = from_uint8_array(input);
    sketch_.update(bytes.empty() ? nullptr : bytes.data(), bytes.size());
  }

  double get_estimate() const { return sketch_.get_estimate(); }
  double get_composite_estimate() const { return sketch_.get_composite_estimate(); }
  double get_lower_bound(std::uint8_t num_std_dev) const { return sketch_.get_lower_bound(num_std_dev); }
  double get_upper_bound(std::uint8_t num_std_dev) const { return sketch_.get_upper_bound(num_std_dev); }
  std::uint8_t get_lg_config_k() const { return sketch_.get_lg_config_k(); }
  datasketches::target_hll_type get_target_type() const { return sketch_.get_target_type(); }
  bool is_compact() const { return sketch_.is_compact(); }
  bool is_empty() const { return sketch_.is_empty(); }
  std::uint32_t get_compact_serialization_bytes() const { return sketch_.get_compact_serialization_bytes(); }
  std::uint32_t get_updatable_serialization_bytes() const { return sketch_.get_updatable_serialization_bytes(); }

  std::string to_string(bool summary, bool detail, bool aux_detail, bool all) const {
    return sketch_.to_string(summary, detail, aux_detail, all);
  }

  val serialize_compact(unsigned header_size_bytes) const {
    return to_uint8_array(sketch_.serialize_compact(header_size_bytes));
  }

  val serialize_updatable() const {
    return to_uint8_array(sketch_.serialize_updatable());
  }

  static HllSketchBinding deserialize(const val& input) {
    const auto bytes = from_uint8_array(input);
    return HllSketchBinding(datasketches::hll_sketch::deserialize(bytes.data(), bytes.size()));
  }

  static std::uint32_t get_max_updatable_serialization_bytes(std::uint8_t lg_config_k, datasketches::target_hll_type target_type) {
    return datasketches::hll_sketch::get_max_updatable_serialization_bytes(lg_config_k, target_type);
  }

  static double get_rel_err(bool upper_bound, bool unioned, std::uint8_t lg_config_k, std::uint8_t num_std_dev) {
    return datasketches::hll_sketch::get_rel_err(upper_bound, unioned, lg_config_k, num_std_dev);
  }

  const datasketches::hll_sketch& unwrap() const { return sketch_; }

private:
  datasketches::hll_sketch sketch_;
};

class HllUnionBinding {
public:
  explicit HllUnionBinding(std::uint8_t lg_max_k)
    : union_(lg_max_k) {}

  double get_estimate() const { return union_.get_estimate(); }
  double get_composite_estimate() const { return union_.get_composite_estimate(); }
  double get_lower_bound(std::uint8_t num_std_dev) const { return union_.get_lower_bound(num_std_dev); }
  double get_upper_bound(std::uint8_t num_std_dev) const { return union_.get_upper_bound(num_std_dev); }
  std::uint8_t get_lg_config_k() const { return union_.get_lg_config_k(); }
  datasketches::target_hll_type get_target_type() const { return union_.get_target_type(); }
  bool is_empty() const { return union_.is_empty(); }

  void reset() { union_.reset(); }

  HllSketchBinding get_result(datasketches::target_hll_type target_type) const {
    return HllSketchBinding(union_.get_result(target_type));
  }

  void update_sketch(const HllSketchBinding& sketch) { union_.update(sketch.unwrap()); }
  void update_string(const std::string& value) { union_.update(value); }
  void update_uint64(std::uint64_t value) { union_.update(value); }
  void update_uint32(std::uint32_t value) { union_.update(value); }
  void update_int64(std::int64_t value) { union_.update(value); }
  void update_int32(std::int32_t value) { union_.update(value); }
  void update_double(double value) { union_.update(value); }
  void update_float(float value) { union_.update(value); }

  void update_bytes(const val& input) {
    const auto bytes = from_uint8_array(input);
    union_.update(bytes.empty() ? nullptr : bytes.data(), bytes.size());
  }

  static double get_rel_err(bool upper_bound, bool unioned, std::uint8_t lg_config_k, std::uint8_t num_std_dev) {
    return datasketches::hll_union::get_rel_err(upper_bound, unioned, lg_config_k, num_std_dev);
  }

private:
  datasketches::hll_union union_;
};

EMSCRIPTEN_BINDINGS(datasketches_hll) {
  enum_<datasketches::target_hll_type>("TargetHllType")
    .value("HLL_4", datasketches::HLL_4)
    .value("HLL_6", datasketches::HLL_6)
    .value("HLL_8", datasketches::HLL_8);

  class_<HllSketchBinding>("HllSketch")
    .constructor<std::uint8_t, datasketches::target_hll_type, bool>()
    .function("reset", &HllSketchBinding::reset)
    .function("updateString", &HllSketchBinding::update_string)
    .function("updateUInt64", &HllSketchBinding::update_uint64)
    .function("updateUInt32", &HllSketchBinding::update_uint32)
    .function("updateInt64", &HllSketchBinding::update_int64)
    .function("updateInt32", &HllSketchBinding::update_int32)
    .function("updateDouble", &HllSketchBinding::update_double)
    .function("updateFloat", &HllSketchBinding::update_float)
    .function("updateBytes", &HllSketchBinding::update_bytes)
    .function("getEstimate", &HllSketchBinding::get_estimate)
    .function("getCompositeEstimate", &HllSketchBinding::get_composite_estimate)
    .function("getLowerBound", &HllSketchBinding::get_lower_bound)
    .function("getUpperBound", &HllSketchBinding::get_upper_bound)
    .function("getLgConfigK", &HllSketchBinding::get_lg_config_k)
    .function("getTargetType", &HllSketchBinding::get_target_type)
    .function("isCompact", &HllSketchBinding::is_compact)
    .function("isEmpty", &HllSketchBinding::is_empty)
    .function("getCompactSerializationBytes", &HllSketchBinding::get_compact_serialization_bytes)
    .function("getUpdatableSerializationBytes", &HllSketchBinding::get_updatable_serialization_bytes)
    .function("toString", &HllSketchBinding::to_string)
    .function("serializeCompact", &HllSketchBinding::serialize_compact)
    .function("serializeUpdatable", &HllSketchBinding::serialize_updatable)
    .class_function("deserialize", &HllSketchBinding::deserialize)
    .class_function("getMaxUpdatableSerializationBytes", &HllSketchBinding::get_max_updatable_serialization_bytes)
    .class_function("getRelErr", &HllSketchBinding::get_rel_err);

  class_<HllUnionBinding>("HllUnion")
    .constructor<std::uint8_t>()
    .function("getEstimate", &HllUnionBinding::get_estimate)
    .function("getCompositeEstimate", &HllUnionBinding::get_composite_estimate)
    .function("getLowerBound", &HllUnionBinding::get_lower_bound)
    .function("getUpperBound", &HllUnionBinding::get_upper_bound)
    .function("getLgConfigK", &HllUnionBinding::get_lg_config_k)
    .function("getTargetType", &HllUnionBinding::get_target_type)
    .function("isEmpty", &HllUnionBinding::is_empty)
    .function("reset", &HllUnionBinding::reset)
    .function("getResult", &HllUnionBinding::get_result)
    .function("updateSketch", &HllUnionBinding::update_sketch)
    .function("updateString", &HllUnionBinding::update_string)
    .function("updateUInt64", &HllUnionBinding::update_uint64)
    .function("updateUInt32", &HllUnionBinding::update_uint32)
    .function("updateInt64", &HllUnionBinding::update_int64)
    .function("updateInt32", &HllUnionBinding::update_int32)
    .function("updateDouble", &HllUnionBinding::update_double)
    .function("updateFloat", &HllUnionBinding::update_float)
    .function("updateBytes", &HllUnionBinding::update_bytes)
    .class_function("getRelErr", &HllUnionBinding::get_rel_err);
}

} // namespace
