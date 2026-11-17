# 2D-Game-Engine

## Examples

### Desktop

```sh
cmake -S examples --preset desktop
cmake --build build --target ecs hello_world
```

### WebAssembly (Emscripten)

Requires [emsdk](https://github.com/emscripten-core/emsdk) with the `EMSDK` environment variable exported (`source <emsdk-path>/emsdk_env.sh`):

```sh
cmake -S examples --preset wasm
cmake --build build-wasm --target ecs hello_world
```

Each example is generated as an `.html` page under `build-wasm/bin/`.
