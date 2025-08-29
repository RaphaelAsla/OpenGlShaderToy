add_library(glad "glad/src/gl.c")
target_include_directories(glad SYSTEM PUBLIC "glad/include")

# ImGui
add_library(imgui
    "imgui/imgui.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui SYSTEM PUBLIC
    "imgui"
    "imgui/backends"
)

target_link_libraries(imgui glfw glad)
