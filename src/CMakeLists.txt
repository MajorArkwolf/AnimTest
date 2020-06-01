
# Add source files.
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp

    Controller/Engine/Engine.cpp
    Controller/InputManager.cpp
        Controller/Animator.cpp

    # Model
    Model/Models/Mesh.cpp
    Model/Models/Model.cpp
    Model/Models/ModelManager.cpp
        Model/MovingModel.cpp
        Model/Models/Joint.cpp
        Model/Models/JointTransform.cpp
        Model/Models/KeyFrame.cpp
        Model/Models/Animation.cpp

    # View
    View/Renderer/Shader.cpp
    View/EulerCamera.cpp
    View/Renderer/OpenGL.cpp

    # Game
    Game/Scene.cpp
)

# Include project header files.
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})