make clean
make
sudo chrt -f 99 time --verbose ./looped
sudo chrt -f 99 time --verbose ./threaded