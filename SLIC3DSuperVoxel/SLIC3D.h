#pragma once

#include <vector>
#include <string>
#include <algorithm>
using namespace std;




class SLIC3D
{
public:
	SLIC3D();
	virtual ~SLIC3D();


	//============================================================================
	// Superpixel segmentation for a given number of superpixels
	//============================================================================
	void PerformSLICO_ForGivenK(
		const unsigned int*			ubuff,//Each 32 bit unsigned int contains ARGB pixel values.
		const int					width,
		const int					height,
		int*						klabels,
		int&						numlabels,
		const int&					K,
		const double&				m);

private:
	//============================================================================
	// Detect color edges, to help PerturbSeeds()
	//============================================================================
	void DetectLabEdges(
		const double*				volumevec,
		const int&					width,
		const int&					height,
		const int&					depth,
		vector<double>&				edges);


	//============================================================================
	// Pick seeds for superpixels when number of superpixels is input.
	//============================================================================
	void GetLABXYSeeds_ForGivenK(
		vector<double>&				kseedintensity,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<double>&		edges);

	//===========================================================================
	///	PerturbSeeds 种子点的微小移动
	//===========================================================================
	void PerturbSeeds(
		vector<double>&				kseedintensity,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		vector<double>&				kseedsz,
		const vector<double>&		edges);

private:
	int										m_width;	//x
	int										m_height;	//y
	int										m_depth;	//z

	double*									m_lvec;
	double*									m_avec;
	double*									m_bvec;

	double*									m_volumevec;

	double**								m_lvecvec;
	double**								m_avecvec;
	double**								m_bvecvec;
};

