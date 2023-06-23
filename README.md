# Graphite

Graphite is an open-source, cross-platform, proof-of-concept graphics engine built for my final year project. This project aims to improve existing and new graphite engines' rendering capabilities by using new and old technologies together. The engine uses forward+ rendering to improve graphics and uses multiple other systems to improve performance.

This project is based on Vulkan and doesn't use any platform-specific code so in theory, porting this project to other platforms other than Windows, Linux and MacOS is possible. Still, since it's out of the scope of this specific project, it will only be officially supported on those platforms I've mentioned.

## Getting started

This project doesn't require a lot of things out of the box. It is stand alone but it relies on Vulkan so make sure your GPU supports Vulkan.

To get started, clone the repository to a local directory. You can use the following command(s) to do so.

```sh
git clone --recurse-submodules https://github.com/DhirajWishal/Graphite
```

This command will clone the project and its submodules to a new `Graphite` directory. Once you're there, you can use CMake to generate the build files. But conveniently, we've provided a couple of scripts that you can use to initialize the project for you.

```sh
cd Graphite
./Scripts/Bootstrap.[bat/sh]
```

Once the build files are generated, you can compile the project as usual. The output should be a file called `Graphite` (with the appropriate extension depending on the platform). You can run this file to run the engine.

## License

This project is licensed under [MIT](https://github.com/DhirajWishal/Graphite/blob/release/LICENSE).
