make clean
make
sudo chrt -f 99 perf stat -ddd ./looped
sudo chrt -f 99 perf stat -ddd ./threaded