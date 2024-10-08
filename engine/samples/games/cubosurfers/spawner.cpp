#include "spawner.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

// Aumenta a velocidade ao longo do tempo, reduzindo o "period"
static float speedMultiplier = 1.0f;

CUBOS_REFLECT_IMPL(Spawner)
{
    return cubos::core::ecs::TypeBuilder<Spawner>("Spawner")
        .withField("scene", &Spawner::scene)
        .withField("sceneRoot", &Spawner::sceneRoot)
        .withField("period", &Spawner::period)
        .withField("laneWidth", &Spawner::laneWidth)
        .withField("accumulator", &Spawner::accumulator)
        .build();
}

void resetGameSpeedMultiplier() {
    speedMultiplier = 1.0f; // Reseta o multiplicador ao valor inicial
}

void spawnerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Spawner>();

    cubos.system("spawn stuff")
        .call([](Commands commands, const DeltaTime& dt, Assets& assets, Query<Spawner&, const Position&> spawners) {
            for (auto [spawner, position] : spawners)
            {
                //Atualiza o tempo do acumulador
                spawner.accumulator += dt.value();

                speedMultiplier += 0.05f * dt.value(); //Ajusta a taxa de aumento da velocidade

                float newPeriod = spawner.period / speedMultiplier;

                if (spawner.accumulator >= newPeriod)
                {
                    spawner.accumulator -= newPeriod;

                    Position spawnPosition = position;
                    int offset = (rand() % 3) - 1;
                    spawnPosition.vec.x += static_cast<float>(offset) * spawner.laneWidth;
                   
                    commands.spawn(assets.read(spawner.scene)->blueprint).add(spawner.sceneRoot, spawnPosition);
                    
                }
            }
        });
}