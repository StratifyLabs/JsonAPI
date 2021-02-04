// Copyright 2016-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef JSONAPI_JSON_JSONDOCUMENT_HPP
#define JSONAPI_JSON_JSONDOCUMENT_HPP

#include <fs/File.hpp>
#include <fs/Path.hpp>
#include <var/StringView.hpp>

#include "Json.hpp"

namespace json {
/*! \brief Json Class
 * \details The Json class is a JsonValue
 * that can be loaded and saved from
 * either memory or the filesystem.
 *
 */
class JsonDocument : public api::ExecutionContext {
public:
  enum class Flags {
    reject_duplicates = JSON_REJECT_DUPLICATES,
    disable_eof_check = JSON_DISABLE_EOF_CHECK,
    decode_any = JSON_DECODE_ANY,
    decode_int_as_real = JSON_DECODE_INT_AS_REAL,
    allow_null = JSON_ALLOW_NUL,
    indent1 = JSON_INDENT(1),
    indent2 = JSON_INDENT(2),
    indent3 = JSON_INDENT(3),
    indent4 = JSON_INDENT(4),
    indent5 = JSON_INDENT(5),
    indent6 = JSON_INDENT(6),
    indent7 = JSON_INDENT(7),
    indent8 = JSON_INDENT(8),
    compact = JSON_COMPACT,
    ensure_ascii = JSON_ENSURE_ASCII,
    encode_any = JSON_ENCODE_ANY,
    preserve_order = JSON_PRESERVE_ORDER,
    escape_slash = JSON_ESCAPE_SLASH,
    embed = JSON_EMBED
  };

  JsonDocument &set_flags(Flags flags) {
    m_flags = flags;
    return *this;
  }

  Flags option_flags() const { return m_flags; }

  // load a JSON object or array from a file?
  /*!
   * \details Loads a JSON value from a file
   * \param path The path to the file
   * \return Zero on success
   */
  JsonValue load(const fs::FileObject &file);

#if defined __link
  enum class IsXmlFlat { no, yes };

  /*!
   * \details Loads a JSON value from a data object
   * \param data A reference to the data object containing the JSON
   * \return
   */
  JsonValue from_xml_string(const char *xml,
                            IsXmlFlat is_flat = IsXmlFlat::yes);

  JsonValue
  load_xml(const fs::FileObject &input, IsXmlFlat is_flat = IsXmlFlat::yes);
#endif

  /*!
   * \details Loads a JSON value from a data object
   * \param data A reference to the data object containing the JSON
   * \return
   */
  JsonValue from_string(const var::StringView json);

  var::String to_string(const JsonValue &value) const;

  var::String stringify(const JsonValue &value) const {
    return to_string(value);
  }

  JsonDocument &save(const JsonValue &value, const fs::FileObject &file);

  JsonDocument &
  save(const JsonValue &value, json_dump_callback_t callback, void *context);

  const JsonDocument &
  seek(const var::StringView path, const fs::FileObject &file) const;
  JsonDocument &seek(const var::StringView path, const fs::FileObject &file) {
    return API_CONST_CAST_SELF(JsonDocument, seek, path, file);
  }

  const JsonError &error() const { return m_error; }

  static bool is_valid(const fs::FileObject & file, printer::Printer *printer = nullptr);

private:
  Flags m_flags = Flags::indent3;
  JsonError m_error;

  u32 json_flags() const { return static_cast<u32>(option_flags()); }

  static int write_file_data(const char *buffer, size_t buflen, void *data);
  static size_t read_file_data(void *buffer, size_t buflen, void *data);
};

API_OR_NAMED_FLAGS_OPERATOR(JsonDocument, Flags)

} // namespace json

#endif // JSONAPI_JSON_JSONDOCUMENT_HPP
