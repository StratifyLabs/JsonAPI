#ifndef JSONAPI_JSON_MACROS_HPP
#define JSONAPI_JSON_MACROS_HPP

// full copy, no reference to original
#define JSON_ACCESS_STRING_WITH_KEY(c, k, v)                                   \
  const char *get_##v##_cstring() const {                                      \
    return to_object().at(MCU_STRINGIFY(k)).to_cstring();                      \
  }                                                                            \
  var::StringView get_##v() const {                                            \
    return var::StringView(to_object().at(MCU_STRINGIFY(k)).to_cstring());     \
  }                                                                            \
  c &set_##v(const var::StringView value) {                                    \
    to_object().insert(MCU_STRINGIFY(k), json::JsonString(value));             \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_string_with_key_never_used_##v()

#define JSON_ACCESS_STRING(c, v) JSON_ACCESS_STRING_WITH_KEY(c, v, v)

// full copy, no reference to original
#define JSON_ACCESS_BOOL_WITH_KEY(c, k, v)                                     \
  bool is_##v() const { return to_object().at(MCU_STRINGIFY(k)).to_bool(); }   \
  c &set_##v(bool value = true) {                                              \
    to_object().insert(MCU_STRINGIFY(k), value);                               \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_bool_with_key_never_used_##v()

#define JSON_ACCESS_BOOL(c, v) JSON_ACCESS_BOOL_WITH_KEY(c, v, v)

// full copy, no reference to original
#define JSON_ACCESS_INTEGER_WITH_KEY(c, k, v)                                  \
  s32 get_##v() const {                                                        \
    return to_object().at(MCU_STRINGIFY(k)).to_integer();                      \
  }                                                                            \
  c &set_##v(s32 value) {                                                      \
    to_object().insert(MCU_STRINGIFY(k), json::JsonInteger(value));            \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_integer_with_key_never_used_##v()
#define JSON_ACCESS_INTEGER(c, v) JSON_ACCESS_INTEGER_WITH_KEY(c, v, v)

#define JSON_ACCESS_REAL_WITH_KEY(c, k, v)                                     \
  float get_##v() const { return to_object().at(MCU_STRINGIFY(k)).to_real(); } \
  c &set_##v(float value) {                                                    \
    to_object().insert(MCU_STRINGIFY(k), json::JsonReal(value));               \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_real_with_key_never_used_##v()
#define JSON_ACCESS_REAL(c, v) JSON_ACCESS_REAL_WITH_KEY(c, v, v)

// gets a copy that refers to the original JSON values
#define JSON_ACCESS_OBJECT_WITH_KEY(c, T, k, v)                                \
  T v() const { return T(to_object().at(MCU_STRINGIFY(k))); }                  \
  T get_##v() const {                                                          \
    return T(json::JsonObject().copy(to_object().at(MCU_STRINGIFY(k))));       \
  }                                                                            \
  c &set_##v(const T &a) {                                                     \
    to_object().insert(MCU_STRINGIFY(k), a);                                   \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_object_with_key_never_used_##v()

#define JSON_ACCESS_OBJECT(c, T, v) JSON_ACCESS_OBJECT_WITH_KEY(c, T, v, v)

// constructs a copy of a list that refers to original values (or full copy with
// get) T should have JsonKeyValue as a base
#define JSON_ACCESS_OBJECT_LIST_WITH_KEY(c, T, k, v)                           \
  json::JsonObject v##_to_object() const {                                     \
    return to_object().at(MCU_STRINGIFY(k));                                   \
  }                                                                            \
  json::JsonKeyValueList<T> get_##v() const {                                  \
    return to_object()                                                         \
        .at(MCU_STRINGIFY(k))                                                  \
        .to_object()                                                           \
        .construct_key_list_copy<T>();                                         \
  }                                                                            \
  json::JsonKeyValueList<T> v() const {                                        \
    return to_object()                                                         \
        .at(MCU_STRINGIFY(k))                                                  \
        .to_object()                                                           \
        .construct_key_list<T>();                                              \
  }                                                                            \
  c &set_##v(const json::JsonKeyValueList<T> &a) {                             \
    to_object().insert(MCU_STRINGIFY(k), json::JsonObject(a));                 \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_object_list_never_used_##v()

#define JSON_ACCESS_OBJECT_LIST(c, T, v)                                       \
  JSON_ACCESS_OBJECT_LIST_WITH_KEY(c, T, v, v)

// constructs a copy of a list that refers to original values (or full copy with
// get)
#define JSON_ACCESS_ARRAY_WITH_KEY(c, T, k, v)                                 \
  json::JsonArray v##_to_array() const {                                       \
    return to_object().at(MCU_STRINGIFY(k));                                   \
  }                                                                            \
  var::Vector<T> get_##v() const {                                             \
    return to_object()                                                         \
        .at(MCU_STRINGIFY(k))                                                  \
        .to_array()                                                            \
        .construct_list_copy<T>();                                             \
  }                                                                            \
  var::Vector<T> v() const {                                                   \
    return to_object().at(MCU_STRINGIFY(k)).to_array().construct_list<T>();    \
  }                                                                            \
  c &set_##v(const var::Vector<T> &a) {                                        \
    to_object().insert(MCU_STRINGIFY(k), json::JsonArray(a));                  \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_array_with_key_never_used_##v()

