Initial goal: plain UNIX executable that runs on my machine (mid 2020 MBP)
    - Build and link via `make`

Medium-term goals: other machines & app packages
    - Necessary platform-specific ports to Linux and Windows
        - Avoid non-SDL dependencies
    - Platform-specific application packaging scripts
        - i.e. on Apple, placing exec in [...].app/Contents/MacOS/[...]

Long term goal: wasm deployment
    - Port main program loop to emscripten
    - Understand file organization for a browser app

