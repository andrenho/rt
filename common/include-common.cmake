if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(FetchContent)
    FetchContent_Declare(
            rt-common
            SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../common
    )
    FetchContent_MakeAvailable(rt-common)
endif()
