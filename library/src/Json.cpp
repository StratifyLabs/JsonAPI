// Copyright 2016-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <type_traits>

#if USE_PRINTER
#include "sys/Printer.hpp"
#endif

//#include "sys/Sys.hpp"
#include <printer/Printer.hpp>
#include <var/StackString.hpp>
#include <var/StringView.hpp>
#include <var/Tokenizer.hpp>

#include "json/Json.hpp"

printer::Printer &
printer::operator<<(Printer &printer, const json::JsonValue &a) {
  return print_value(printer, a, "");
}

printer::Printer &
printer::operator<<(Printer &printer, const json::JsonError &a) {
  printer.key("text", a.text());
  printer.key("line", var::NumberString(a.line()).string_view());
  printer.key("column", var::NumberString(a.column()).string_view());
  printer.key("position", var::NumberString(a.position()).string_view());
  printer.key("source", a.source());
  return printer;
}

printer::Printer &printer::print_value(
  Printer &printer,
  const json::JsonValue &a,
  var::StringView key) {
  if (a.is_object()) {
    json::JsonValue::KeyList key_list = a.to_object().get_key_list();
    if (!key.is_empty()) {
      printer.print_open_object(printer.verbose_level(), key);
    }
    for (const auto &subkey : key_list) {
      const json::JsonValue &entry = a.to_object().at(subkey);
      print_value(printer, entry, subkey);
    }
    if (!key.is_empty()) {
      printer.print_close_object();
    }
  } else if (a.is_array()) {
    if (!key.is_empty()) {
      printer.print_open_array(printer.verbose_level(), key);
    }
    for (u32 i = 0; i < a.to_array().count(); i++) {
      const json::JsonValue &entry = a.to_array().at(i);
      var::String subkey;
      subkey.format("[%04d]", i);
      print_value(printer, entry, subkey);
    }
    if (!key.is_empty()) {
      printer.print_close_array();
    }
  } else if (a.is_integer()) {
    printer.key(key, var::NumberString(a.to_integer()).string_view());
  } else if (a.is_real()) {
    printer.key(key, var::NumberString(a.to_real()).string_view());
  } else {
    printer.key(key, var::StringView(a.to_cstring()));
  }
  return printer;
}

using namespace var;
using namespace json;

JsonApi JsonValue::m_api;

JsonValue::JsonValue() {
  if (api().is_valid() == false) {
    exit_fatal("json api missing");
  }
  m_value = nullptr; // create() method from children are not available in the
                     // constructor
}

int JsonValue::translate_json_error(int json_result) {
  switch (json_result) {
  case json_error_unknown:
    return -1;
  case json_error_out_of_memory:
    return -1;
  case json_error_stack_overflow:
    return -1;
  case json_error_cannot_open_file:
    return -1;
  case json_error_invalid_argument:
    return -1;
  case json_error_invalid_utf8:
    return -1;
  case json_error_premature_end_of_input:
    return -1;
  case json_error_end_of_input_expected:
    return -1;
  case json_error_invalid_syntax:
    return -1;
  case json_error_invalid_format:
    return -1;
  case json_error_wrong_type:
    return -1;
  case json_error_null_character:
    return -1;
  case json_error_null_value:
    return -1;
  case json_error_null_byte_in_key:
    return -1;
  case json_error_duplicate_key:
    return -1;
  case json_error_numeric_overflow:
    return -1;
  case json_error_item_not_found:
    return -1;
  case json_error_index_out_of_range:
    return -1;
  }
  return 0;
}

JsonValue::JsonValue(json_t *value) {
  if (api().is_valid() == false) {
    exit_fatal("json api missing");
  }
  add_reference(value);
}

JsonValue::JsonValue(const JsonValue &value) {
  if (api().is_valid() == false) {
    exit_fatal("json api missing");
  }
  add_reference(value.m_value);
}

JsonValue &JsonValue::operator=(const JsonValue &value) {
  if (this != &value) {
    api()->decref(m_value);
    add_reference(value.m_value);
  }
  return *this;
}

void JsonValue::add_reference(json_t *value) {
  m_value = value;
  api()->incref(value);
}

