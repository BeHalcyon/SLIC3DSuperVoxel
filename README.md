# SLIC3DSuperVoxel
SLIC algorithm for volume data in 3D space


Using:
```c
SLIC3DSuperVoxel.exe "input vifo file" "output raw label file" "cluster number k"
```

Example:
```c
SLIC3DSuperVoxel.exe F:/H.vifo F:/H_label.raw 4096
```
![image](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch2.png)
  
vifo file format:
-----------------------------------------------------
```cpp
1                   //volume number
uchar               //volume data type in "uchar" "float" "ushort"
500 500 100         //volume dimensions in x y z direction
1 1 1               //volume spaces in x y z direction
SPEEDf21.raw        //volume relative path
```
