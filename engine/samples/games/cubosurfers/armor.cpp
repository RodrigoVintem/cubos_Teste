#include "armor.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

static float speedMultiplier = 1.0f; // Multiplicador que aumenta ao longo do tempo (caso aplicável ao power-up)

// Refletir a estrutura Armor
CUBOS_REFLECT_IMPL(Armor)
{
    return cubos::core::ecs::TypeBuilder<Armor>("Armor")
        .withField("velocity", &Armor::velocity)
        .withField("killZ", &Armor::killZ)
        .build();
}

// Função para resetar o estado do jogo, se necessário
void resetGame() {
    speedMultiplier = 1.0f; // Reseta o multiplicador ao valor inicial
}

// Função que define o comportamento do plugin da armadura
void armorPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Armor>(); // Registrando o componente Armor

    cubos.system("move armor")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, const Armor&, Position&> armors) {

            static float speedIncrease = 0.1f; // Taxa de aumento da velocidade

            // Aumenta a velocidade ao longo do tempo
            speedMultiplier += speedIncrease * dt.value();
            
            for (auto [ent, armor, position] : armors)
            {
                // Atualiza a posição do obstáculo
                glm::vec3 adjustedVelocity = armor.velocity * speedMultiplier;

                // Atualiza a posição do obstáculo
                position.vec += adjustedVelocity * dt.value();

                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.15F)) * 1.5F;

                if (position.vec.z < armor.killZ)
                {
                    cmds.destroy(ent);
                }
            }
        });
}

// Função que reseta o jogo (caso aplicável)
void callResetGame() {
    resetGame();
}