JsonValue::JsonValue(JsonValue &&a) {
  m_value = a.m_value;
  if (this != &a) { // check for a move to self
    a.m_value = nullptr;
  }
}

JsonValue &JsonValue::operator=(JsonValue &&a) {
  if (this != &a) {
    m_value = a.m_value;
    a.m_value = nullptr;
  }
  return *this;
}

JsonValue::~JsonValue() {
  // only decref if object was created (not just a reference)
  api()->decref(m_value);
  m_value = nullptr;
}

const JsonObject &JsonValue::to_object() const {
  return static_cast<const JsonObject &>(*this);
}

JsonObject &JsonValue::to_object() { return static_cast<JsonObject &>(*this); }

const JsonArray &JsonValue::to_array() const {
  return static_cast<const JsonArray &>(*this);
}

JsonArray &JsonValue::to_array() { return static_cast<JsonArray &>(*this); }

int JsonValue::create_if_not_valid() {
  API_RETURN_VALUE_IF_ERROR(-1);
  if (is_valid()) {
    return 0;
  }
  m_value = create();
  if (m_value == nullptr) {
    API_SYSTEM_CALL("", -1);
    return -1;
  }
  return 0;
}

JsonValue &JsonValue::assign(const var::StringView value) {
  API_RETURN_VALUE_IF_ERROR(*this);
  if (is_string()) {
    API_SYSTEM_CALL(
      "",
      api()->string_setn(m_value, value.data(), value.length()));
  } else if (is_real()) {
    API_SYSTEM_CALL(
      "",
      api()->real_set(m_value, var::NumberString(value).to_float()));
  } else if (is_integer()) {
    API_SYSTEM_CALL(
      "",
      api()->integer_set(m_value, var::NumberString(value).to_integer()));
  } else if (is_true() || is_false()) {
    if (var::StringView(value) == "true") {
      *this = JsonTrue();
    } else {
      *this = JsonFalse();
    }
  }
  return *this;
}

JsonValue &JsonValue::copy(const JsonValue &value, IsDeepCopy is_deep) {
  api()->decref(m_value);
  if (is_deep == IsDeepCopy::yes) {
    m_value = api()->deep_copy(value.m_value);
  } else {
    m_value = api()->copy(value.m_value);
  }
  return *this;
}

const char *JsonValue::to_cstring() const {
  const char *result;
  if (is_string()) {
    result = api()->string_value(m_value);
  } else if (is_true()) {
    result = "true";
  } else if (is_false()) {
    result = "false";
  } else if (is_null()) {
    result = "null";
  } else if (is_object()) {
    result = "{object}";
  } else if (is_array()) {
    result = "[array]";
  } else {
    result = "";
  }
  return result;
}

var::String JsonValue::to_string() const {
  var::String result;
  if (is_string()) {
    result = api()->string_value(m_value);
  } else if (is_real()) {
    result.format("%f", api()->real_value(m_value));
  } else if (is_integer()) {
    result.format("%ld", api()->integer_value(m_value));
  } else if (is_true()) {
    result = "true";
  } else if (is_false()) {
    result = "false";
  } else if (is_null()) {
    result = "null";
  } else if (is_object()) {
    result = "{object}";
  } else if (is_array()) {
    result = "[array]";
  } else {
    result = "";
  }
  return result;
}

float JsonValue::to_real() const {
  if (is_string()) {
    return to_string().to_float();
  } else if (is_integer()) {
    return to_integer() * 1.0f;
  } else if (is_real()) {
    return api()->real_value(m_value);
  } else if (is_true()) {
    return 1.0f;
  }

  return 0.0f;
}

int JsonValue::to_integer() const {
  if (is_string()) {
    return to_string().to_integer();
  } else if (is_real()) {
    return to_real();
  } else if (is_integer()) {
    return api()->integer_value(m_value);
  }

  if (is_true()) {
    return 1;
  }

  if (is_false()) {
    return 0;
  }
  if (is_null()) {
    return 0;
  }

  return 0;
}

