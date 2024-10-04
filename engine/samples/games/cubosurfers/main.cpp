#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "obstacle.hpp"
#include "player.hpp"
#include "spawner.hpp"
#include "armor.hpp"

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("ee5bb451-05b7-430f-a641-a746f7009eef");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("101da567-3d23-46ae-a391-c10ec00e8718");
static const Asset<InputBindings> InputBindingsAsset = AnyAsset("b20900a4-20ee-4caa-8830-14585050bead");

int main()
{

    //CUBOS_FAIL("teste");
    Cubos cubos{};

    cubos.plugin(defaultsPlugin);
    cubos.plugin(spawnerPlugin);
    cubos.plugin(obstaclePlugin);
    cubos.plugin(playerPlugin);
    cubos.plugin(armorPlugin);

    cubos.startupSystem("configure settings").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("set the palette, environment, input bindings and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, Input& input,
                 RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.2F, 0.4F, 0.8F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            input.bind(*assets.read(InputBindingsAsset));
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("restart the game on input")
        .call([](Commands cmds, const Assets& assets, const Input& input, Query<Entity> all) {
            if (input.justPressed("restart"))
            {
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }

                cmds.spawn(assets.read(SceneAsset)->blueprint);
            }
        });

    //Comentario para teste de print
    CUBOS_INFO("Estou aqui");

    cubos.system("detect player vs obstacle collisions")
        .call([](Commands cmds, Query<Player&, const CollidingWith&, const Obstacle&> collisions, const Assets& assets, Query<Entity> all) {
            for (auto [player, collidingWith, obstacle] : collisions)
            {
                CUBOS_INFO("Player collided with an obstacle!");

                if (isActive()) {
                    // Se activeArmor for true, defina a variável em armor.cpp como false e saia da função
                    setArmorActive(false); // Supondo que setArmorActive seja a função para definir a variável
                    CUBOS_INFO("Armor was active, deactivating armor and exiting.");
                    return;
                }

                for (auto [ent] : all)
                   {
                       cmds.destroy(ent);
                   }
                   CUBOS_INFO("destroyed player and obstacle");
                   callResetGame(); // Chama a função para redefinir o estado do jogo
                   resetGameSpeedMultiplier(); // Chama a função para redefinir o multiplicador de velocidade
                   void resetGame2(); // Chama a função para redefinir o estado do jogo
                   cmds.spawn(assets.read(SceneAsset)->blueprint);
                   
                CUBOS_INFO("destroyed player and obstacle");

                return; // Sai após a colisão para evitar múltiplos resets
            }
        });

    cubos.system("detect player vs armor collisions")
        .call([](Commands cmds, Query<Player&, const CollidingWith&, const Armor&> collisions, Query<Entity> all) {
            for (auto [player, collidingWith, armor] : collisions)
            {
                CUBOS_INFO("Player collided with an armor!");

                for (auto [ent] : all)
                    {
                        cmds.destroy(ent);
                    }

                    setArmorActive(true); // Chama a função para dar mais um escudo ao jogador/ mais uma vida

                CUBOS_INFO("destroyed armor");

                return; // Sai após a colisão para evitar múltiplos resets
            }
        });

    cubos.run();
}

