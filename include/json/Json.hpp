/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
#ifndef JSONAPI_JSON_JSON_HPP_
#define JSONAPI_JSON_JSON_HPP_

#include <jansson/jansson_api.h>

#include <api/api.hpp>

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
  ~JsonValue();

  JsonValue(JsonValue &&a);
  JsonValue &operator=(JsonValue &&a);

  operator const JsonObject &() const { return to_object(); }

  operator JsonObject &() { return to_object(); }

  operator const JsonArray &() const { return to_array(); }

  operator JsonArray &() { return to_array(); }

  /*! \details Returns true if the value is valid.
   *
   * \code
   * JsonObject object;
   * object.is_valid(); //true - a valid (though empty) object
   * object.insert("foo", "bar");
   * object.at("foo").is_valid(); //true
   * object.at("bar").is_valid(); //false
   *
   * JsonValue value;
   * value.is_valid(); //false
   *
   * JsonValue object_observer(object);
   *
   * object_observer.is_valid(); //true
   *
   * \endcode
   *
   */
  bool is_valid() const { return m_value != nullptr; }

  enum class Type {
    type_invalid = -1,
    type_object = JSON_OBJECT,
    type_array = JSON_ARRAY,
    type_string = JSON_STRING,
    type_real = JSON_REAL,
    type_integer = JSON_INTEGER,
    type_true = JSON_TRUE,
    type_false = JSON_FALSE,
    type_null = JSON_NULL,
    type_zero = JSON_NULL
  };

  Type type() const {
    if (m_value) {
      return static_cast<Type>(json_typeof(m_value));
    }
    return Type::type_invalid;
  }

  bool is_object() const { return type() == Type::type_object; }
  bool is_array() const { return type() == Type::type_array; }
  bool is_string() const { return type() == Type::type_string; }
  bool is_real() const { return type() == Type::type_real; }
  bool is_integer() const { return type() == Type::type_integer; }
  bool is_true() const { return type() == Type::type_true; }
  bool is_false() const { return type() == Type::type_false; }
  bool is_null() const { return type() == Type::type_zero; }
  bool is_zero() const { return is_null(); }

  JsonValue &to_value() { return *this; }
  const JsonValue &to_value() const { return *this; }
  const JsonObject &to_object() const;
  JsonObject &to_object();
  const JsonArray &to_array() const;
  JsonArray &to_array();

  const char *to_cstring() const;

  var::String to_string() const;

  /*! \details Returns a float value of the JSON value.
   *
   * If the JSON value is of type float, integer, or string,
   * it will be converted to a real number.
   *
   * Otherwise, 0.0f is returned.
   *
   */
  float to_real() const;

  /*! \details Returns an integer value of the JSON value.
   *
   * If the JSON value is of type float, integer, or string,
   * it will be converted to an integer.
   *
   * Otherwise, 0 is returned.
   *
   */
  int to_integer() const;

  /*! \details Returns a bool value of the JSON value.
   *
   * If the value is a string, it will return true
   * if the string is "true" (not case sensitive).
   *
   * - INTEGER: true if non-zero
   * - FLOAT: true if non-zero
   * - TRUE: true
   * - FALSE: false
   * - NULL: false
   * - OBJECT: true
   * - ARRAY: true
   * - INVALID: false
   *
   */
  bool to_bool() const;

  JsonValue &assign(const char *value);
  JsonValue &assign(float value);
  JsonValue &assign(int value);
  JsonValue &assign(bool value);

  enum class IsDeepCopy { no, yes };

  JsonValue &
  copy(const JsonValue &value, IsDeepCopy deep_copy = IsDeepCopy::yes);

  static JsonApi &api() { return m_api; }

protected:
  int create_if_not_valid();
  virtual json_t *create() { return 0; }

  static void exit_fatal(const char *message);
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
  JsonKeyValue(const char *key, const JsonValue &value)
      : JsonValue(value), m_key(key) {}

  JsonKeyValue &set_value(const JsonValue &a) {
    add_reference(a.m_value);
    return *this;
  }
  const JsonValue &value() const { return *this; }
  JsonValue get_value() const { return JsonValue(*this); }

private:
  API_READ_ACCESS_COMPOUND(JsonKeyValue, var::String, key);
};

