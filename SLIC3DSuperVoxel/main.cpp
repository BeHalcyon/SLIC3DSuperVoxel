#include <iostream>
#include <fstream>
#include <string>

#include "SourceVolume.h"
#include "SLIC3D.h"

void readInfoFile(const string& infoFileName, int& data_number, string& datatype, hxy::my_int3& dimension, hxy::my_double3& space,
	vector<string>& file_list)
{
	file_list.clear();

	ifstream inforFile(infoFileName);

	inforFile >> data_number;
	inforFile >> datatype;
	inforFile >> dimension.x >> dimension.y >> dimension.z;
	//Debug 20190520 增加sapce接口
	inforFile >> space.x >> space.y >> space.z;
	const string filePath = infoFileName.substr(0, infoFileName.find_last_of('/') == -1?
		infoFileName.find_last_of('\\') + 1: infoFileName.find_last_of('/')+1);
	std::cout << (filePath.c_str()) << std::endl;
	for (auto i = 0; i < data_number; i++)
	{
		string rawFileName;
		inforFile >> rawFileName;
		string volumePath = filePath + rawFileName;
		file_list.push_back(volumePath);
	}
	std::cout << "Info file name : \t\t" << infoFileName.c_str() << std::endl;
	std::cout << "Volume number : \t\t" << data_number << std::endl;
	std::cout << "data type : \t\t\t" << datatype.c_str() << std::endl;
	std::cout << "Volume dimension : \t\t" << "[" << dimension.x << "," << dimension.y << "," << dimension.z << "]" << std::endl;
	std::cout << "Space dimension : \t\t" << "[" << space.x << "," << space.y << "," << space.z << "]" << std::endl;
	for (auto i = 0; i < data_number; i++)
	{
		std::cout << "Volume "<<i<<" name : \t\t" << file_list[i].c_str() << std::endl;
	}


	std::cout << "Info file has been loaded successfully." << std::endl;
}

int main(int argc, char* argv[])
{
	
	if(argc<4)
	{
		std::cout << "Please using [SLIC3D.exe <input vifo file> <output raw label file> <cluster number> [output raw boundary file]]." << std::endl;
		return -1;
	}

	//string			infoFileName = "F:/CThead/manix/manix.vifo";
	//string			infoFileName = "F:/atmosphere/timestep21_float/multiple_variables.vifo";
	string				infoFileName = argv[1];
	int				data_number;
	string			datatype;
	hxy::my_int3	dimension;
	hxy::my_double3	space;
	vector<string>	file_list;

	readInfoFile(infoFileName, data_number, datatype, dimension, space, file_list);

	SourceVolume source_volume(file_list, dimension.x, dimension.y, dimension.z, datatype);

	source_volume.loadVolume();	//origin data
	source_volume.loadRegularVolume(); //[0, 255] data
	source_volume.loadDownSamplingVolume(); //[0, histogram_dimension] data

	auto volume_data = source_volume.getRegularVolume(0);


	//----------------------------------
	// Initialize parameters
	//----------------------------------
	int k = atoi(argv[3]);//Desired number of superpixels.
	double m = 20;//Compactness factor. use a value ranging from 10 to 40 depending on your needs. Default is 10
	int* klabels = new int[dimension.x*dimension.y*dimension.z];
	int num_labels(0);
	SLIC3D slic_3d;
	slic_3d.PerformSLICO_ForGivenK((*volume_data).data(), dimension.x, dimension.y, dimension.z, klabels, num_labels, k, m);

	//string label_file = "F:/CThead/manix/manix_label.raw";
	//string label_file = "F:/atmosphere/timestep21_float/_SPEEDf21_label.raw";
	string label_file = argv[2];
	slic_3d.SaveSuperpixelLabels(klabels, dimension.x, dimension.y, dimension.z, label_file);


	if(argc==5)
	{
		auto segment_boundary_array = new int[dimension.x*dimension.y*dimension.z];
		slic_3d.DrawContoursAroundSegments(segment_boundary_array, klabels, dimension.x, dimension.y, dimension.z);
		slic_3d.SaveSegmentBouyndaries(segment_boundary_array, dimension.x, dimension.y, dimension.z, argv[4]);
	}


	//getchar();
	delete[] klabels;
}