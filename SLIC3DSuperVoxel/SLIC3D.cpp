#include "SLIC3D.h"



SLIC3D::SLIC3D()
{
}


SLIC3D::~SLIC3D()
{
}


//===========================================================================
///	PerformSLICO_ForGivenK
///
/// Zero parameter SLIC algorithm for a given number K of superpixels.
//===========================================================================
void SLIC3D::PerformSLICO_ForGivenK(
	const unsigned int*			ubuff,
	const int					width,
	const int					height,
	const int					depth,
	int*						klabels,	//size is width*height*depth
	int&						numlabels,
	const int&					K,//required number of superpixels
	const double&				m)//weight given to spatial distance
{
	vector<double> kseedsl(0);
	vector<double> kseedsa(0);
	vector<double> kseedsb(0);
	vector<double> kseedsx(0);
	vector<double> kseedsy(0);

	//--------------------------------------------------
	m_width = width;
	m_height = height;
	m_depth = depth;

	int sz = m_width * m_height*m_depth;
	//--------------------------------------------------
	//if(0 == klabels) klabels = new int[sz];
	for (int s = 0; s < sz; s++) klabels[s] = -1;
	//--------------------------------------------------
	// if (1)//LAB
	// {
	// 	DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
	// }
	// else//RGB
	// {
	// 	m_lvec = new double[sz]; m_avec = new double[sz]; m_bvec = new double[sz];
	// 	for (int i = 0; i < sz; i++)
	// 	{
	// 		m_lvec[i] = ubuff[i] >> 16 & 0xff;
	// 		m_avec[i] = ubuff[i] >> 8 & 0xff;
	// 		m_bvec[i] = ubuff[i] & 0xff;
	// 	}
	// }
	//��ʼ������
	m_volumevec = new double[sz];
	for (auto i = 0; i < sz; i++) m_volumevec[i] = ubuff[i];
	//--------------------------------------------------

	bool perturbseeds(true);
	//edgemag��ʾͼ�е�ÿ���ڵ��Ӧ���ݶȲ�
	vector<double> edgemag(0);
	//�������ص���ݶ�ֵ�������ص�ı仯�̶�
	if (perturbseeds) DetectLabEdges(m_volumevec, m_width, m_height, m_depth, edgemag);
	//��ȡ��Ӧ�����ӵ㣬�ҽ����ӵ�����
	GetLABXYSeeds_ForGivenK(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, K, perturbseeds, edgemag);

	//����̫С������£����һ��С��ֵ
	int STEP = sqrt(double(sz) / double(K)) + 2.0;//adding a small value in the even the STEP size is too small.
	//PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, m);
	PerformSuperpixelSegmentation_VariableSandM(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, 10);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, K);
	{for (int i = 0; i < sz; i++) klabels[i] = nlabels[i]; }
	if (nlabels) delete[] nlabels;
}


void SLIC3D::DetectLabEdges(
	const double*				volumevec,
	const int&					width,
	const int&					height,
	const int&					depth,
	vector<double>&				edges)
{
	int sz = width * height * depth;

	edges.resize(sz, 0);

	for (int q = 1; q < depth - 1; q++)
	{
		for (int j = 1; j < height - 1; j++)
		{
			for (int k = 1; k < width - 1; k++)
			{
				auto i = q * height*width + j * width + k;
				//dx���x�����ϵ��ݶ�ƽ��
				double dx = (volumevec[i - 1] - volumevec[i + 1])*(volumevec[i - 1] - volumevec[i + 1]);
				//dy���y�����ϵ��ݶ�ƽ��
				double dy = (volumevec[i - width] - volumevec[i + width])*(volumevec[i - width] - volumevec[i + width]);
				//dz���z�����ϵ��ݶ�ƽ��
				double dz = (volumevec[i - width*height] - volumevec[i + width * height])*(volumevec[i - width * height] - volumevec[i + width * height]);

				//edges��ʾ���ص����ֵ�ݶȴ�С
				//edges[i] = (sqrt(dx) + sqrt(dy));
				edges[i] = (dx + dy + dz);
			}

		}
	}
}

