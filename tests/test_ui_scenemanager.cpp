#include <catch2/catch.hpp>
#include "Interface/ui/UISceneManager.h"
#include "Systems/SDLManager.h"
#include <memory>

// Mock scene for testing
class TestScene : public UIScene {
public:
    TestScene() = default;
    
    void onCreate() override {
        created = true;
    }
    
    void onEnter() override {
        entered = true;
    }
    
    void onExit() override {
        exited = true;
    }
    
    void onDestroy() override {
        destroyed = true;
    }
    
    void update(float deltaTime) override {
        lastDeltaTime = deltaTime;
        updateCount++;
    }
    
    void render() override {
        renderCount++;
    }
    
    void handleEvent(const SDL_Event& event) override {
        eventCount++;
    }
    
    // Test state
    bool created = false;
    bool entered = false;
    bool exited = false;
    bool destroyed = false;
    float lastDeltaTime = 0.0f;
    int updateCount = 0;
    int renderCount = 0;
    int eventCount = 0;
};

class AnotherTestScene : public UIScene {
public:
    AnotherTestScene() = default;
    
    void onCreate() override {
        created = true;
    }
    
    void onEnter() override {
        entered = true;
    }
    
    void onExit() override {
        exited = true;
    }
    
    bool created = false;
    bool entered = false;
    bool exited = false;
};

TEST_CASE("UISceneManager Basic Functionality", "[UISceneManager]") {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    SECTION("Scene Registration") {
        // Register a test scene
        sceneManager.registerScene<TestScene>("test");
        
        REQUIRE(sceneManager.hasScene("test"));
        REQUIRE_FALSE(sceneManager.hasScene("nonexistent"));
        
        auto scene = sceneManager.getScene("test");
        REQUIRE(scene != nullptr);
        
        auto testScene = std::dynamic_pointer_cast<TestScene>(scene);
        REQUIRE(testScene != nullptr);
        REQUIRE(testScene->created); // onCreate should be called during registration
    }
    
    SECTION("Scene Stack Management") {
        sceneManager.registerScene<TestScene>("test1");
        sceneManager.registerScene<AnotherTestScene>("test2");
        
        // Initially no scene
        REQUIRE(sceneManager.getCurrentScene() == nullptr);
        REQUIRE(sceneManager.getSceneStackSize() == 0);
        
        // Switch to first scene
        sceneManager.switchToScene("test1");
        REQUIRE(sceneManager.getCurrentScene() != nullptr);
        REQUIRE(sceneManager.getSceneStackSize() == 1);
        
        auto testScene1 = std::dynamic_pointer_cast<TestScene>(sceneManager.getCurrentScene());
        REQUIRE(testScene1 != nullptr);
        REQUIRE(testScene1->entered);
        
        // Push second scene
        sceneManager.pushScene("test2");
        REQUIRE(sceneManager.getSceneStackSize() == 2);
        
        auto testScene2 = std::dynamic_pointer_cast<AnotherTestScene>(sceneManager.getCurrentScene());
        REQUIRE(testScene2 != nullptr);
        REQUIRE(testScene2->entered);
        REQUIRE(testScene1->isPaused()); // First scene should be paused
        
        // Pop back to first scene
        sceneManager.popScene();
        REQUIRE(sceneManager.getSceneStackSize() == 1);
        REQUIRE(sceneManager.getCurrentScene() == testScene1);
        REQUIRE_FALSE(testScene1->isPaused()); // Should be unpaused
        REQUIRE(testScene2->exited); // Second scene should have exited
    }
}

TEST_CASE("UISceneManager Scene Transitions", "[UISceneManager]") {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    SECTION("Transition State") {
        sceneManager.registerScene<TestScene>("test1");
        sceneManager.registerScene<AnotherTestScene>("test2");
        
        // Start transition
        sceneManager.switchToScene("test1", SceneTransition::FADE, 0.5f);
        REQUIRE(sceneManager.isTransitioning());
        
        // Update transition
        sceneManager.update(0.1f); // 20% of transition
        REQUIRE(sceneManager.isTransitioning());
        
        sceneManager.update(0.5f); // Complete transition
        REQUIRE_FALSE(sceneManager.isTransitioning());
        
        auto testScene = std::dynamic_pointer_cast<TestScene>(sceneManager.getCurrentScene());
        REQUIRE(testScene != nullptr);
        REQUIRE(testScene->entered);
    }
    
    SECTION("No Transition for Same Scene") {
        sceneManager.registerScene<TestScene>("test");
        
        sceneManager.switchToScene("test");
        auto scene1 = sceneManager.getCurrentScene();
        
        sceneManager.switchToScene("test"); // Switch to same scene
        auto scene2 = sceneManager.getCurrentScene();
        
        REQUIRE(scene1 == scene2); // Should be the same scene instance
    }
}

