
#include <fs/File.hpp>

#include "json/JsonDocument.hpp"

using namespace var;
using namespace json;

JsonValue JsonDocument::load(fs::Path path) {
  JsonValue value;
  value.m_value = JsonValue::api()->load_file(path.path().cstring(),
                                              json_flags(), &m_error.m_value);
  return value;
}

#if defined __link
JsonValue JsonDocument::load(XmlString xml, XmlIsFlat is_flat) {
#if !defined __android
  std::string json_string =
      xml2json(xml.argument().cstring(), is_flat.argument() == false);
  return load(String(json_string.c_str()));
#else
  return JsonValue();
#endif
}

JsonValue JsonDocument::load(XmlFilePath path, XmlIsFlat is_flat) {
  fs::DataFile data_file(fs::File::Path(path.argument()));
  String xml_string = String(data_file.data());
  return load(XmlString(xml_string), is_flat);
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

JsonValue JsonDocument::load(fs::File &file) {
  JsonValue value;
  value.m_value = JsonValue::api()->load_callback(
      load_file_data, static_cast<void *>(&file), json_flags(),
      &m_error.m_value);
  return value;
}

JsonValue JsonDocument::load(json_load_callback_t callback, void *context) {
  JsonValue value;
  value.m_value = JsonValue::api()->load_callback(
      callback, context, json_flags(), &m_error.m_value);
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
  API_ASSIGN_ERROR_CODE(api::ErrorCode::io_error, result);
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

JsonDocument &JsonDocument::save(const JsonValue &value, fs::File &file) {
  JsonValue::api()->dumpfd(value.m_value, file.fileno(), json_flags());
  return *this;
}

JsonDocument &JsonDocument::save(const JsonValue &value,
                                 json_dump_callback_t callback, void *context) {
  JsonValue::api()->dump_callback(value.m_value, callback, context,
                                  json_flags());
  return *this;
}
