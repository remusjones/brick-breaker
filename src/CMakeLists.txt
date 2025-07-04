add_executable(${PROJECT_NAME}
        systems/PhysicsSystem.cpp
        Game.cpp
        rendering/Display.cpp
        main.cpp
        systems/RenderSystem.cpp
        systems/RenderSystem.h
        systems/LevelSystem.cpp
        systems/LevelSystem.h
        systems/InputManager.cpp
        systems/InputManager.h
)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/systems
        ${CMAKE_CURRENT_SOURCE_DIR}/rendering
        ${CMAKE_CURRENT_SOURCE_DIR}/input
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE vendored)
