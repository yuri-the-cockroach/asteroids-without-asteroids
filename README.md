# Yet another clone of asteroids
## How to build
The thing expects to have libraylib.so in it's `shared/` folder and, as of now, has no way of building or acquiring it.
So you'll need to provide it.
After that, do `make -j$(nproc)` and it should be built

## How to run
`make run` or just `./main`

### Optional arguments:
`-lc` -- console log level
`-lf` -- file log level
