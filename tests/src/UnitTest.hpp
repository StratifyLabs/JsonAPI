
#include <cstdio>

#include "chrono.hpp"
#include "fs.hpp"
#include "printer.hpp"
#include "sys.hpp"
#include "var.hpp"

#include "json.hpp"

#include "test/Test.hpp"

class UnitTest : public test::Test {
public:

  UnitTest(var::StringView name) : test::Test(name) {}

  bool execute_class_api_case() {

    if (!object_case()) {
      return false;
    }

    return true;
  }

  bool object_case() {

    PrinterObject po(printer(), "object");

    printer().object("objectTest",
                     JsonObject().insert("Hello", JsonString("World")));

    TEST_ASSERT(JsonObject()
                    .insert("test", JsonString("Hello"))
                    .at("Hello")
                    .to_string() == StringView("Hello"));
    return true;
  }

private:
};
