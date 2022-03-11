# JsonAPI

JsonAPI is a C++ wrapper library for [jansson](https://github.com/akheron/jansson) JSON C library. It is well-suited to run on microcontrollers but can also run on the desktop.

## Design

The JsonAPI uses the Stratify Labs [API framework](https://github.com/StratifyLabs/API) which is based on the following design principles

- Thread Local Error Contexts
- Method Chaining
- Filesystem Inspired Abstraction
- Strong Arguments
- RAII Everywhere

More details of these principles are available at https://github.com/StratifyLabs/API.

## Building

The `JsonAPI` library is designed to be a CMake sub-project. To build, please use one of these projects:

- Desktop [Command Line Interface](https://github.com/StratifyLabs/cli)
- [Stratify OS on Nucleo-144](https://github.com/StratifyLabs/StratifyOS-Nucleo144)

## Usage

Under the hood, all memory management is handled by [jansson](https://github.com/akheron/jansson). The C++ objects just hold pointers to the jansson handled memory. So all objects should be passed by value rather than by reference.

You can easily create a JSON object using method chaining:

```cpp
#include <json.hpp>

//create an object
JsonObject object = JsonObject()
                      .insert("string", JsonString("string"))
                      .insert("integerString", JsonString("100"))
                      .insert("integer", JsonInteger(10))
                      .insert("integerZero", JsonInteger(0))
                      .insert("real", JsonReal(2.2f))
                      .insert("realString", JsonString("2.2f"))
                      .insert("realZero", JsonReal(0.0f))
                      .insert("true", JsonTrue())
                      .insert("false", JsonFalse())
                      .insert("null", JsonNull())
                      .insert("trueString", JsonString("true"))
                      .insert(
                        "array",
                        JsonArray()
                          .append(JsonString("string"))
                          .append(JsonInteger(5))
                          .append(JsonReal(2.5f))
                          .append(JsonTrue())
                          .append(JsonFalse())
                          .append(JsonNull()));

//access an object
const auto str = object.at("string").to_cstring();
const auto b = object.at("true").to_bool();
```

You can load and save JSON documents using `JsonDocument`.

```cpp

#include <fs.hpp>
#include <json.hpp>

//using the object above
JsonDocument().save(object, File(File::IsOverwrite::yes, "object.json"));
JsonObject loaded_object = JsonDocument().load(File("object.json"));
```