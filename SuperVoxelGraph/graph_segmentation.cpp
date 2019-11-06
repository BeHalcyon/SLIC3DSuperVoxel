
#include "graph_segmentation.h"
#include <limits>
#include <stack>
#include <iostream>

void GraphSegmentation::buildGraph(	const int*		label, 
									const int&		k_number, 
									const double*	gradient, 
									const int &		width, 
									const int &		height,
									const int &		depth)
{
	W = width;
	H = height;
	D = depth;
	auto N = width * height * depth;
	graph = ImageGraph(k_number);

	// Init each node and assign the id as cluster id.
	for(auto i=0; i < N ;i++)
	{
		const auto k_id = label[i];
		auto & node = graph.getNode(k_id);
		node.id = k_id;
		node.l = k_id;
		node.voxels.push_back(i);
	}

	// Calculate the max weight of neighbor voxels for each super-voxel (region)
	
	std::cout << "The nodes id and voxels has been initialized." << std::endl;

	// Update voxle number n and max weight for each node.
	// TODO

	const int dx6[6] = { -1,  1,  0,  0,  0,  0 };
	const int dy6[6] = { 0,  0, -1,  1,  0,  0 };
	const int dz6[6] = { 0,  0,  0,  0, -1,  1 };
	// For each super-voxel, we do loop To get the maximum weight
	// Warning : The internal varying are measured by the maximum weight rather than the max weight of the MST.


	for(auto i = 0;i<graph.getNumNodes();i++)
	{
		auto & node = graph.getNode(i);
		
		const auto & voxels = node.voxels;
		double max_weight = 0;

		// To get the maximum weight
		if(!voxels.empty())
		{
			std::stack<int> neighbor_voxels;
			std::map<int, int> not_accessed_voxels;

			for (auto voxel : voxels)
			{
				not_accessed_voxels[voxel] = 1;
			}

			neighbor_voxels.push(voxels[0]);
			while(!neighbor_voxels.empty())
			{
				const auto voxel = neighbor_voxels.top();
				neighbor_voxels.pop();

				not_accessed_voxels[voxel] = 0;

				const auto Z = voxel / (W*H);
				const auto buf = voxel % (W*H);
				const auto X = buf % W;
				const auto Y = buf / W;
				for (auto idx = 0; idx < 6; idx++)
				{
					const auto z = Z + dz6[idx];
					const auto y = Y + dy6[idx];
					const auto x = X + dx6[idx];
					if ((z > 0 && z < D) && (y > 0 && y < H) && (x > 0 && x < W))
					{
						auto neighbor_idx = z * H * W + y * W + x;
						//the two voxels are neighbored and belong to the same cluster.
						if (not_accessed_voxels[neighbor_idx] && label[voxel] == label[neighbor_idx])
						{
							max_weight = std::max(max_weight, abs(gradient[voxel] - gradient[neighbor_idx]));
							neighbor_voxels.push(neighbor_idx);
							//if(gradient[voxel]>100|| gradient[neighbor_idx]>100)
							//std::cout << voxel << "\t" << neighbor_idx <<"\t"<< gradient[voxel] <<"\t"<<gradient[neighbor_idx] <<std::endl;
						}
					}
				}
			}
			node.max_w = max_weight;
		}
		else
		{
			node.max_w = 0;
		}
		node.n = voxels.size();
	}

	std::cout << "The weight of nodes have been initialized." << std::endl;


	// Add edge.
	int dx3[3] = { 1,0,0 };
	int dy3[3] = { 0,1,0 };
	int dz3[3] = { 0,0,1 };
	std::vector<std::vector<double>> label_edge(k_number);
	for (auto i = 0; i < k_number; i++) label_edge[i].resize(k_number, 0xffffff);

	for (auto i = 0; i < D; i++)
	{
		for (auto j = 0; j < H; j++)
		{
			for (auto k = 0; k < W; k++)
			{
				const auto cur_idx = i * H*W + j * W + k;
				
				for (auto d : dx3)
				{
					
					const auto ii = i + d;
					const auto jj = j + d;
					const auto kk = k + d;
					if(ii<D&&jj<H&&kk<W)
					{
						const auto cur_label = label[cur_idx];
						const auto neighbor_idx = ii * H * W + jj * W + kk;
						const auto neighbor_label = label[neighbor_idx];
						// The two neighbor regions have an edge if they belong to different clusters
						if(cur_label!=neighbor_label)
						{
							label_edge[cur_label][neighbor_label] = std::min(std::min(
								label_edge[cur_label][neighbor_label], 
								abs(gradient[cur_idx] - gradient[neighbor_idx])),
								label_edge[neighbor_label][cur_label]);
							label_edge[neighbor_label][cur_label] = label_edge[cur_label][neighbor_label];
						}
					}
				}
			}
		}
	}

	std::cout << "The adjacent matrix for nodes has been calculated." << std::endl;


	// Create edge for the graph
	int edge_number = 0;
	int nonzero_edge_number = 0;
	for(auto i=0;i<k_number;i++)
	{
		const auto & node = graph.getNode(i);
		for(auto j=i+1;j<k_number;j++)
		{
			// That means the two regions are neighbored.
			if(label_edge[i][j] < 0xffffff)
			{
				const auto & other = graph.getNode(j);
				ImageEdge edge;
				edge.n = graph.getNode(i).id;
				edge.m = graph.getNode(j).id;
				//edge.w = (*distance)(node, other);
				edge.w = label_edge[edge.n][edge.m];
				graph.addEdge(edge);

				//if (edge.w > 100)
				//	std::cout << label_edge[i][j] << "\t" << i << "\t" << j << std::endl;
				edge_number++;
				if (edge.w) nonzero_edge_number++;
			}
		}
	}
	std::cout << "The edges have been add to the graph."<< std::endl;
	std::cout << "Edge number : \t" <<edge_number<< std::endl;
	std::cout << "Nonzero edge number : \t"<<nonzero_edge_number << std::endl;


	//Debug 20191106
	// for(auto i=0;i<graph.getNumNodes();i++)
	// {
	// 	auto& node = graph.getNode(i);
	// 	if(node.max_w!=0)
	// 	std::cout << node.l << "\t" << node.id << "\t" << node.n << "\t" << node.max_w << std::endl;
	// }



}

