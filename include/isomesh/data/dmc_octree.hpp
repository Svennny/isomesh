/* This file is part of Isomesh library, released under MIT license.
  Copyright (c) 2018-2019 Pavel Asyutchenko (sventeam@yandex.ru) */
/** \file
	\brief Octree data storage for DMC algorithm
*/
#pragma once

#include "../common.hpp"
#include "../data/mesh.hpp"
#include "../field/scalar_field.hpp"
#include "../qef/qef_solver_4d.hpp"
#include "dmc_octree_node.hpp"

#include <random>

namespace isomesh
{

// TODO: make generic octree
class DMC_Octree {
public:
	explicit DMC_Octree (int32_t root_size, glm::dvec3 global_pos = glm::dvec3 (0), double global_scale = 1);
	
	void build (const ScalarField &field, QefSolver4D &solver, float epsilon,
	            bool use_simple_split_policy = false, bool use_random_sampling = true,
	            bool use_early_split_stop = false, uint32_t seed = 0xDEADBEEF);
	Mesh contour () const;

	// Mappings between local and global coordinate spaces
	glm::dvec3 localToGlobal (const glm::dvec3 &L) const noexcept { return L * m_globalScale + m_globalPos; }
	glm::dvec3 globalToLocal (const glm::dvec3 &G) const noexcept { return (G - m_globalPos) / m_globalScale; }
private:
	const glm::dvec3 m_globalPos;
	const double m_globalScale;
	const int32_t m_rootSize;
	
	DMC_OctreeNode m_root;

	struct BuildArgs {
		const ScalarField &field;
		QefSolver4D &solver;
		float epsilon;
		bool use_simple_split_policy;
		bool use_random_sampling;
		bool use_early_split_stop;
		std::mt19937 rng;
	};

	void buildNode (DMC_OctreeNode *node, glm::ivec3 min_corner, int32_t size, BuildArgs &args);
	bool generateDualVertex (DMC_OctreeNode *node, glm::ivec3 min_corner, int32_t size, BuildArgs &args);
	// Using error function from "3D Finite Element Meshing from Imaging Data" (Zhang, Bajaj, Sohn)
	bool shouldSplit (glm::ivec3 min_corner, int32_t size, BuildArgs &args);
	bool shouldStopSplitting (glm::ivec3 min_corner, int32_t size, BuildArgs &args);
};

}
