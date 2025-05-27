#include <gtest/gtest.h>
#include "../../src/EventManager.h"

class EventManagerTest : public ::testing::Test {
protected:
    EventManager eventManager;
    int callCount = 0;
    EventData lastEventData;

    void SetUp() override {
        callCount = 0;
        lastEventData = EventData{};
    }
};

TEST_F(EventManagerTest, EmitCallsRegisteredCallback) {
    // Listener registrieren
    eventManager.addListener(GameEvent::BallHitPaddle,
                            [this](const EventData& data) {
                                callCount++;
                                lastEventData = data;
                            });

    // Event auslösen
    EventData testData{10.5f, 20.5f};
    eventManager.emit(GameEvent::BallHitPaddle, testData);

    // Überprüfen ob Callback aufgerufen wurde
    EXPECT_EQ(1, callCount);
    EXPECT_FLOAT_EQ(10.5f, lastEventData.posX);
    EXPECT_FLOAT_EQ(20.5f, lastEventData.posY);
}

TEST_F(EventManagerTest, RemoveListenerStopsCallbacks) {
    // Listener mit Owner-Pointer registrieren
    int ownerObject = 42;
    eventManager.addListener(GameEvent::BallHitPaddle,
                            [this](const EventData&) { callCount++; },
                            &ownerObject);

    // Event auslösen und prüfen ob Callback aufgerufen wurde
    eventManager.emit(GameEvent::BallHitPaddle, {});
    EXPECT_EQ(1, callCount);

    // Listener entfernen
    eventManager.removeListener(GameEvent::BallHitPaddle, &ownerObject);

    // Event erneut auslösen - diesmal sollte nichts passieren
    eventManager.emit(GameEvent::BallHitPaddle, {});
    EXPECT_EQ(1, callCount); // Immer noch 1
}

TEST_F(EventManagerTest, MultipleListenersReceiveEvent) {
    int counter1 = 0, counter2 = 0;

    // Zwei verschiedene Listener registrieren
    eventManager.addListener(GameEvent::BallLost, [&counter1](const EventData&) {
        counter1++;
    });

    eventManager.addListener(GameEvent::BallLost, [&counter2](const EventData&) {
        counter2++;
    });

    // Event auslösen
    eventManager.emit(GameEvent::BallLost, {});

    // Beide Listener sollten aufgerufen worden sein
    EXPECT_EQ(1, counter1);
    EXPECT_EQ(1, counter2);
}

TEST_F(EventManagerTest, ClearAllListenersRemovesAll) {
    int counter = 0;

    // Listener für verschiedene Events registrieren
    eventManager.addListener(GameEvent::BallHitPaddle, [&counter](const EventData&) {
        counter++;
    });

    eventManager.addListener(GameEvent::BrickDestroyed, [&counter](const EventData&) {
        counter++;
    });

    // Alle Listener entfernen
    eventManager.clearAllListeners();

    // Events auslösen, sollte nichts passieren
    eventManager.emit(GameEvent::BallHitPaddle, {});
    eventManager.emit(GameEvent::BrickDestroyed, {});

    EXPECT_EQ(0, counter);
}

TEST_F(EventManagerTest, RemoveAllListenersForSpecificEvent) {
    int counter1 = 0, counter2 = 0;

    // Listener für zwei verschiedene Events registrieren
    eventManager.addListener(GameEvent::BallHitPaddle, [&counter1](const EventData&) {
        counter1++;
    });

    eventManager.addListener(GameEvent::BrickDestroyed, [&counter2](const EventData&) {
        counter2++;
    });

    // Alle Listener für ein bestimmtes Event entfernen
    eventManager.removeAllListeners(GameEvent::BallHitPaddle);

    // Events auslösen
    eventManager.emit(GameEvent::BallHitPaddle, {});
    eventManager.emit(GameEvent::BrickDestroyed, {});

    // Nur das nicht entfernte Event sollte einen Callback auslösen
    EXPECT_EQ(0, counter1);
    EXPECT_EQ(1, counter2);
}