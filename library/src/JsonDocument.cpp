
#if defined __link
#include "xml2json.hpp"
#endif

#include <fs.hpp>

#include "json/JsonDocument.hpp"

using namespace var;
using namespace json;

#if defined __link
JsonValue JsonDocument::from_xml_string(const char *xml, IsXmlFlat is_flat) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
#if !defined __android
  std::string json_string = xml2json(xml, is_flat == IsXmlFlat::no);
  return from_string(json_string.c_str());
#else
  return JsonValue();
#endif
}

JsonValue JsonDocument::load_xml(StringView path, IsXmlFlat is_flat) {
  fs::File input(path, fs::OpenMode::read_only());
  fs::DataFile data_file
    = fs::DataFile().reserve(input.size()).write(input).move();
  return from_xml_string(data_file.data().add_null_terminator(), is_flat);
}
#endif

JsonValue JsonDocument::from_string(const StringView json) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
  JsonValue value;
  value.m_value = API_SYSTEM_CALL_NULL(
    "",
    JsonValue::api()
      ->loadb(json.data(), json.length(), json_flags(), &m_error.m_value));
  return value;
}

var::String JsonDocument::to_string(const JsonValue &value) const {
  API_RETURN_VALUE_IF_ERROR(String());

  u32 size = API_SYSTEM_CALL(
    "",
    JsonValue::api()->dumpb(value.m_value, 0, 0, json_flags()));
  if (size == 0) {
    return var::String();
  }
  var::String result;
  result.resize(size);

  if (
    JsonValue::api()
      ->dumpb(value.m_value, result.to_char(), result.capacity(), json_flags())
    == 0) {
    return var::String();
  }
  return result;
}

size_t JsonDocument::read_file_data(void *buffer, size_t buflen, void *data) {
  return reinterpret_cast<const fs::File *>(data)
    ->read(buffer, buflen)
    .return_value();
}

JsonValue JsonDocument::load(const fs::File &file) {
  API_RETURN_VALUE_IF_ERROR(JsonValue());
  JsonValue value;
  value.m_value = API_SYSTEM_CALL_NULL(
    "",
    JsonValue::api()->load_callback(
      read_file_data,
      (void *)&file,
      json_flags(),
      &m_error.m_value));
  return value;
}

int JsonDocument::write_file_data(
  const char *buffer,
  size_t buflen,
  void *data) {
  reinterpret_cast<const fs::File *>(data)->write(buffer, buflen);
  if (return_value() != buflen) {
    return -1;
  }
  return 0;
}

JsonDocument &JsonDocument::save(const JsonValue &value, const fs::File &file) {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL(
    "",
    JsonValue::api()->dump_callback(
      value.m_value,
      write_file_data,
      (void *)&file,
      json_flags()));
  return *this;
}
