# NovusCore
[![Build Status](https://travis-ci.com/novuscore/NovusCore.svg?branch=master)](https://travis-ci.com/novuscore/NovusCore)
[![Build status](https://ci.appveyor.com/api/projects/status/aduidyknihendeph/branch/master?svg=true)](https://ci.appveyor.com/project/novuscore/novusCore/branch/master)
[![GitHub](https://img.shields.io/github/license/Novuscore/novuscore)](https://github.com/novuscore/NovusCore/blob/master/LICENSE)
[![Website Documentation](https://img.shields.io/badge/Website-Documentation-success)](https://novuscore.org)
[![Discord](https://img.shields.io/discord/554123112384299010?label=Discord&logo=Discord)](https://discord.gg/gz6FMZa)

Note: Build Status is temporarily broken

### What is NovusCore

NovusCore is an MMO Engine.

Currently NovusCore is developing a game/server solution where our main focus is to be feature complete with the most popular MMOs out there.
Long term, we hope to support many more features.

### The NovusCore Promise

The project was made with the promise that we would always focus on reliability, redundancy, and performance over convenience. We achieve this through making use of experience, but also applying modern techniques and design patterns.

The end-goal is to provide a game/server setup, capable of tackling all of the limitations set by the current standard. Solving those issues are complicated, but we start by applying a proper foundation for our architecture to allow for better flow of information(data) and performance.

### NovusCore Discord

The project has an official [Discord](https://discord.gg/gz6FMZa).
You will find the developers to be active on the discord and always up for answering any questions you might have regarding the project. Despite NovusCore not currently being ready for production level use, we are always welcoming any users that want to try using it.

### Dependencies
* [OpenSSL 1.1.0](https://www.openssl.org/source/)
* [CMAKE 3.12 or newer](https://cmake.org/download/)
* [Vulkan](https://vulkan.lunarg.com/)

### Libraries
Here we include a honorable mention to all the libraries included directly into the source of NovusCore. You do not need to download these on your own, but we felt it was important to address these as without them, NovusCore would be a lot more time consuming to develop.
* [Moodycamel ConcurrentQueue](https://github.com/cameron314/concurrentqueue)
* [Nlohmann Json](https://github.com/nlohmann/json)
* [Robin Hood Hashing](https://github.com/martinus/robin-hood-hashing)
* [Amy](https://github.com/liancheng/amy)
* [Asio](http://sourceforge.net/projects/asio/files/asio/1.12.1%20%28Stable%29/)
* [Tracy Profiler](https://bitbucket.org/wolfpld/tracy/src/master)
* [Entt ECS](https://github.com/skypjack/entt/)
* [Cpp-Taskflow](https://github.com/cpp-taskflow/cpp-taskflow)