TEST_CASE("UISceneManager Update and Render", "[UISceneManager]") {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    SECTION("Scene Update") {
        sceneManager.registerScene<TestScene>("test");
        sceneManager.switchToScene("test");
        
        auto testScene = std::dynamic_pointer_cast<TestScene>(sceneManager.getCurrentScene());
        REQUIRE(testScene != nullptr);
        
        // Update scene
        sceneManager.update(0.016f);
        REQUIRE(testScene->updateCount == 1);
        REQUIRE(testScene->lastDeltaTime == Approx(0.016f));
        
        // Update again
        sceneManager.update(0.032f);
        REQUIRE(testScene->updateCount == 2);
        REQUIRE(testScene->lastDeltaTime == Approx(0.032f));
    }
    
    SECTION("Paused Scene Not Updated") {
        sceneManager.registerScene<TestScene>("test1");
        sceneManager.registerScene<AnotherTestScene>("test2");
        
        sceneManager.switchToScene("test1");
        sceneManager.pushScene("test2");
        
        auto testScene1 = std::dynamic_pointer_cast<TestScene>(sceneManager.getScene("test1"));
        auto testScene2 = std::dynamic_pointer_cast<AnotherTestScene>(sceneManager.getScene("test2"));
        
        REQUIRE(testScene1->isPaused());
        
        sceneManager.update(0.016f);
        
        // Only active scene should be updated
        REQUIRE(testScene1->updateCount == 0); // Paused, not updated
    }
}

TEST_CASE("UISceneManager Global Data", "[UISceneManager]") {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    SECTION("Set and Get Global Data") {
        REQUIRE_FALSE(sceneManager.hasGlobalData("test_key"));
        REQUIRE(sceneManager.getGlobalData("test_key") == "");
        
        sceneManager.setGlobalData("test_key", "test_value");
        REQUIRE(sceneManager.hasGlobalData("test_key"));
        REQUIRE(sceneManager.getGlobalData("test_key") == "test_value");
        
        // Update existing data
        sceneManager.setGlobalData("test_key", "new_value");
        REQUIRE(sceneManager.getGlobalData("test_key") == "new_value");
    }
    
    SECTION("Multiple Global Data Items") {
        sceneManager.setGlobalData("player_name", "Alice");
        sceneManager.setGlobalData("player_score", "1000");
        sceneManager.setGlobalData("game_level", "5");
        
        REQUIRE(sceneManager.getGlobalData("player_name") == "Alice");
        REQUIRE(sceneManager.getGlobalData("player_score") == "1000");
        REQUIRE(sceneManager.getGlobalData("game_level") == "5");
        REQUIRE_FALSE(sceneManager.hasGlobalData("nonexistent"));
    }
}

TEST_CASE("UISceneManager Error Handling", "[UISceneManager]") {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    SECTION("Switch to Non-existent Scene") {
        // Should handle gracefully without crashing
        sceneManager.switchToScene("nonexistent");
        REQUIRE(sceneManager.getCurrentScene() == nullptr);
        REQUIRE(sceneManager.getSceneStackSize() == 0);
    }
    
    SECTION("Push Non-existent Scene") {
        sceneManager.registerScene<TestScene>("test");
        sceneManager.switchToScene("test");
        
        size_t initialSize = sceneManager.getSceneStackSize();
        sceneManager.pushScene("nonexistent");
        
        // Stack size should remain unchanged
        REQUIRE(sceneManager.getSceneStackSize() == initialSize);
    }
    
    SECTION("Pop from Empty Stack") {
        // Should handle gracefully
        sceneManager.popScene();
        REQUIRE(sceneManager.getCurrentScene() == nullptr);
        REQUIRE(sceneManager.getSceneStackSize() == 0);
    }
    
    SECTION("Pop Last Scene") {
        sceneManager.registerScene<TestScene>("test");
        sceneManager.switchToScene("test");
        
        // Should not allow popping the last scene
        sceneManager.popScene();
        REQUIRE(sceneManager.getCurrentScene() != nullptr);
        REQUIRE(sceneManager.getSceneStackSize() == 1);
    }
}

TEST_CASE("UIScene Base Class", "[UIScene]") {
    TestScene scene;
    
    SECTION("Initial State") {
        REQUIRE_FALSE(scene.isActive());
        REQUIRE_FALSE(scene.isPaused());
    }
    
    SECTION("Pause State") {
        scene.setPaused(true);
        REQUIRE(scene.isPaused());
        
        scene.setPaused(false);
        REQUIRE_FALSE(scene.isPaused());
    }
    
    SECTION("UI Manager Access") {
        // Should have a valid UI manager
        UIManager& manager = scene.getUIManager();
        const UIManager& constManager = scene.getUIManager();
        
        // Just verify we can access it without crash
        // (SDLManager would be needed for full functionality)
        REQUIRE(&manager == &constManager);
    }
}
