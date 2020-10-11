
#if defined __link
#include "xml2json.hpp"
#endif

#include <fs/File.hpp>

#include "json/JsonDocument.hpp"

using namespace var;
using namespace json;

JsonValue JsonDocument::load(StringView path) {
  JsonValue value;
  value.m_value = JsonValue::api()->load_file(path.cstring(), json_flags(),
                                              &m_error.m_value);
  return value;
}

#if defined __link
JsonValue JsonDocument::from_xml_string(var::StringView xml,
                                        IsXmlFlat is_flat) {
#if !defined __android
  std::string json_string = xml2json(xml.cstring(), is_flat == IsXmlFlat::no);
  return from_string(String(json_string.c_str()));
#else
  return JsonValue();
#endif
}

JsonValue JsonDocument::load_xml(StringView path, IsXmlFlat is_flat) {
  fs::File input(path, fs::OpenMode::read_only());
  fs::DataFile data_file = fs::DataFile().reserve(input.size()).write(input);
  String xml_string = String(data_file.data());
  return from_xml_string(xml_string, is_flat);
}
#endif

JsonValue JsonDocument::from_string(var::StringView json) {
  JsonValue value;
  value.m_value = JsonValue::api()->loadb(json.cstring(), json.length(),
                                          json_flags(), &m_error.m_value);
  return value;
}

size_t JsonDocument::load_file_data(void *buffer, size_t buflen, void *data) {
  return reinterpret_cast<fs::File *>(data)
      ->read(buffer, buflen)
      .status()
      .value();
}

JsonValue JsonDocument::load(const fs::File &file) {
  JsonValue value;
  value.m_value =
      JsonValue::api()->loadfd(file.fileno(), json_flags(), &m_error.m_value);
  return value;
}

JsonDocument &JsonDocument::save(const JsonValue &value, var::StringView path) {
  int result;
#if defined __win32
  result = JsonValue::api()->dump_file(value.m_value, path.argument().cstring(),
                                       flags());
#else
  fs::File f = fs::File::create(path, fs::File::IsOverwrite::yes);
  result = JsonValue::api()->dumpfd(value.m_value, f.fileno(), json_flags());
#endif
  API_SYSTEM_CALL("", result);
  return *this;
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
