file(GLOB_RECURSE RaceSources **.cpp **.h)

add_executable(Race ${RaceSources})

target_include_directories(Race PUBLIC
    ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(Race PUBLIC
    glfw
    ${GLFW_LIBRARIES}
    glm
    ${Vulkan_LIBRARY}
)
