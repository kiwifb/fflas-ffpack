/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

/* fflas/fflas_freduce.inl
 * Copyright (C) 2014 FFLAS FFPACK group
 *
 * Written by  Brice Boyer (briceboyer) <boyer.brice@gmail.com>
 *
 *
 * ========LICENCE========
 * This file is part of the library FFLAS-FFPACK.
 *
 * FFLAS-FFPACK is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */

#ifndef __FFLASFFPACK_fflas_freduce_H
#define __FFLASFFPACK_fflas_freduce_H

#include "fflas-ffpack/fflas/fflas_simd.h"
#include "fflas-ffpack/field/field-traits.h"
#include "fflas-ffpack/utils/cast.h"

namespace FFLAS {

	template<class T>
	struct support_simd_mod  : public std::false_type {} ;

#ifdef __FFLASFFPACK_USE_SIMD
	template<>
	struct support_simd_mod<float> : public std::true_type {} ;
	template<>
	struct support_simd_mod<double> : public std::true_type {} ;
#ifdef SIMD_INT
	template<>
	struct support_simd_mod<int64_t> : public std::true_type {} ;
#endif  // SIMD_INT

#endif // __FFLASFFPACK_USE_SIMD

} // FFLAS

#include "fflas-ffpack/fflas/fflas_freduce.inl"

namespace FFLAS {

	/***************************/
	/*         LEVEL 1         */
	/***************************/

	template<class Field>
	void
	freduce (const Field & F, const size_t m,
		 typename Field::ConstElement_ptr  B, const size_t incY,
		 typename Field::Element_ptr A, const size_t incX)
	{
		return details::freduce (F,m,B,incY,A,incX,typename FieldTraits<Field>::category());
	}

	template<class Field>
	void
	freduce (const Field & F, const size_t m,
		 typename Field::Element_ptr A, const size_t incX)
	{
		return details::freduce (F,m,A,incX,typename FieldTraits<Field>::category());
	}

	template<class Field>
	void
	freduce_constoverride(const Field & F, const size_t m,
			      typename Field::ConstElement_ptr A, const size_t incX)
	{
		return freduce(F, m, FFPACK::fflas_const_cast<typename Field::Element_ptr>(A), incX);
	}

	// OOOPS
        // CP: to be moved to a fflas_finit field, if ever needed
	template<class Field, class ConstOtherElement_ptr>
	void
	finit (const Field& F, const size_t n,
	       ConstOtherElement_ptr Y, const size_t incY,
	       typename Field::Element_ptr X, const size_t incX)
	{
		typename Field::Element_ptr Xi = X ;
		ConstOtherElement_ptr Yi = Y ;

		if (incX == 1 && incY == 1)
			for (; Yi < Y + n ; ++Xi, ++Yi)
				F.init( *Xi , *Yi);
		else
			for (; Yi < Y+n*incY; Xi+=incX, Yi += incY )
				F.init( *Xi , *Yi);
	}


	/***************************/
	/*         LEVEL 2         */
	/***************************/


	template<class Field>
	void
	freduce (const Field& F, const size_t m , const size_t n,
		 typename Field::Element_ptr A, const size_t lda)
	{
		if (n == lda)
			freduce (F, n*m, A, 1);
		else
			for (size_t i = 0 ; i < m ; ++i)
				freduce (F, n, A+i*lda, 1);
		return;
	}

	template<class Field>
	void
	freduce (const Field& F, const size_t m , const size_t n,
		 typename Field::ConstElement_ptr B, const size_t ldb,
		 typename Field::Element_ptr A, const size_t lda)
	{
		for (size_t i = 0 ; i < m ; ++i) {
			freduce(F,n,B+i*ldb,1,A+i*lda,1);
		}
	}


	template<class Field>
	void
	freduce_constoverride(const Field & F, const size_t m, const size_t n,
			      typename Field::ConstElement_ptr A, const size_t lda)
	{
		return freduce(F, m, n,
			       FFPACK::fflas_const_cast<typename Field::Element_ptr>(A), lda);
	}

        // CP: to be moved to a fflas_finit field, if ever needed
	template<class Field, class OtherElement_ptr>
	void
	finit (const Field& F, const size_t m , const size_t n,
	       const OtherElement_ptr B, const size_t ldb,
	       typename Field::Element_ptr A, const size_t lda)
	{
		if (n == lda && n == ldb)
			finit (F, n*m, B, 1, A, 1);
		else
			for (size_t i = 0 ; i < m ; ++i)
				finit (F, n, B + i*ldb, 1, A + i*lda, 1);
		return;
	}

} // end of namespace FFLAS

//#include "fflas_freduce_mp.inl" moved to fflas.h

#endif // __FFLASFFPACK_fflas_freduce_H
