# Catch2
add_library(Catch2 INTERFACE)

# target_compile_features(Catch2
#     INTERFACE
#     cxx_alignas
#     cxx_alignof
#     cxx_attributes
#     cxx_auto_type
#     cxx_constexpr
#     cxx_defaulted_functions
#     cxx_deleted_functions
#     cxx_final
#     cxx_lambdas
#     cxx_noexcept
#     cxx_override
#     cxx_range_for
#     cxx_rvalue_references
#     cxx_static_assert
#     cxx_strong_enums
#     cxx_trailing_return_types
#     cxx_unicode_literals
#     cxx_user_literals
#     cxx_variadic_macros
#     )

target_include_directories(Catch2 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

