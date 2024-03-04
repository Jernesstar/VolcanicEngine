#pragma once

#include "Defines.h"
#include "Log.h"

#include <filesystem>

#define VOLCANICORE_INTERNAL_ASSERT_CHECK(condition, message, ...) { if(!(condition)) { VOLCANICORE_LOG_ERROR(message, ## __VA_ARGS__); /*VOLCANICORE_ERROR_CLOSE();*/ } }
#define VOLCANICORE_ASSERT_ARGS(condition, message, ...) VOLCANICORE_INTERNAL_ASSERT_CHECK(condition, message, ## __VA_ARGS__)
#define VOLCANICORE_ASSERT_NO_ARGS(condition) VOLCANICORE_INTERNAL_ASSERT_CHECK(condition, "Assertion failed: %s, file %s, line %s", #condition, std::filesystem::path(__FILE__).string(), std::to_string(__LINE__))

#define VOLCANICORE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

#define VOLCANICORE_ASSERT(...) VOLCANICORE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VOLCANICORE_ASSERT_ARGS, VOLCANICORE_ASSERT_NO_ARGS)(__VA_ARGS__)