//===========================================================================
///	GetLABXYSeeds_ForGivenK
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC3D::GetLABXYSeeds_ForGivenK(
	//l a b x y��ʾ���ӵ����Ϣ
	vector<double>&				kseedintensity,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	vector<double>&				kseedsz,
	const int&					K,
	const bool&					perturbseeds,
	const vector<double>&		edgemag)
{
	int sz = m_width * m_height * m_depth;
	//step��ʾ���ӵ�֮��ľ���
	//double step = sqrt(double(sz) / double(K));
	double step = pow(double(sz) / double(K), 1.0/3);
	int T = step;
	//xoff��yoff��ʾ��ʼ�����ӵ�λ��
	int xoff = step / 2;
	int yoff = step / 2;
	int zoff = step / 2;

	int n(0); int r(0);


	for(auto z = 0;z<m_depth;z++)
	{
		int Z = z * step + zoff;
		if (Z > m_depth - 1) break;
		//r��һ��ѭ�����������Ϊż������������Ϊ�������Ա�֤��������ĳ�ʼ���ӵ㲻һ��
		if (!(r & 0x1)) r = 1;
		//else r = 0;
		for(auto y =0;y<m_height;y++)
		{
			int Y = y * step + yoff;
			if (Y > m_height - 1) break;
			
			for(auto x=0;x<m_width;x++)
			{
				//int X = x*step + xoff;//square grid
				int X = x * step + (xoff << (r & 0x1));//hex grid
				if (X > m_width - 1) break;

				int i = Z*m_height*m_width + Y * m_width + X;

				kseedintensity.push_back(m_volumevec[i]);
				kseedsx.push_back(X);
				kseedsy.push_back(Y);
				kseedsz.push_back(Z);
				n++;
			}
			r++;
		}
	}

	//�������ӵ㣬���ӵ��΢С�ƶ�
	if (perturbseeds)
	{
		PerturbSeeds(kseedintensity, kseedsx, kseedsy, kseedsz, edgemag);
	}
}

//===========================================================================
///	PerturbSeeds ���ӵ��΢С�ƶ� ȡ��Χ26������
//===========================================================================
void SLIC3D::PerturbSeeds(
	vector<double>&				kseedintensity,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
	vector<double>&				kseedsz,
	const vector<double>&		edges)
{
	//const int dx8[8] = { -1, -1,  0,  1, 1, 1, 0, -1 };
	//const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1 };
	
	const int dx26[26] = { -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  1 };
	const int dy26[26] = { -1, -1, -1,  0,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  1,  1,  1, -1, -1, -1,  0,  0,  0,  1,  1,  1 };
	const int dz26[26] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1 };

	int numseeds = kseedintensity.size();

	for (int n = 0; n < numseeds; n++)
	{
		int ox = kseedsx[n];//original x
		int oy = kseedsy[n];//original y
		int oz = kseedsz[n];//original z

		int oind = oz*m_height*m_width + oy * m_width + ox;

		//�����ǰ�ڵ�Ĳ����Ը�С���򱣴�
		int storeind = oind;
		for (int i = 0; i < 26; i++)
		{
			int nx = ox + dx26[i];//new x
			int ny = oy + dy26[i];//new y
			int nz = oz + dz26[i];//new z

			if (nx >= 0 && nx < m_width && ny >= 0 && ny < m_height && nz>=0 && nz <=m_depth)
			{
				int nind = nz*m_height*m_width + ny * m_width + nx;
				if (edges[nind] < edges[storeind])
				{
					storeind = nind;
				}
			}
		}
		//��̬�����µ����ӵ�
		if (storeind != oind)
		{
			kseedsz[n] = storeind / (m_width*m_height);
			auto buf = storeind % (m_width*m_height);
			kseedsx[n] = buf % m_width;
			kseedsy[n] = buf / m_width;

			kseedintensity[n] = m_volumevec[storeind];
		}
	}
}