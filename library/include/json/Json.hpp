// Copyright 2016-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef JSONAPI_JSON_JSON_HPP_
#define JSONAPI_JSON_JSON_HPP_

#include <jansson/jansson_api.h>

#include <api/api.hpp>

#include <var/StackString.hpp>
#include <var/String.hpp>
#include <var/Vector.hpp>

namespace json {

#undef TRUE
#undef FALSE

class Container;
class JsonDocument;

class JsonError {
public:
  JsonError() { memset(&m_value, 0, sizeof(m_value)); }
  int line() const { return m_value.line; }
  int column() const { return m_value.column; }
  int position() const { return m_value.position; }
  var::StringView source() const { return m_value.source; }
  var::StringView text() const { return m_value.text; }

private:
  friend class JsonDocument;
  json_error_t m_value;
};

class JsonObject;
class JsonArray;
class JsonReal;
class JsonNull;
class JsonInteger;
class JsonString;

typedef api::Api<jansson_api_t, JANSSON_API_REQUEST> JsonApi;

class JsonValue : public api::ExecutionContext {
public:

  JsonValue();
  JsonValue(json_t *value);
  JsonValue(const JsonValue &value);
  JsonValue &operator=(const JsonValue &value);
  JsonValue(JsonValue &&a);
  JsonValue &operator=(JsonValue &&a);
  ~JsonValue();

  operator const JsonObject &() const { return to_object(); }
  operator JsonObject &() { return to_object(); }

  operator const JsonArray &() const { return to_array(); }
  operator JsonArray &() { return to_array(); }

  bool is_valid() const { return m_value != nullptr; }

  enum class Type {
    invalid = -1,
    object = JSON_OBJECT,
    array = JSON_ARRAY,
    string = JSON_STRING,
    real = JSON_REAL,
    integer = JSON_INTEGER,
    true_ = JSON_TRUE,
    false_ = JSON_FALSE,
    null = JSON_NULL
  };

  Type type() const {
    if (m_value) {
      return static_cast<Type>(json_typeof(m_value));
    }
    return Type::invalid;
  }

  bool is_object() const { return type() == Type::object; }
  bool is_array() const { return type() == Type::array; }
  bool is_string() const { return type() == Type::string; }
  bool is_real() const { return type() == Type::real; }
  bool is_integer() const { return type() == Type::integer; }
  bool is_true() const { return type() == Type::true_; }
  bool is_false() const { return type() == Type::false_; }
  bool is_null() const { return type() == Type::null; }

  JsonValue &to_value() { return *this; }
  const JsonValue &to_value() const { return *this; }
  const JsonObject &to_object() const;
  JsonObject &to_object();
  const JsonArray &to_array() const;
  JsonArray &to_array();

  JsonValue lookup(const var::StringView key_path) const;


  const char *to_cstring() const;

  var::StringView to_string_view() const {
    return var::StringView(to_cstring());
  }

  var::String to_string() const;

  float to_real() const;
  int to_integer() const;
  bool to_bool() const;

  JsonValue &assign(const var::StringView value);
  JsonValue &assign(const char *value) {
    return assign(var::StringView(value));
  }

  enum class IsDeepCopy { no, yes };

  using KeyList = var::StringViewList;

  JsonValue &
  copy(const JsonValue &value, IsDeepCopy deep_copy = IsDeepCopy::yes);

  static JsonApi &api() { return m_api; }

  const json_t * native_value() const {
    return m_value;
  }

protected:
  struct Key {
    Key(const var::StringView value)
      : key_string(value.is_null_terminated() ? var::StringView() : value),
        cstring(
          value.is_null_terminated() ? value.data() : key_string.cstring()) {}

    var::KeyString key_string;
    const char *cstring;
  };

  int create_if_not_valid();
  virtual json_t *create() { return 0; }

  static int translate_json_error(int json_error);

private:
  friend class JsonDocument;
  friend class JsonObject;
  friend class JsonArray;
  friend class JsonTrue;
  friend class JsonFalse;
  friend class JsonReal;
  friend class JsonInteger;
  friend class JsonString;
  friend class JsonNull;
  friend class JsonKeyValue;
  static JsonApi m_api;

  json_t *m_value;

  void add_reference(json_t *value);
};

class JsonKeyValue : public JsonValue {
public:
  JsonKeyValue(const var::StringView key, const JsonValue &value)
    : JsonValue(value), m_key(key) {}

  JsonKeyValue &set_value(const JsonValue &a) {
    add_reference(a.m_value);
    return *this;
  }

