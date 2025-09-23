# Observer Pattern

## Other players

The Observer pattern is excellent for modularity in event-driven architectures, as it decouples subjects (event sources)
from observers (event handlers). It plays well with several other design patterns, enhancing flexibility, scalability,
and maintainability in C++ systems. Below are key patterns that complement the Observer pattern, along with how they
interact and why they work well together:

### Mediator Pattern

How it works with Observer: The Mediator pattern centralizes communication between objects, reducing direct
dependencies. In an event-driven system, the Mediator can act as a central hub that manages event notifications, while
the Observer pattern handles the subscription and notification of specific events. For example, a Mediator can
coordinate which observers are notified based on complex logic, avoiding tight coupling between subjects and observers.
Why it’s complementary: Observer can lead to many-to-many relationships, which can become complex. Mediator simplifies
this by providing a single point of control, improving modularity.
C++ Example Use Case: In a GUI framework, a Mediator (e.g., a controller) manages interactions between UI components (
observers) that subscribe to events like button clicks or state changes.
Implementation Tip: Use a Mediator class to route events to registered observers, leveraging C++ std::function or std::
vector<std::function> for flexible callback handling.

### Strategy Pattern

How it works with Observer: The Strategy pattern allows interchangeable algorithms or behaviors. Observers can use
different strategies to handle events, enabling runtime flexibility. For instance, an observer might switch between
different event-handling strategies based on context (e.g., logging to console vs. file).

Why it’s complementary: Observer focuses on event notification, while Strategy focuses on how events are processed,
allowing observers to be more modular and adaptable.

C++ Example Use Case: In a game, entities (observers) subscribe to collision events, and each entity uses a different
Strategy (e.g., damage calculation, bounce behavior) to respond to the event.

Implementation Tip: Use polymorphism or std::unique_ptr to encapsulate strategies within observer classes, allowing
dynamic behavior switching.

### Command Pattern

How it works with Observer: The Command pattern encapsulates actions as objects, which can be triggered by events in an
Observer-based system. Observers can execute commands when notified, decoupling the event trigger from the action taken.

Why it’s complementary: Observer handles event propagation, while Command encapsulates the action, enabling undo/redo,
logging, or queuing of actions. This enhances modularity by separating event detection from execution logic.

C++ Example Use Case: In a text editor, observers listen for user input events (e.g., keypresses), and each event
triggers a Command object (e.g., InsertTextCommand, DeleteTextCommand) that can be executed or undone.
Implementation Tip: Implement commands as classes with an execute() method, stored in a std::vector for history or
queuing in the observer.

### State Pattern

How it works with Observer: The State pattern allows an object to change its behavior based on its internal state.
Observers can use the State pattern to adapt their event-handling logic based on the current state of the system or
themselves.

Why it’s complementary: Observer propagates events, while State determines how those events are interpreted, making the
system more flexible and context-aware.

C++ Example Use Case: In a network application, a connection object (observer) subscribes to packet arrival events and
uses different states (e.g., ConnectedState, DisconnectedState) to handle packets differently.

Implementation Tip: Use a State base class with derived classes for each state, and switch states in the observer using
a pointer or std::variant.

### Decorator Pattern

How it works with Observer: The Decorator pattern adds responsibilities to objects dynamically. Observers can be
decorated with additional functionality (e.g., logging, filtering, or throttling) without modifying their core
event-handling logic.

Why it’s complementary: Decorators enhance observers without altering their subscription logic, keeping the system
modular and extensible.

C++ Example Use Case: In a messaging system, an observer that processes messages can be decorated with a
LoggingDecorator to log events or a FilterDecorator to ignore certain events.
Implementation Tip: Use inheritance or composition with std::shared_ptr to wrap observers with decorators dynamically.

### Reactor Pattern

How it works with Observer: The Reactor pattern handles multiple event sources in a single-threaded event loop,
dispatching events to handlers. Observer can be used within the Reactor to notify registered handlers of specific
events, integrating event-driven I/O with notification logic.

Why it’s complementary: Reactor manages the event loop, while Observer manages the notification of interested parties,
making them a natural fit for scalable event-driven systems.

C++ Example Use Case: In a server using Boost.Asio, a Reactor (event loop) dispatches socket events, and observers
handle specific events like “data received” or “connection closed.”

Implementation Tip: Use Boost.Asio or libuv for the Reactor, with observers implemented as callback handlers registered
via std::function.

Why These Combinations Work

    Modularity: Observer’s decoupling of subjects and observers aligns with the encapsulation provided by Mediator, Command, and Decorator, reducing dependencies.
    Flexibility: Strategy and State allow observers to adapt their behavior dynamically, while Reactor scales event handling for high-performance systems.
    Scalability: Combining these patterns supports complex systems (e.g., GUIs, servers, or games) by organizing event flow and behavior cleanly.

Practical C++ Considerations

    Use Modern C++: Leverage std::function, std::bind, or lambdas for flexible event callbacks in Observer implementations.
    Memory Management: Use smart pointers (std::shared_ptr, std::weak_ptr) to manage observer lifetimes and avoid dangling pointers.
    Thread Safety: In concurrent systems, combine Observer with thread-safe mechanisms (e.g., std::mutex, std::atomic) or use Reactor for single-threaded event loops.
    Libraries: Consider Boost.Asio for Reactor or libsigc++ for signal-slot (Observer-like) implementations in C++.

Example Integration (Observer + Mediator + Command)

```c++
#include <functional>
#include <vector>
#include <memory>
#include <iostream>

// Command interface
class Command {
public:
virtual void execute() = 0;
virtual ~Command() = default;
};

// Concrete command
class LogCommand : public Command {
public:
void execute() override { std::cout << "Logging event\n"; }
};

// Subject (Observer pattern)
class Subject {
std::vector<std::function<void()>> observers;
public:
void attach(std::function<void()> observer) { observers.push_back(observer); }
void notify() { for (auto& observer : observers) observer(); }
};

// Mediator
class Mediator {
Subject subject;
std::vector<std::unique_ptr<Command>> commands;
public:
void registerObserver(std::function<void()> observer) { subject.attach(observer); }
void addCommand(std::unique_ptr<Command> cmd) { commands.push_back(std::move(cmd)); }
void triggerEvent() {
subject.notify();
for (auto& cmd : commands) cmd->execute();
}
};

// Observer
class Logger {
public:
void onEvent() { std::cout << "Observer: Event received\n"; }
};

int main() {
Mediator mediator;
Logger logger;

    mediator.registerObserver([&]() { logger.onEvent(); });
    mediator.addCommand(std::make_unique<LogCommand>());
    
    mediator.triggerEvent();
    return 0;
}
```