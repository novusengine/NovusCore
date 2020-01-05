project(client-demo_deps VERSION 1.0.0)

## Entt

add_library(Entt INTERFACE)
add_library(Entt::Entt ALIAS Entt)

target_include_directories(Entt INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/Entt"
)

## client-demo_deps

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(client-demo::dependencies ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    Entt::Entt
)
