#include "obstacle.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static float speedMultiplier = 1.0f; // Multiplicador que aumenta ao longo do tempo

CUBOS_REFLECT_IMPL(Obstacle)
{
    return cubos::core::ecs::TypeBuilder<Obstacle>("Obstacle")
        .withField("velocity", &Obstacle::velocity)
        .withField("killZ", &Obstacle::killZ)
        .build();
}

void resetGame() {
    speedMultiplier = 1.0f; // Reseta o multiplicador ao valor inicial
}

void obstaclePlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Obstacle>();

    cubos.system("move obstacles")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const Obstacle&, Position&> obstacles) {

            static float speedIncrease = 0.1f; // Taxa de aumento da velocidade

            // Aumenta a velocidade ao longo do tempo
            speedMultiplier += speedIncrease * dt.value();
            
            for (auto [ent, obstacle, position] : obstacles)
            {
                // Atualiza a posição do obstáculo
                glm::vec3 adjustedVelocity = obstacle.velocity * speedMultiplier;

                // Atualiza a posição do obstáculo
                position.vec += adjustedVelocity * dt.value();

                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.15F)) * 1.5F;

                if (position.vec.z < obstacle.killZ)
                {
                    cmds.destroy(ent);
                }
            }
        });
}

void callResetGame() {
    resetGame();
}
