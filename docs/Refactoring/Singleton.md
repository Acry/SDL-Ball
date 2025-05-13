# Meyer's Singleton Pattern

```c++
class TtfLegacyGl {
    // Private constructor for singleton pattern
    TtfLegacyGl(); // Private constructor
    ...
public:
    // Delete copy constructor and assignment operator
    TtfLegacyGl(const TtfLegacyGl&) = delete;
    TtfLegacyGl& operator=(const TtfLegacyGl&) = delete;

    // Static method to get the singleton instance
    static TtfLegacyGl& getInstance() {
        static TtfLegacyGl instance;  // Initialized on first call
        return instance;
    }

    ~TtfLegacyGl();
    ...
```

```c++
// Singleton.hpp
class Singleton {
public:
    static Singleton& Instance() {
        static Singleton S;
        return S;
    }

private:
    Singleton();
    ~Singleton();
};
```

"This approach is founded on C++'s guarantee that local static objects are initialized when the object's definition is first encountered during a call to that function." ... "As a bonus, if you never call a function emulating a non-local static object, you never incur the cost of constructing and destructing the object."

When you call S`ingleton& s=Singleton::Instance()` first time the object is created and every next call to `Singleton::Instance()` results with the same object being returned.

Main issue:
subject to Destruction Order Fiasco
 (the equivalent to the Initialization Order Fiasco)

Another implementation is called the trusty leaky Singleton.

```c++
class Singleton {
public:
    static Singleton& Instance() {
        if (I == nullptr) { I = new Singleton(); }
        return *I;
    }

private:
    Singleton();
    ~Singleton();

    static Singleton* I;
};

// Singleton.cpp
Singleton* Singleton::I = 0;
```

Two issues:

leaks, unless you implement a Release and make sure to call it (once)
not thread safe

https://stackoverflow.com/questions/17712001/how-is-meyers-implementation-of-a-singleton-actually-a-singleton
https://stackoverflow.com/questions/1008019/what-is-the-singleton-design-pattern

## Against Singletons

replacing global variables with Singletons!

A singleton gives you:

Global access to an object, and
A guarantee that no more than one object of this type can ever be created

The problem with singletons is not their implementation. It is that they conflate two different concepts, neither of which is obviously desirable.

1) Singletons provide a global access mechanism to an object. Although they might be marginally more threadsafe or marginally more reliable in languages without a well-defined initialization order, this usage is still the moral equivalent of a global variable. It's a global variable dressed up in some awkward syntax (foo::get_instance() instead of g_foo, say), but it serves the exact same purpose (a single object accessible across the entire program) and has the exact same drawbacks.

2) Singletons prevent multiple instantiations of a class. It's rare, IME, that this kind of feature should be baked into a class. It's normally a much more contextual thing; a lot of the things that are regarded as one-and-only-one are really just happens-to-be-only-one. IMO a more appropriate solution is to just create only one instance--until you realize that you need more than one instance.

