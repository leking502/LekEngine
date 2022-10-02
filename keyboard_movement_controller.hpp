//
// Created by leking on 22-10-1.
//

#ifndef LEKENGINE_KEYBOARD_MOVEMENT_CONTROLLER_HPP
#define LEKENGINE_KEYBOARD_MOVEMENT_CONTROLLER_HPP

#include "lek_game_object.hpp"
#include "lek_window.hpp"

namespace leking {

    struct CursorPos {
        double x;
        double y;
    };
    class KeyboardMovementController {

    public:
        struct KeyMapping {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, LekGameObject& gameObject);

        KeyMapping keys{};
        float moveSpeed{3.0f};
        float lookSpeed{1.5f};
    private:

        CursorPos clickPos{0};
        CursorPos cursorPos{0};
        bool onDrop{false};
    };

} // leking

#endif //LEKENGINE_KEYBOARD_MOVEMENT_CONTROLLER_HPP
