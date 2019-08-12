# Helper docker environment

### Setting up image
Use `docker-compose build` to setup docker image

Alternatively you can use `Dockerfile` to setup your own image.
If you're setting up your own image, please make sure to do the following:
1. mount git repo directory to `/home/docker/novuscore/src` container path.
2. use combination of `*.env` files to setup your container build environment (some environment variables are processed through `envsubst` to get over docker limitation on `.env` files)

It should be noted that as a part of image preparation `travisBuildScript.sh` is cached and processed to remove Windows line endings.

### Building using docker-compose
The following services are provided in `docker-compose.yml`:
- `novuscore-gcc`
- `novuscore-gcc-release`
- `novuscore-clang`
- `novuscore-clang-release`

To build the repo use `docker-compose run SERVICE_NAME [--verbose]]`, for example to build release version using clang use `docker-compose run novuscore-clang-release`.
If you want to have control over the build process you can start the container without triggering the build script using argument `--entrypoint` like so `docker-compose --entrypoint="bash" SERVICE_NAME`. This will initialize environment variables and start a container.

### Building manually
Upon starting the docker container make sure that you have all expected environment variables set using `env`. Make sure that all required variables are set:
- `REPO_ROOT` points to the git repo directory
- `BUILD_DIR` points to the desired build directory
- `C_COMPILER` contains C-compiler command
- `CXX_COMPILER` contains CXX-compiler command
- `EXTRA_CMAKE_FLAGS` sets CMAKE_INSTALL_PREFIX argument; `envsubst` is used to substitute environment variables

To trigger the build invoke `~/travisBuildScript.sh`
