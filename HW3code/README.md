前往工程根目录
```shell
cd ${PROJECT_SOURCE_DIR}
```
example:
```shell
cd /home/cw/code/LIN_ws/C3_Homework_code
```

在工程根目录创建build文件夹（用于放置编译的文件）...
```shell
mkdir build
cd build
cmake ..
make
./app/add_rir ./../data/audio.raw ./../data/res.raw
```
