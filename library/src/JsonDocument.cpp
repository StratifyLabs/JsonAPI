
#if defined __link
#include "xml2json.hpp"
#endif

#include <fs/File.hpp>

#include "json/JsonDocument.hpp"

using namespace var;
using namespace json;

#if defined __link
JsonValue JsonDocument::from_xml_string(const char *xml, IsXmlFlat is_flat) {
#if !defined __android
  std::string json_string = xml2json(xml, is_flat == IsXmlFlat::no);
  return from_string(json_string.c_str());
#else
  return JsonValue();
#endif
}

JsonValue JsonDocument::load_xml(StringView path, IsXmlFlat is_flat) {
  fs::File input(path, fs::OpenMode::read_only());
  fs::DataFile data_file(
      std::move(fs::DataFile().reserve(input.size()).write(input)));
  return from_xml_string(data_file.data().add_null_terminator(), is_flat);
}
#endif

JsonValue JsonDocument::from_string(const StringView json) {
  JsonValue value;
  value.m_value = JsonValue::api()->loadb(json.data(), json.length(),
                                          json_flags(), &m_error.m_value);
  return value;
}

size_t JsonDocument::load_file_data(void *buffer, size_t buflen, void *data) {
  return reinterpret_cast<fs::File *>(data)
      ->read(buffer, buflen)
      .return_value();
}

JsonValue JsonDocument::load(const fs::File &file) {
  JsonValue value;
  value.m_value =
      JsonValue::api()->loadfd(file.fileno(), json_flags(), &m_error.m_value);
  return value;
}

var::String JsonDocument::to_string(const JsonValue &value) const {
  u32 size = JsonValue::api()->dumpb(value.m_value, 0, 0, json_flags());
  if (size == 0) {
    return var::String();
  }
  var::String result;
  result.resize(size);

  if (JsonValue::api()->dumpb(value.m_value, result.to_char(),
                              result.capacity(), json_flags()) == 0) {
    return var::String();
  }
  return result;
}

JsonDocument &JsonDocument::save(const JsonValue &value, const fs::File &file) {
  JsonValue::api()->dumpfd(value.m_value, file.fileno(), json_flags());
  return *this;
}

JsonDocument &JsonDocument::save(const JsonValue &value,
                                 json_dump_callback_t callback, void *context) {
  JsonValue::api()->dump_callback(value.m_value, callback, context,
                                  json_flags());
  return *this;
}
