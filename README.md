# Language Speed Test

Tests and algorithms to test the speeds of some languages/frameworks/tools, mainly c++ and python related.

## Tests

### Prime Numbers

| Language | Time (seconds) |
| -------- | -------------- |
| C++      | 0.225          |
| Python3  | 5.026          |
| nuitka3  | 1.056          |

## Replication

C++ was compiled with `g++ -std=c++14`

Python was used standard Python 3.8.5

nuitka3 is a converter from Python to machine code, compiled with `py2c`, a bash script that is basically `nuitka3 --standalone --python-flag=no_site $@`, it can be found in [my public-bin repo](https://github.com/onikenx/public-bin).

**Warning** : Don't forget to clear the buffer from your linux system so it doesn't use pre processed results, you can use my `clearbuffer` in [my public-bin repo](https://github.com/onikenx/public-bin) or just write `sudo bash -c 'sync; echo 3 > /proc/sys/vm/drop_caches'` in your terminal.