void GraphSegmentation::oversegmentGraph() {

	// Sort edges.
	graph.sortEdges();

	for(auto i=graph.getNumEdges()-1;i>=0;i--)
	{
		//std::cout << graph.getEdge(i).n <<"\t" << graph.getEdge(i).m << "\t" << graph.getEdge(i).w << std::endl;
	}

	for (int e = 0; e < graph.getNumEdges(); e++) {
		ImageEdge edge = graph.getEdge(e%graph.getNumEdges());

		// Assume that n less than m.
		ImageNode & n = graph.getNode(edge.n);
		ImageNode & m = graph.getNode(edge.m);

		ImageNode & S_n = graph.findNodeComponent(n);
		ImageNode & S_m = graph.findNodeComponent(m);

		// Are the nodes in different components?
		if (S_m.id != S_n.id) {

			// Here comes the magic!
			if ((*magic)(S_n, S_m, edge)) {

				//if(edge.w>100)
				//{
				//	std::cout << edge.n << "\t" << edge.m << "\t" << edge.w << std::endl;
				//}

				graph.merge(S_n, S_m, edge);
			}
		}
	}
	std::cout << "The graph has been merged." << std::endl;

}

void GraphSegmentation::enforceMinimumSegmentSize(int M) {
	assert(graph.getNumNodes() > 0);
	// assert(graph.getNumEdges() > 0);

	for (int e = 0; e < graph.getNumEdges(); e++) {
		ImageEdge edge = graph.getEdge(e);

		ImageNode & n = graph.getNode(edge.n);
		ImageNode & m = graph.getNode(edge.m);

		ImageNode & S_n = graph.findNodeComponent(n);
		ImageNode & S_m = graph.findNodeComponent(m);

		if (S_n.l != S_m.l) {
			if (S_n.n < M || S_m.n < M) {
				graph.merge(S_n, S_m, edge);
			}
		}
	}
	std::cout << "The minimum segmentation has been merged." << std::endl;

}


void GraphSegmentation::deriveLabels(int * merged_label) {




	int cnt = 0;
	for(auto i=0;i<graph.getNumNodes();i++)
	{
		ImageNode & node = graph.getNode(i);
		ImageNode & S_node = graph.findNodeComponent(node);
		const auto label_id = S_node.id;
		if (merged_label[S_node.voxels[0]] != -1) continue;
		for (auto voxel : S_node.voxels)
		{
			merged_label[voxel] = label_id;
		}
		cnt++;
	}
	std::cout << "Merged label has been calculated. The merged number is : "<< cnt <<"\t"<<graph.getNumComponents()<< std::endl;
}