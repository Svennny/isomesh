/* This file is part of Isomesh library, released under MIT license.
  Copyright (c) 2018-2019 Pavel Asyutchenko (sventeam@yandex.ru) */
/** \file
	\brief Scalar field (data source) interface
*/
#pragma once

#include "../common.hpp"

namespace isomesh
{

/** \brief Scalar field interface

	Algorithms build the surface as zero isosurface of the scalar field. To be able
	to do that properly, this scalar field should have these properties:
	- Be continuous over R^3
	- Have its gradient (or at least some approximation) defined everywhere
	- Have negative sign inside the surface and positive outside (i.e. every point with zero value
	shall have at least one point with negative value and one with positive in its neighbourhood)
*/
class ScalarField {
public:
	virtual ~ScalarField () = default;
	/** \brief Computes scalar field value at a given point
	
		\param[in] x,y,z Coordinates of the point
		\return Value of the scalar field in point (x, y, z)
	*/
	virtual double value (double x, double y, double z) const noexcept = 0;
	/** \brief Computes scalar field gradient at a given point
	
		\param[in] x,y,z Coordinates of the point
		\return Gradient of the scalar field in point (x, y, z)
	*/
	virtual glm::dvec3 grad (double x, double y, double z) const noexcept = 0;
	/// Shorthand for \ref value
	double operator () (double x, double y, double z) const noexcept { return value (x, y, z); }
	/// Shorthand for \ref value, using glm::dvec3 instead of separate variables
	double operator () (const glm::dvec3 &p) const noexcept { return value (p.x, p.y, p.z); }
	/// Same as \ref grad(double,double,double), using glm::dvec3 instead of separate variables
	glm::dvec3 grad (const glm::dvec3 &p) const noexcept { return grad (p.x, p.y, p.z); }
};

}