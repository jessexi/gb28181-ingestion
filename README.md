# gb28181-ingestion
build a gb28181 client

## Dependence
### pjsip
https://www.pjsip.org/download.htm
Install:
```
> download source
> ./configure --enable-shared  --disable-sound  --disable-video
> make dep
> make -j4
> make install
```

###  JRTPLIB
https://github.com/j0r1/JRTPLIB
Install:
```
> download source
> cd JRTPLIB
> mkdir build
> cd build
> cmake .. && make -j4 && sudo make install
```