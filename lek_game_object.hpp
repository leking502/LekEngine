//
// Created by leking on 22-9-30.
//

#ifndef LEKENGINE_LEK_GAME_OBJECT_HPP
#define LEKENGINE_LEK_GAME_OBJECT_HPP

#include "lek_model.hpp"


#include <memory>


namespace leking {

    struct  Transform2dComponent {
        vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float  rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s},{-s, c}};

            glm::mat2 scaleMat{{scale.x, 0.0f},{0.0f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class LekGameObject {
    public:
        using id_t = unsigned int;

        static LekGameObject createGameObject() {
            static  id_t currentId = 0;
            return  LekGameObject{currentId++};
        }

        LekGameObject(const LekGameObject &) = delete;
        LekGameObject& operator=(const LekGameObject&) = delete;
        LekGameObject(LekGameObject&&) = default;
        LekGameObject& operator=(LekGameObject&&) = default;

        id_t getId() {return id;}

        std::shared_ptr<LekModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        LekGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
}


#endif //LEKENGINE_LEK_GAME_OBJECT_HPP
