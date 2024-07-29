# bytecode
A demo bytecode interpreter with and without [threaded code](https://en.wikipedia.org/wiki/Threaded_code).

You can benchmark the results of using the looped vs. threaded using either `time` or `perf` via `time.sh` and `perf.sh`. Make sure you have these programs installed first! I've tested this on WSL Debian using `clang` and `gcc`. Some compilers such as Microsoft Visual C++ (MSVC) do not support [labels as values](https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Labels-as-Values.html) so if you're not using `gcc` or `clang`, this code will probably not compile.

On my machine the threaded version runs about 20%-30% faster. It seems like this is due to the threaded version requiring less than half as many branch instructions as the looped version (according to the perf output, when compiling `gcc -O2`). Looping over a switch statement of instructions requires a branch to the top of the loop, and a branch in the switch statement to get to the next set of instructions to execute. The threaded version avoids this by jumping directly to the next instruction.

Interestingly, the threaded version is a little slower than the looped version when compiling with `-O0` for gcc and for clang, but is still faster than the looped version when compiled with `-O1`, `-O2`, and `-O3`. 