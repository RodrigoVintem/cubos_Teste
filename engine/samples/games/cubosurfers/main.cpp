#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/load.hpp> //Load voxel grid
#include <cubos/engine/render/voxels/grid.hpp> // Include the header for RenderVoxelGrid
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
static const Asset<VoxelGrid> ArmorAsset = AnyAsset("4892c2f3-10b3-4ca7-9de3-822b77a0ba7e");
static const Asset<VoxelGrid> PlayerAsset = AnyAsset("57d1b886-8543-4b8b-8f78-d911e9c4f896");

int main()
{

    bool isActive = false; // Variável para verificar se o escudo está ativo ou não

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
            .call([&isActive](Commands cmds, Query<Entity, RenderVoxelGrid&, Player&, const CollidingWith&, Entity, const Obstacle&> collisions, const Assets& assets, Query<Entity> all) {
                for (auto [playerEntity, renderVoxelGrid, player, collidingWith, entity, obstacle] : collisions)
                {
                    CUBOS_INFO("Player collided with an obstacle!");

                    if (isActive) {
                        
                        isActive = false; 
                        CUBOS_INFO("Armor was active, deactivating armor and exiting.");

                        renderVoxelGrid.asset = PlayerAsset;
                        cmds.add(playerEntity, LoadRenderVoxels{});

                        // Destruir apenas o obstáculo colidido
                        cmds.destroy(entity);

                        return; // Sai da função após desativar a armadura
                    } else { 
                        for (auto [ent] : all)
                        {
                            cmds.destroy(ent);
                        }

                        CUBOS_INFO("destroyed player and obstacle");
                        callResetGame(); 
                        resetGameSpeedMultiplier(); 
                        resetGame2(); 
                        cmds.spawn(assets.read(SceneAsset)->blueprint);

                        CUBOS_INFO("destroyed player and obstacle");
                    }

                    return;
                }
            });
    

        cubos.system("detect player vs armor collisions")
            .call([&isActive](Commands cmds, Query<Entity, RenderVoxelGrid&, Player&, const CollidingWith&, Entity, const Armor&> collisions) {
                for (auto [playerEntity, renderVoxelGrid,player, collidingWith, entity, armor] : collisions)
                {
                    CUBOS_INFO("Player collided with an armor!");
                    isActive = true;

                    renderVoxelGrid.asset = ArmorAsset;
                    cmds.add(playerEntity, LoadRenderVoxels{});  

                    cmds.destroy(entity);

                    CUBOS_INFO("Destroyed armor");

                    return; 
                }
            });

    cubos.run();
}

