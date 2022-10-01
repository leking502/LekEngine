//
// Created by leking on 22-9-30.
//

#ifndef LEKENGINE_LEK_GAME_OBJECT_HPP
#define LEKENGINE_LEK_GAME_OBJECT_HPP

#include "lek_model.hpp"

#include <glm/gtc/matrix_transform.hpp>


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
    class Lek2dGameObject {
    public:
        using id_t = unsigned int;
        int test{0};

        static Lek2dGameObject createGameObject() {
            static  id_t currentId = 0;
            return  Lek2dGameObject{currentId++};
        }

        Lek2dGameObject(const Lek2dGameObject &) = delete;
        Lek2dGameObject& operator=(const Lek2dGameObject&) = delete;
        Lek2dGameObject(Lek2dGameObject&&) = default;
        Lek2dGameObject& operator=(Lek2dGameObject&&) = default;

        id_t getId() {return id;}

        std::shared_ptr<LekModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        Lek2dGameObject(id_t objId) : id{objId} {}

        id_t id;
    };

    struct  TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3  rotation{};

        glm::mat4 mat4() {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4 {
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {translation.x, translation.y, translation.z, 1.0f}};
        }
    };
    class LekGameObject {
    public:
        using id_t = unsigned int;
        int test{0};

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
        TransformComponent transform{};
        string name{"gameObject"};

    private:
        LekGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
}


#endif //LEKENGINE_LEK_GAME_OBJECT_HPP
