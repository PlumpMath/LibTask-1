LibTask (pre alpha) (MIT licence)
=========================

A small Coroutine lib in C based on Russ Cox libtask.
(see https://code.google.com/p/libtask/ for the original)

This one is rather limited, but compiles in Visual Studio.
Only currently works for x86. (more comming as needed)
It uses a much smaller context for storing the CPU state.

This uses asm, so is not the (set|long)jump hacky version.

Currently only the coroutines work, and will not delete properly yet.