bool JsonValue::to_bool() const {
  if (is_true()) {
    return true;
  }
  if (is_false()) {
    return false;
  }

  if (is_string()) {
    if (to_cstring() == var::StringView("true")) {
      return true;
    }
    return false;
  }

  if (is_integer()) {
    return to_integer() != 0;
  }
  if (is_real()) {
    return to_real() != 0.0f;
  }

  if (is_object()) {
    return true;
  }

  if (is_array()) {
    return true;
  }
  return false;
}

JsonObject::JsonObject() { m_value = JsonObject::create(); }

JsonObject::JsonObject(const JsonObject &value) {
  add_reference(value.m_value);
}

JsonObject &JsonObject::operator=(const JsonObject &value) {
  api()->decref(m_value);
  add_reference(value.m_value);
  return *this;
}

json_t *JsonObject::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  json_t *result = API_SYSTEM_CALL_NULL("", api()->create_object());
  return result;
}

json_t *JsonArray::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_array());
}

json_t *JsonReal::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_real(0.0f));
}

json_t *JsonInteger::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_integer(0));
}

json_t *JsonTrue::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_true());
}

json_t *JsonFalse::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_false());
}

json_t *JsonString::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_string(""));
}

json_t *JsonNull::create() {
  API_RETURN_VALUE_IF_ERROR(nullptr);
  return API_SYSTEM_CALL_NULL("", api()->create_null());
}

JsonObject &JsonObject::insert_bool(const var::StringView key, bool value) {

  if (value) {
    return insert(Key(key).cstring, JsonTrue());
  }

  return insert(Key(key).cstring, JsonFalse());
}

JsonObject &
JsonObject::insert(const var::StringView key, const JsonValue &value) {
  if (create_if_not_valid() < 0) {
    return *this;
  }

  API_SYSTEM_CALL(
    "",
    api()->object_set(m_value, Key(key).cstring, value.m_value));
  return *this;
}

JsonObject &JsonObject::update(const JsonValue &value, UpdateFlags o_flags) {
  API_RETURN_VALUE_IF_ERROR(*this);
  if (o_flags & UpdateFlags::existing) {
    API_SYSTEM_CALL("", api()->object_update_existing(m_value, value.m_value));
    return *this;
  }

  if (o_flags & UpdateFlags::missing) {
    API_SYSTEM_CALL("", api()->object_update_missing(m_value, value.m_value));
    return *this;
  }

  API_SYSTEM_CALL("", api()->object_update(m_value, value.m_value));
  return *this;
}

JsonObject &JsonObject::remove(const var::StringView key) {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL("", api()->object_del(m_value, Key(key).cstring));
  return *this;
}

u32 JsonObject::count() const { return api()->object_size(m_value); }

JsonObject &JsonObject::clear() {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL("", api()->object_clear(m_value));
  return *this;
}

JsonObject::KeyList JsonObject::get_key_list() const {
  const char *key;
  u32 count = 0;
  for (key = api()->object_iter_key(api()->object_iter(m_value)); key;
       key = api()->object_iter_key(
         api()->object_iter_next(m_value, api()->object_key_to_iter(key)))) {
    count++;
  }

  KeyList result = KeyList().reserve(count);
  for (key = api()->object_iter_key(api()->object_iter(m_value)); key;
       key = api()->object_iter_key(
         api()->object_iter_next(m_value, api()->object_key_to_iter(key)))) {
    result.push_back(key);
  }

  return result;
}

JsonValue JsonObject::at(const var::StringView key) const {
  return JsonValue(api()->object_get(m_value, Key(key).cstring));
}

JsonArray::JsonArray() { m_value = JsonArray::create(); }

JsonArray::JsonArray(const JsonArray &value) { add_reference(value.m_value); }

JsonArray &JsonArray::operator=(const JsonArray &value) {
  api()->decref(m_value);
  add_reference(value.m_value);
  return *this;
}

u32 JsonArray::count() const { return api()->array_size(m_value); }

JsonValue JsonArray::at(size_t position) const {
  return JsonValue(api()->array_get(m_value, position));
}

JsonArray::JsonArray(const var::StringList &list) {
  m_value = JsonArray::create();
  for (const auto &entry : list) {
    append(JsonString(entry.cstring()));
  }
}

