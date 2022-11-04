#include "cmesh.h"
#include <set>
#include <iostream>
#include <unordered_map>
#include <stdio.h>

using namespace std;
#include "mat3f.h"
#include "box.h"
#include "tmbvh.hpp"

#define MAX_CD_PAIRS 4096*1024

extern mesh *cloths[16];
extern mesh *lions[16];

extern vector<int> vtx_set;
extern set<int> cloth_set;
extern set<int> lion_set;
static bvh *bvhCloth = NULL;
static bvh *bvhBody = NULL;

bool findd;

#include <omp.h>

# define	TIMING_BEGIN \
	{double tmp_timing_start = omp_get_wtime();

# define	TIMING_END(message) \
	{double tmp_timing_finish = omp_get_wtime();\
	double  tmp_timing_duration = tmp_timing_finish - tmp_timing_start;\
	printf("%s: %2.5f seconds\n", (message), tmp_timing_duration);}}


// CPU with BVH
void buildBVH()
{

	TIMING_BEGIN
	static std::vector<mesh *> meshes;

		if (bvhCloth == NULL)
		{
			for (int i = 0; i < 16; i++)
				if (cloths[i] != NULL)
					meshes.push_back(cloths[i]);

			bvhCloth = new bvh(meshes);
		}

	bvhCloth->refit(meshes);
	TIMING_END("bvh done...")
}

void drawBVH(int level)
{
	if (bvhCloth == NULL) return;
	bvhCloth->visualize(level);
}


int maxLevel = 60;
std::vector<std::vector<int>> gAdjInfo; // 存储的是每个三角形的邻接三角形（邻接指的是两个三角形的包围盒相交）
extern std::vector<std::vector<int>> adjoinTriangles; // 三角形的邻接三角形
std::vector<REAL> gIntensity[2];
int currentPass = -1; // 用这个来表示当前 gIntensity 是第几个，在 0 和 1 之间交替
std::vector<int> gSources; // 热源点

struct pairhash {
public:
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U>& x) const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};

void buildAdjoinTriangles() {
	mesh* mc = cloths[0];
	int face_num = mc->getNbFaces();
	int vertex_num = mc->getNbVertices();
	adjoinTriangles.clear();
	adjoinTriangles.resize(face_num);

	std::unordered_map<std::pair<int, int>, int, pairhash> edges2trangle; // 每条边对应的三角形

	for (int i = 0; i < face_num; i++) {
		for (int j = 0; j < 3; j++) {
			int k = (j + 1) % 3;
			int a = mc->_tris[i].id(j);
			int b = mc->_tris[i].id(k);

			if (a > b) {
				int temp = a;
				a = b;
				b = temp;
			}

			std::pair<int, int> edge(a, b);
			if (edges2trangle.count(edge) == 0) {
				edges2trangle[edge] = i;
			}
			else {
				adjoinTriangles[i].push_back(edges2trangle[edge]);
				adjoinTriangles[edges2trangle[edge]].push_back(i);
			}
		}
	}

	for (int i = 0; i < adjoinTriangles.size(); i++) {
		if (adjoinTriangles[i].size() != 3) {
			std::cout << "error occurs because the " << i << "th triangle does not have 3 adjoin triangles." << std::endl;
		}
	}
}

//void doPropogate()
//{
//	int prevPass = currentPass;
//	currentPass = 1 - currentPass;
//
//	mesh *mc = cloths[0];
//	int num = mc->getNbFaces();
//
//	TIMING_BEGIN
//		for (int i = 0; i < num; i++) {
//			std::vector<int> &adjs = gAdjInfo[i];
//			// printf("adjs size is: %d", adjs.size());
//			gIntensity[currentPass][i] = gIntensity[prevPass][i];
//			for (int j = 0; j < adjs.size(); j++) {
//				int tj = adjs[j];
//				gIntensity[currentPass][i] += gIntensity[prevPass][tj];
//			}
//
//			gIntensity[currentPass][i] /= REAL(adjs.size() + 1);
//		}
//
//	for (int i = 0; i < gSources.size(); i++) {
//		gIntensity[currentPass][gSources[i]] = 1.0;
//	}
//	TIMING_END("propogating...")
//}

//extern void buildIt();
extern "C" int doPropogateGPU();

void doIt()
{
	// 类似于 update() 函数，每帧调用一次
	if (currentPass == -1) {

		// 构建三角形邻接关系
		buildAdjoinTriangles();

		// 第一次执行
		currentPass = 0;

		gSources.push_back(100);
		gSources.push_back(10);
		gSources.push_back(200);

		mesh *mc = cloths[0];
		int num = mc->getNbFaces();
		gIntensity[0].resize(num);
		gIntensity[1].resize(num);

		for (int i = 0; i < num; i++) {
			gIntensity[currentPass][i] = 0;
		}

		for (int i = 0; i < gSources.size(); i++) {
			gIntensity[currentPass][gSources[i]] = 1.0;
		}
	}
	else {
		doPropogateGPU();
	}
}

void buildIt()
{
	mesh *mc = cloths[0];
	int num = mc->getNbFaces();
	gAdjInfo.clear();
	gAdjInfo.resize(num);

	TIMING_BEGIN
	for (int i = 0; i < num; i++) {
		BOX bx = mc->getTriBox(i);
		bvhCloth->query(bx, gAdjInfo[i], i);
	}
	TIMING_END("build adj info")
}
