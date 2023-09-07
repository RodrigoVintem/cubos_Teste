#include <cubos/core/ecs/event_reader.hpp>
#include <cubos/core/ecs/event_writer.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::core;

/// [Event struct]
struct MyEvent
{
    int value;
};
/// [Event struct]

struct State
{
    int step;
};

/// [Event reader system]
static void firstSystem(ecs::EventReader<MyEvent> reader) 
{
    for (const auto& event : reader)
    {
        CUBOS_INFO("A read {}", event.value);
    }
}
/// [Event reader system]

/// [Event writer system]
static void secondSystem(ecs::EventWriter<MyEvent> writer, ecs::Write<State> state, ecs::Write<cubos::engine::ShouldQuit> quit)
{
    state->step += 1;
    if (state->step == 1) // Write 1 2 3 on first run.
    {
        writer.push({1});
        writer.push({2});
        writer.push({3});
        CUBOS_INFO("B wrote 1 2 3");
    }
    else if (state->step == 2)
    {
        quit->value = true; // Stop the loop.
        writer.push({4});
        writer.push({5});
        writer.push({6});
        CUBOS_INFO("B wrote 4 5 6");
    }
}
/// [Event writer system]

static void thirdSystem(ecs::EventReader<MyEvent> reader)
{
    for (const auto& event : reader)
    {
        CUBOS_INFO("C read {}", event.value);
    }
}

static void fourthSystem(ecs::EventReader<MyEvent> reader)
{
    for (const auto& event : reader)
    {
        CUBOS_INFO("D read {}", event.value);
    }
}

int main()
{
    cubos::engine::Cubos cubos;
    cubos.addResource<State>(State{.step = 0});

    /// [Add Event]
    cubos.addEvent<MyEvent>();
    /// [Add Event]

    /// [Set ShouldQuit resource]
    cubos.startupSystem([](ecs::Write<cubos::engine::ShouldQuit> quit) { quit->value = false; });
    /// [Set ShouldQuit resource]

    /// [Add systems]
    cubos.system(firstSystem).tagged("A").before("B");
    cubos.system(secondSystem).tagged("B");
    cubos.system(thirdSystem).tagged("C").after("B");
    cubos.system(fourthSystem).tagged("D").after("C");
    /// [Add systems]

    /// [Expected results]
    // Should print:
    // B wrote 1 2 3
    // C read 1
    // C read 2
    // C read 3
    // D read 1
    // D read 2
    // D read 3
    // A read 1
    // A read 2
    // A read 3
    // B wrote 4 5 6
    // C read 4
    // C read 5
    // C read 6
    // D read 4
    // D read 5
    // D read 6
    /// [Expected results]

    cubos.run();
}