JsonArray::JsonArray(const var::StringViewList &list) {
  m_value = JsonArray::create();
  for (const auto &entry : list) {
    append(JsonString(entry));
  }
}

JsonArray::JsonArray(const var::Vector<float> &list) {
  m_value = JsonArray::create();
  for (const auto &entry : list) {
    append(JsonReal(entry));
  }
}

JsonArray::JsonArray(const var::Vector<u32> &list) {
  m_value = JsonArray::create();
  for (const auto &entry : list) {
    append(JsonInteger(entry));
  }
}

JsonArray::JsonArray(const var::Vector<s32> &list) {
  m_value = JsonArray::create();
  for (const auto &entry : list) {
    append(JsonInteger(entry));
  }
}

JsonArray &JsonArray::append(const JsonValue &value) {
  if (create_if_not_valid() < 0) {
    return *this;
  }
  API_SYSTEM_CALL("", api()->array_append(m_value, value.m_value));
  return *this;
}

JsonArray &JsonArray::append(const JsonArray &array) {
  if (create_if_not_valid() < 0) {
    return *this;
  }
  API_SYSTEM_CALL("", api()->array_extend(m_value, array.m_value));
  return *this;
}

JsonArray &JsonArray::insert(size_t position, const JsonValue &value) {
  if (create_if_not_valid() < 0) {
    return *this;
  }
  API_SYSTEM_CALL("", api()->array_insert(m_value, position, value.m_value));
  return *this;
}

JsonArray &JsonArray::remove(size_t position) {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL("", api()->array_remove(m_value, position));
  return *this;
}

JsonArray &JsonArray::clear() {
  API_RETURN_VALUE_IF_ERROR(*this);
  API_SYSTEM_CALL("", api()->array_clear(m_value));
  return *this;
}

var::StringViewList JsonArray::string_view_list() const {
  var::StringViewList result;
  result.reserve(count());
  for (u32 i = 0; i < count(); i++) {
    result.push_back(var::StringView(at(i).to_cstring()));
  }
  return result;
}

var::Vector<s32> JsonArray::integer_list() const {
  var::Vector<s32> result;
  result.reserve(count());
  for (u32 i = 0; i < count(); i++) {
    result.push_back(at(i).to_integer());
  }
  return result;
}

var::Vector<float> JsonArray::float_list() const {
  var::Vector<float> result;
  result.reserve(count());
  for (u32 i = 0; i < count(); i++) {
    result.push_back(at(i).to_real());
  }
  return result;
}

var::Vector<bool> JsonArray::bool_list() const {
  var::Vector<bool> result;
  result.reserve(count());
  for (u32 i = 0; i < count(); i++) {
    result.push_back(at(i).to_bool());
  }
  return result;
}

JsonString::JsonString() { m_value = JsonString::create(); }

JsonString::JsonString(const char *str) {
  API_RETURN_IF_ERROR();
  m_value = API_SYSTEM_CALL_NULL("", api()->create_string(str));
}

JsonString::JsonString(const var::StringView str) {
  API_RETURN_IF_ERROR();
  m_value
    = API_SYSTEM_CALL_NULL("", api()->create_stringn(str.data(), str.length()));
}

JsonString::JsonString(const var::String &str) {
  API_RETURN_IF_ERROR();
  m_value = API_SYSTEM_CALL_NULL(
    "",
    api()->create_stringn(str.cstring(), str.length()));
}

const char *JsonString::cstring() const { return api()->string_value(m_value); }

JsonReal::JsonReal() { m_value = JsonReal::create(); }

JsonReal::JsonReal(float value) {
  API_RETURN_IF_ERROR();
  m_value = API_SYSTEM_CALL_NULL("", api()->create_real(value));
}

JsonInteger::JsonInteger() { m_value = JsonInteger::create(); }

JsonInteger::JsonInteger(int value) {
  API_RETURN_IF_ERROR();
  m_value = API_SYSTEM_CALL_NULL("", api()->create_integer(value));
}

JsonNull::JsonNull() { m_value = JsonNull::create(); }

JsonTrue::JsonTrue() { m_value = JsonTrue::create(); }

JsonFalse::JsonFalse() { m_value = JsonFalse::create(); }
