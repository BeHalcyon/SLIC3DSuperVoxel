
# SLIC3DSuperVoxel (SLIC algorithm for volume data in 3D space)

Two works are achieved:

**STEP 1. Calculating super-voxels**
	
**STEP 2. Merging similar super-voxels using graph-based volume segmentation** (Not completed)

Compute the edge weight using the **chi-squared distance** between 1D intensity histograms of the two super-voxels. Each histogram uses a total of 64 bins across the entire scalar range of the input volume.

The segementation algorithm is under testing and is not convincing. There is an error in the measurement of the weight for edges and internal variance of regions. Current internal variance is measured by the maximum gradient difference of the edges in region, while the origin method is using the maximum edge weight of MST of the region.


Using:
```c
SLIC3DSuperVoxel.exe --TAG=parameter_value
```

Example:
```c
SLIC3DSuperVoxel.exe --vifo_path="C:/mixfrac.vifo" --merge=0
```
![cmdline](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/cmdline.png)


## vifo file format:
-----------------------------------------------------
```cpp
1                   //volume number
uchar               //volume data type in "uchar" "float" "ushort"
500 500 100         //volume dimensions in x y z direction
1 1 1               //volume spaces in x y z direction
SPEEDf21.raw        //volume relative path
```

Results for STEP 1:
-----------------------------------------------------

![xy plane](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_xyplane.png)
![yz plane](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_yzplane.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_volumerendering.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_asteroid_tev.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_MANIX.png)
![3D results](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/design%20sketch_tooth.png)


Results for STEP 2:
-----------------------------------------------------
(A modification for distance measurement to achieve irregular super-voxels)
![image](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/merged_combustion.png)
![image](https://github.com/XiangyangHe/SLIC3DSuperVoxel/blob/master/image/merged_H.png)

Reference:
-----------------------------------------------------
- [1] SLIC Superpixels Compared to State-of-the-Art Superpixel Methods
- [2] FeatureLego: Volume Exploration Using Exhausting Clustering of Super-Voxels
- [3] E±cient Graph-Based Image Segmentation