  const JsonValue &value() const { return *this; }
  JsonValue get_value() const { return JsonValue(*this); }

private:
  API_READ_ACCESS_COMPOUND(JsonKeyValue, var::KeyString, key);
};

template <class Derived> class JsonKeyValueList : public var::Vector<Derived> {
public:
  Derived at(const var::StringView key) {
    for (const auto &item : *this) {
      if (item.key() == key) {
        return item;
      }
    }
    return Derived("", JsonValue());
  }

  JsonValue::KeyList get_key_list() const {
    JsonValue::KeyList result;
    result.reserve(this->count());
    for (const auto &item : *this) {
      result.push_back(item.key());
    }
    return result;
  }
};

class JsonObject : public JsonValue {
public:
  JsonObject();

  JsonObject(const JsonObject &value);
  JsonObject &operator=(const JsonObject &value);

  template <class T> JsonKeyValueList<T> construct_key_list() {
    KeyList list = get_key_list();
    JsonKeyValueList<T> result;
    for (const auto &key : list) {
      result.push_back(T(key, at(key)));
    }
    return result;
  }

  template <class T> JsonKeyValueList<T> construct_key_list_copy() {
    KeyList list = get_key_list();
    JsonKeyValueList<T> result;
    for (const auto &key : list) {
      result.push_back(T(key, JsonValue().copy(at(key))));
    }
    return result;
  }

  template <class T> explicit JsonObject(const JsonKeyValueList<T> &list) {
    m_value = create();
    for (const auto &item : list) {
      insert(item.key(), item.value());
    }
  }

  bool is_empty() const { return count() == 0; }

  JsonObject &insert(const var::StringView key, const JsonValue &value);

  JsonObject &insert(const JsonKeyValue &key_value) {
    return insert(key_value.key(), key_value.value());
  }

  JsonObject &insert_bool(const var::StringView key, bool value);

  enum class UpdateFlags {
    null = 0x00,
    existing = 0x01,
    missing = 0x02,
    missing_and_existing = 0x03
  };

  JsonObject &update(const JsonValue &value,
                     UpdateFlags o_flags = UpdateFlags::null);

  JsonObject &remove(const var::StringView key);
  u32 count() const;
  JsonObject &clear();

  JsonValue at(const var::StringView key) const;

  KeyList get_key_list() const;

private:
  json_t *create() override;
};

class JsonArray : public JsonValue {
public:
  JsonArray();
  JsonArray(const JsonArray &value);
  JsonArray &operator=(const JsonArray &value);

  explicit JsonArray(const var::StringList &list);
  explicit JsonArray(const var::StringViewList &list);
  explicit JsonArray(const var::Vector<float> &list);
  explicit JsonArray(const var::Vector<u32> &list);
  explicit JsonArray(const var::Vector<s32> &list);

  template <class T> explicit JsonArray(const var::Vector<T> &list) {
    m_value = create();
    for (const auto &item : list) {
      append(item.to_object());
    }
  }

  template <class T> var::Vector<T> construct_list() const {
    var::Vector<T> result;
    result.reserve(count());
    for (u32 i = 0; i < count(); i++) {
      result.push_back(T(at(i).to_object()));
    }
    return result;
  }

  template <class T> var::Vector<T> construct_list_copy() const {
    var::Vector<T> result;
    result.reserve(count());
    for (u32 i = 0; i < count(); i++) {
      result.push_back(T(JsonValue().copy(at(i))));
    }
    return result;
  }

  bool is_empty() const { return count() == 0; }
  u32 count() const;

  JsonValue at(size_t position) const;
  JsonArray &append(const JsonValue &value);

  JsonArray &append(const JsonArray &array);

  JsonArray &insert(size_t position, const JsonValue &value);

  JsonArray &remove(size_t position);
  JsonArray &clear();

  var::StringViewList string_view_list() const;
  var::Vector<s32> integer_list() const;
  var::Vector<float> float_list() const;
  var::Vector<bool> bool_list() const;

private:
  json_t *create() override;
};

class JsonString : public JsonValue {
public:
  JsonString();
  explicit JsonString(const char *str);
  explicit JsonString(const var::StringView str);
  explicit JsonString(const var::String &str);

  const char *cstring() const;

private:
  json_t *create() override;
};

class JsonReal : public JsonValue {
public:
  JsonReal();
  explicit JsonReal(float value);

private:
  json_t *create() override;
};

class JsonInteger : public JsonValue {
public:
  JsonInteger();
  explicit JsonInteger(int value);

private:
  json_t *create() override;
};

class JsonNull : public JsonValue {
public:
  JsonNull();

private:
  json_t *create() override;
};

class JsonTrue : public JsonValue {
public:
  JsonTrue();

private:
  json_t *create() override;
};

class JsonFalse : public JsonValue {
public:
  JsonFalse();

private:
  json_t *create() override;
};

API_OR_NAMED_FLAGS_OPERATOR(JsonObject, UpdateFlags)

} // namespace json

#include "macros.hpp"

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const json::JsonValue &a);
Printer &operator<<(Printer &printer, const json::JsonError &a);
Printer &print_value(Printer &printer, const json::JsonValue &a,
                     var::StringView key);
} // namespace printer

#endif // JSONAPI_JSON_JSON_HPP_