template <class Derived> class JsonKeyValueList : public var::Vector<Derived> {
public:
  Derived at(const char *key) {
    for (const auto &item : *this) {
      if (item.key() == key) {
        return item;
      }
    }
    return Derived("", JsonValue());
  }

  var::StringList get_key_list() const {
    var::StringList result;
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
    var::StringList list = key_list();
    JsonKeyValueList<T> result;
    for (const auto &key : list) {
      result.push_back(T(key, at(key.cstring())));
    }
    return result;
  }

  template <class T> JsonKeyValueList<T> construct_key_list_copy() {
    var::StringList list = key_list();
    JsonKeyValueList<T> result;
    for (const auto &key : list) {
      result.push_back(T(key, JsonValue().copy(at(key.cstring()))));
    }
    return result;
  }

  template <class T> explicit JsonObject(const JsonKeyValueList<T> &list) {
    m_value = create();
    for (const auto &item : list) {
      insert(item.key(), item.value());
    }
  }

  /*! \details Returns true if the object is empty. */
  bool is_empty() const { return count() == 0; }

  /*!
   * \details Inserts the key value pair into the object.
   *
   * \param key The UTF-8 key
   * \param value The value of the key
   * \return Zero on success
   *
   * If \a key already exists in the object, it is
   * updated to the new value. If \a key does not
   * exist in the object, it is created.
   *
   */
  JsonObject &insert(const char *key, const JsonValue &value);

  JsonObject &insert(const JsonKeyValue &key_value) {
    return insert(key_value.key().cstring(), key_value.value());
  }

  JsonObject &insert(const char *key, bool value);

  enum updates {
    update_none = 0x00,
    update_existing = 0x01,
    update_missing = 0x02,
    update_missing_and_existing = 0x03
  };

  JsonObject &
  update(const JsonValue &value, enum updates o_flags = update_none);

  /*!
   * \details Removes the specified key from the object.
   * \param key The key to remove
   * \return Zero on success (-1 is key was not found)
   *
   */
  JsonObject &remove(const char *key);

  /*!
   * \details Returns the number of key/value pairs in the object
   * \return The number of pairs
   */
  u32 count() const;

  /*!
   * \details Removes all key/value pairs from the object.
   * \return Zero on success
   */
  JsonObject &clear();

  /*!
   * \details Returns a JsonValue (as a reference) to the specified key.
   * \param key The key to access
   * \return A JsonValue (referenced to the key)
   *
   * If the key does not exist, an invalid JsonValue is returned.
   *
   * \code
   * JsonObject json_object;
   * json_object.insert("name", "john");
   * json_object.at("name").is_valid(); //this is true
   * json_object.at("john").is_valid(); //this is false
   * \endcode
   *
   * Becaues the JsonValue is referenced (see JsonValue::is_observer())
   * the life of the object is not affected when the returned
   * JsonValue is destroyed.
   *
   */
  JsonValue at(const char *key) const;

  var::StringList key_list() const;
  var::StringList keys() const { return key_list(); }

private:
  json_t *create() override;
};

class JsonArray : public JsonValue {
public:
  JsonArray();
  JsonArray(const JsonArray &value);
  JsonArray &operator=(const JsonArray &value);

  explicit JsonArray(const var::StringList &list);
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

  /*! \details Returns true if the object is empty. */
  bool is_empty() const { return count() == 0; }

  u32 count() const;

  /*! \details Returns a JsonValue as a reference at
   * the specified index.
   *
   * @param idx The index of the value to access
   * @return A JsonValue as a reference (see JsonValue::is_observer())
   *
   */
  JsonValue at(size_t position) const;

  JsonArray &append(const JsonValue &value);

  JsonArray &append(const JsonArray &array);

  JsonArray &insert(size_t position, const JsonValue &value);

  JsonArray &remove(size_t position);
  JsonArray &clear();

  var::StringList string_list();
  var::Vector<s32> integer_list();
  var::Vector<float> float_list();
  var::Vector<bool> bool_list();

private:
  json_t *create() override;
};

class JsonString : public JsonValue {
public:
  JsonString();
  explicit JsonString(const char *str);

  const char *cstring() const;

private:
  json_t *create() override;
};

class JsonReal : public JsonValue {
public:
  JsonReal();
  explicit JsonReal(float value);
  JsonReal &operator=(float a);

private:
  json_t *create() override;
};

class JsonInteger : public JsonValue {
public:
  JsonInteger();
  explicit JsonInteger(int value);
  JsonInteger &operator=(int a);

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
