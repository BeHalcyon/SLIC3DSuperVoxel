# SLIC3DSuperVoxel
SLIC algorithm for volume data in 3D space


Using:
```c
SLIC3DSuperVoxel.exe "input vifo file" "cluster number k" "0 or 1 for output label file" "0 or 1 for output boundary file"
```

Example:
```c
SLIC3DSuperVoxel.exe F:/H.vifo 8196 0 1
```

vifo file format:
-----------------------------------------------------
```cpp
1                   //volume number
uchar               //volume data type in "uchar" "float" "ushort"
500 500 100         //volume dimensions in x y z direction
1 1 1               //volume spaces in x y z direction
SPEEDf21.raw        //volume relative path
```

Several results:
-----------------------------------------------------

![xy plane](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_xyplane.png)
![yz plane](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_yzplane.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_volumerendering.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_asteroid_tev.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_MANIX.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_tooth.png)