#define JSON_ACCESS_ARRAY(c, T, v) JSON_ACCESS_ARRAY_WITH_KEY(c, T, v, v)

// gets a copy that refers to the original JSON values
#define JSON_ACCESS_VALUE_WITH_KEY(c, k, v)                                    \
  const json::JsonValue &v() const {                                           \
    return to_object().at(MCU_STRINGIFY(k));                                   \
  }                                                                            \
  json::JsonValue get_##v() const {                                            \
    return json::JsonValue().copy(to_object().at(MCU_STRINGIFY(k)));           \
  }                                                                            \
  c &set_##v(const json::JsonValue &a) {                                       \
    to_object().insert(MCU_STRINGIFY(k), a);                                   \
    return *this;                                                              \
  }                                                                            \
  void json_access_value_with_key_never_used_##v()

#define JSON_ACCESS_VALUE(c, v) JSON_ACCESS_VALUE_WITH_KEY(c, v, v)

// full copy, no reference to original
#define JSON_ACCESS_STRING_ARRAY_WITH_KEY(c, k, v)                             \
  var::StringViewList get_##v() const {                                        \
    return to_object().at(MCU_STRINGIFY(k)).to_array().string_view_list();     \
  }                                                                            \
  c &set_##v(const var::StringViewList &a) {                                   \
    to_object().insert(MCU_STRINGIFY(k), json::JsonArray(a));                  \
    return *this;                                                              \
  }                                                                            \
  c &set_##v(const var::StringList &a) {                                       \
    to_object().insert(MCU_STRINGIFY(k), json::JsonArray(a));                  \
    return *this;                                                              \
  }                                                                            \
  c &remove_##v() {                                                            \
    to_object().remove(MCU_STRINGIFY(k));                                      \
    return *this;                                                              \
  }                                                                            \
  void json_access_array_with_key_never_used_##v()

#define JSON_ACCESS_STRING_ARRAY(c, v)                                         \
  JSON_ACCESS_STRING_ARRAY_WITH_KEY(c, v, v)

#define JSON_ACCESS_CONSTRUCT_OBJECT(c)                                        \
  c() : json::JsonValue(json::JsonObject()) {}                                 \
  c(const json::JsonObject object) : json::JsonValue(object) {}                \
  void json_access_construct_object_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_STRING(c, k, v)                             \
  var::StringView k() const { return key(); }                                  \
  var::String get_##v() const { return to_string(); }                          \
  c &set_##v(var::StringView a) {                                              \
    set_value(json::JsonString(a));                                            \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_string_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_REAL(c, k, v)                               \
  var::StringView k() const { return key(); }                                  \
  float v() const { return to_float(); }                                       \
  float get_##v() const { return to_float(); }                                 \
  c &set_##v(float a) {                                                        \
    set_value(json::JsonReal(a));                                              \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_real_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_INTEGER(c, k, v)                            \
  var::StringView k() const { return key(); }                                  \
  s32 v() const { return to_integer(); }                                       \
  s32 get_##v() const { return to_integer(); }                                 \
  c &set_##v(s32 a) {                                                          \
    set_value(json::JsonInteger(a));                                           \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_integer_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_BOOL(c, k, v)                               \
  var::StringView k() const { return key(); }                                  \
  bool is_##v() const { return to_bool(); }                                    \
  c &set_##v(bool a = true) {                                                  \
    a ? set_value(json::JsonTrue()) : set_value(json::JsonFalse());            \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_bool_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_ARRAY(c, T, k, v)                           \
  var::StringView k() const { return key(); }                                  \
  var::Vector<T> v() const { return to_array().construct_list<T>(); }          \
  var::Vector<T> get_##v() const {                                             \
    return to_array().construct_list_copy<T>();                                \
  }                                                                            \
  c &set_##v(const var::Vector<T> &a) {                                        \
    set_value(json::JsonArray(a));                                             \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_array_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_STRING_ARRAY(c, T, k, v)                    \
  var::StringView k() const { return key(); }                                  \
  var::StringList v() const { return to_array().construct_list<T>(); }         \
  var::StringList get_##v() const {                                            \
    return to_array().construct_list_copy<T>();                                \
  }                                                                            \
  c &set_##v(const var::StringList &a) {                                       \
    set_value(json::JsonArray(a));                                             \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_string_array_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_INTEGER_ARRAY(c, T, k, v)                   \
  var::StringView k() const { return key(); }                                  \
  var::Vector<s32> get_##v() const { return to_array().construct_list<T>(); }  \
  c &set_##v(const var::Vector<s32> &a) {                                      \
    set_value(json::JsonArray(a));                                             \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_integer_array_never_used_##v()

// access functions for working with JsonKeyValue objects
#define JSON_ACCESS_KEY_VALUE_PAIR_REAL_ARRAY(c, T, k, v)                      \
  var::StringView k() const { return key(); }                                  \
  var::Vector<float> get_##v() const {                                         \
    return to_array().construct_list<T>();                                     \
  }                                                                            \
  c &set_##v(const var::Vector<float> &a) {                                    \
    set_value(json::JsonArray(a));                                             \
    return *this;                                                              \
  }                                                                            \
  void json_access_key_value_pair_integer_array_never_used_##v()

#endif // JSONAPI_JSON_MACROS_HPP
