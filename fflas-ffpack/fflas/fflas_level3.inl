/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/*
 * Copyright (C) 2014 the FFLAS-FFPACK group
 *
 * Written by Clement Pernet <Clement.Pernet@imag.fr>
 *            BB <bbboyer@ncsu.edu>
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

/** @file fflas/fflas_level3.h
 * @brief  Matrix-Matrix operations
 * or anything of \f$>n^2\f$ complexity.
 */

#ifndef __FFLASFFPACK_fflas_fflas_level3_INL
#define __FFLASFFPACK_fflas_fflas_level3_INL

#include "fflas_bounds.inl"
#include "fflas_helpers.inl"

namespace FFLAS { namespace Protected {
	//-----------------------------------------------------------------------------
	// Some conversion functions
	//-----------------------------------------------------------------------------


	//---------------------------------------------------------------------
	// Finite Field matrix => double matrix
	// Special design for upper-triangular matrices
	//---------------------------------------------------------------------
	template<class Field>
	void MatF2MatD_Triangular (const Field& F,
				   DoubleDomain::Element* S, const size_t lds,
				   const typename Field::Element* const E,
				   const size_t lde,
				   const size_t m, const size_t n)
	{

		const typename Field::Element* Ei = E;
		DoubleDomain::Element* Si = S;
		size_t i=0, j;
		for ( ; i<m;++i, Ei+=lde, Si+=lds)
			for ( j=i; j<n;++j)
				F.convert(*(Si+j),*(Ei+j));
	}

	//---------------------------------------------------------------------
	// Finite Field matrix => float matrix
	// Special design for upper-triangular matrices
	//---------------------------------------------------------------------
	//! @todo do finit(...,FFLAS_TRANS,FFLAS_DIAG)
	//! @todo do fconvert(...,FFLAS_TRANS,FFLAS_DIAG)
	template<class Field>
	void MatF2MatFl_Triangular (const Field& F,
				    FloatDomain::Element* S, const size_t lds,
				    const typename Field::Element* const E,
				    const size_t lde,
				    const size_t m, const size_t n)
	{

		const typename Field::Element* Ei = E;
		FloatDomain::Element* Si = S;
		size_t i=0, j;
		for ( ; i<m;++i, Ei+=lde, Si+=lds)
			for ( j=i; j<n;++j)
				F.convert(*(Si+j),*(Ei+j));
	}

	/**
	 * Computes the maximal size for delaying the modular reduction
	 *         in a triangular system resolution.
	 *
	 *  Compute the maximal dimension k, such that a unit diagonal triangular
	 *  system of dimension k can be solved over Z without overflow of the
	 *  underlying floating point representation.
	 *
	 *  @bib
	 *  - Dumas, Giorgi, Pernet 06, arXiv:cs/0601133.
	 *
	 * \param F Finite Field/Ring of the computation
	 *
	 */
	// Specialized routines for ftrsm
	template <class Element> class ftrsmLeftUpperNoTransNonUnit;
	template <class Element> class ftrsmLeftUpperNoTransUnit;
	template <class Element> class ftrsmLeftUpperTransNonUnit;
	template <class Element> class ftrsmLeftUpperTransUnit;
	template <class Element> class ftrsmLeftLowerNoTransNonUnit;
	template <class Element> class ftrsmLeftLowerNoTransUnit;
	template <class Element> class ftrsmLeftLowerTransNonUnit;
	template <class Element> class ftrsmLeftLowerTransUnit;
	template <class Element> class ftrsmRightUpperNoTransNonUnit;
	template <class Element> class ftrsmRightUpperNoTransUnit;
	template <class Element> class ftrsmRightUpperTransNonUnit;
	template <class Element> class ftrsmRightUpperTransUnit;
	template <class Element> class ftrsmRightLowerNoTransNonUnit;
	template <class Element> class ftrsmRightLowerNoTransUnit;
	template <class Element> class ftrsmRightLowerTransNonUnit;
	template <class Element> class ftrsmRightLowerTransUnit;

	// Specialized routines for ftrmm
	template <class Element> class ftrmmLeftUpperNoTransNonUnit;
	template <class Element> class ftrmmLeftUpperNoTransUnit;
	template <class Element> class ftrmmLeftUpperTransNonUnit;
	template <class Element> class ftrmmLeftUpperTransUnit;
	template <class Element> class ftrmmLeftLowerNoTransNonUnit;
	template <class Element> class ftrmmLeftLowerNoTransUnit;
	template <class Element> class ftrmmLeftLowerTransNonUnit;
	template <class Element> class ftrmmLeftLowerTransUnit;
	template <class Element> class ftrmmRightUpperNoTransNonUnit;
	template <class Element> class ftrmmRightUpperNoTransUnit;
	template <class Element> class ftrmmRightUpperTransNonUnit;
	template <class Element> class ftrmmRightUpperTransUnit;
	template <class Element> class ftrmmRightLowerNoTransNonUnit;
	template <class Element> class ftrmmRightLowerNoTransUnit;
	template <class Element> class ftrmmRightLowerTransNonUnit;
	template <class Element> class ftrmmRightLowerTransUnit;

} // protected
} // FFLAS

namespace FFLAS {

	//---------------------------------------------------------------------
	// Level 3 routines
	//---------------------------------------------------------------------
            // set by default for ftrsm to be thread safe
	    // undef it at your own risk, and only if you run it in sequential
#define __FFLAS__TRSM_READONLY

	/** @brief ftrsm: <b>TR</b>iangular <b>S</b>ystem solve with <b>M</b>atrix.
	 * Computes  \f$ B \gets \alpha \mathrm{op}(A^{-1}) B\f$ or  \f$B \gets \alpha B \mathrm{op}(A^{-1})\f$.
	 * \param F field
	 * \param Side if \c Side==FflasLeft then  \f$ B \gets \alpha \mathrm{op}(A^{-1}) B\f$ is computed.
	 * \param Uplo if \c Uplo==FflasUpper then \p A is upper triangular
	 * \param TransA if \c TransA==FflasTrans then \f$\mathrm{op}(A)=A^t\f$.
	 * \param Diag if \c Diag==FflasUnit then \p A is unit.
	 * \param M rows of \p B
	 * \param N cols of \p B
	 * @param alpha scalar
	 * \param A triangular invertible matrix. If \c Side==FflasLeft then \p A is \f$N\times N\f$, otherwise \p A is \f$M\times M\f$
	 * @param lda leading dim of \p A
	 * @param B matrix of size \p MxN
	 * @param ldb leading dim of \p B
	 * @bug \f$\alpha\f$ must be non zero.
	 */
	template<class Field>
	void
	ftrsm (const Field& F, const FFLAS_SIDE Side,
	       const FFLAS_UPLO Uplo,
	       const FFLAS_TRANSPOSE TransA,
	       const FFLAS_DIAG Diag,
	       const size_t M, const size_t N,
	       const typename Field::Element alpha,
#ifdef __FFLAS__TRSM_READONLY
	       const
#endif
	       typename Field::Element * A, const size_t lda,
	       typename Field::Element * B, const size_t ldb);

	/** @brief ftrmm: <b>TR</b>iangular <b>M</b>atrix <b>M</b>ultiply.
	 * Computes  \f$ B \gets \alpha \mathrm{op}(A) B\f$ or  \f$B \gets \alpha B \mathrm{op}(A)\f$.
	 * @param F field
	 * \param Side if \c Side==FflasLeft then  \f$ B \gets \alpha \mathrm{op}(A) B\f$ is computed.
	 * \param Uplo if \c Uplo==FflasUpper then \p A is upper triangular
	 * \param TransA if \c TransA==FflasTrans then \f$\mathrm{op}(A)=A^t\f$.
	 * \param Diag if \c Diag==FflasUnit then \p A is implicitly unit.
	 * \param M rows of \p B
	 * \param N cols of \p B
	 * @param alpha scalar
	 * \param A triangular matrix. If \c Side==FflasLeft then \p A is \f$N\times N\f$, otherwise \p A is \f$M\times M\f$
	 * @param lda leading dim of \p A
	 * @param B matrix of size \p MxN
	 * @param ldb leading dim of \p B
	 */
	template<class Field>
	void
	ftrmm (const Field& F, const FFLAS_SIDE Side,
	       const FFLAS_UPLO Uplo,
	       const FFLAS_TRANSPOSE TransA,
	       const FFLAS_DIAG Diag,
	       const size_t M, const size_t N,
	       const typename Field::Element alpha,
	       typename Field::Element * A, const size_t lda,
	       typename Field::Element * B, const size_t ldb);

	/** @brief  fgemm: <b>F</b>ield <b>GE</b>neral <b>M</b>atrix <b>M</b>ultiply.
	 *
	 * Computes \f$C = \alpha \mathrm{op}(A) \times \mathrm{op}(B) + \beta C\f$
	 * Automatically set Winograd recursion level
	 * \param F field.
	 * \param ta if \c ta==FflasTrans then \f$\mathrm{op}(A)=A^t\f$, else \f$\mathrm{op}(A)=A\f$,
	 * \param tb same for matrix \p B
	 * \param m see \p A
	 * \param n see \p B
	 * \param k see \p A
	 * \param alpha scalar
	 * \param beta scalar
	 * \param A \f$\mathrm{op}(A)\f$ is \f$m \times k\f$
	 * \param B \f$\mathrm{op}(B)\f$ is \f$k \times n\f$
	 * \param C \f$C\f$ is \f$m \times n\f$
	 * \param lda leading dimension of \p A
	 * \param ldb leading dimension of \p B
	 * \param ldc leading dimension of \p C
	 * \param w recursive levels of Winograd's algorithm are used. No argument (or -1) does auto computation of \p w.
	 * @warning \f$\alpha\f$ \e must be invertible
	 */
	template<class Field>
	typename Field::Element*
	fgemm( const Field& F,
	       const FFLAS_TRANSPOSE ta,
	       const FFLAS_TRANSPOSE tb,
	       const size_t m,
	       const size_t n,
	       const size_t k,
	       const typename Field::Element alpha,
	       const typename Field::Element* A, const size_t lda,
	       const typename Field::Element* B, const size_t ldb,
	       const typename Field::Element beta,
	       typename Field::Element* C, const size_t ldc);

	/** @brief  fgemm: <b>F</b>ield <b>GE</b>neral <b>M</b>atrix <b>M</b>ultiply.
	 *
	 * Computes \f$C = \alpha \mathrm{op}(A) \times \mathrm{op}(B) + \beta C\f$
	 * Version with Helper. Input and Output are not supposed to be reduced.
	 * \param F field.
	 * \param ta if \c ta==FflasTrans then \f$\mathrm{op}(A)=A^t\f$, else \f$\mathrm{op}(A)=A\f$,
	 * \param tb same for matrix \p B
	 * \param m see \p A
	 * \param n see \p B
	 * \param k see \p A
	 * \param alpha scalar
	 * \param beta scalar
	 * \param A \f$\mathrm{op}(A)\f$ is \f$m \times k\f$
	 * \param B \f$\mathrm{op}(B)\f$ is \f$k \times n\f$
	 * \param C \f$C\f$ is \f$m \times n\f$
	 * \param lda leading dimension of \p A
	 * \param ldb leading dimension of \p B
	 * \param ldc leading dimension of \p C
	 * \param H helper, driving the computation (algorithm, delayed modular reduction, switch of base type, etc)
	 * @warning \f$\alpha\f$ \e must be invertible
	 */
	template<class Field, class AlgoT, class FieldTrait, class ParSeqTrait>
	inline  typename Field::Element*
	fgemm (const Field& F,
	       const FFLAS_TRANSPOSE ta,
	       const FFLAS_TRANSPOSE tb,
	       const size_t m, const size_t n, const size_t k,
	       const typename Field::Element alpha,
	       typename Field::Element * A, const size_t lda,
	       typename Field::Element * B, const size_t ldb,
	       const typename Field::Element beta,
	       typename Field::Element * C, const size_t ldc,
	       MMHelper<Field, AlgoT, FieldTrait, ParSeqTrait> & H);

} // FFLAS

#include "fflas-ffpack/fflas/parallel.h"

namespace FFLAS {
	//#ifdef __FFLASFFPACK_USE_OPENMP
	enum CuttingStrategy {
		ROW_FIXED	,
		COLUMN_FIXED	,
		BLOCK_FIXED	,
		ROW_THREADS	,
		COLUMN_THREADS	,
		BLOCK_THREADS	//,
	};


	/**
	 * @brief pfgemm: <b>P</b>arallel <b>F</b>ield <b>GE</b>neral <b>M</b>atrix <b>M</b>ultiply.
	 */
	// Parallel fgemm with OpenMP tasks
	template<class Field>
	typename Field::Element*
	pfgemm( const Field& F,
            const FFLAS_TRANSPOSE ta,
            const FFLAS_TRANSPOSE tb,
            const size_t m,
            const size_t n,
            const size_t k,
            const typename Field::Element alpha,
            typename Field::Element* A, const size_t lda,
            typename Field::Element* B, const size_t ldb,
            const typename Field::Element beta,
            typename Field::Element* C, const size_t ldc,
            const size_t w,
            const CuttingStrategy method,
            const int maxThreads  = HPAC_NUM_THREADS
            );

	// Parallel fgemm with OpenMP tasks
	// winograd level is automatic
	template<class Field>
	typename Field::Element*
	pfgemm( const Field& F,
            const FFLAS_TRANSPOSE ta,
            const FFLAS_TRANSPOSE tb,
            const size_t m,
            const size_t n,
            const size_t k,
            const typename Field::Element alpha,
            typename Field::Element* A, const size_t lda,
            typename Field::Element* B, const size_t ldb,
            const typename Field::Element beta,
            typename Field::Element* C, const size_t ldc,
            const CuttingStrategy method,
            const int maxThreads = HPAC_NUM_THREADS
            );



	//Parallel ftrsm with OpenMP tasks
	template<class Field>
	typename Field::Element*
	pftrsm( const Field& F,
		const FFLAS_SIDE Side,
		const FFLAS_UPLO UpLo,
		const FFLAS_TRANSPOSE TA,
		const FFLAS_DIAG Diag,
		const size_t m,
		const size_t n,
		const typename Field::Element alpha,
		const typename Field::Element* A, const size_t lda,
		typename Field::Element* B, const size_t ldb,
		const FFLAS::CuttingStrategy method,
                const size_t numThreads = HPAC_NUM_THREADS);
	//#endif

	/** @brief fsquare: Squares a matrix.
	 * compute \f$ C \gets \alpha \mathrm{op}(A) \mathrm{op}(A) + \beta C\f$ over a Field \p F
	 * Avoid the conversion of B
	 * @param ta  if \c ta==FflasTrans, \f$\mathrm{op}(A)=A^T\f$.
	 * @param F field
	 * @param n size of \p A
	 * @param alpha scalar
	 * @param beta scalar
	 * @param A dense matrix of size \c nxn
	 * @param lda leading dimension of \p A
	 * @param C dense matrix of size \c nxn
	 * @param ldc leading dimension of \p C
	 */
	template<class Field>
	typename Field::Element* fsquare (const Field& F,
					  const FFLAS_TRANSPOSE ta,
					  const size_t n,
					  const typename Field::Element alpha,
					  const typename Field::Element* A,
					  const size_t lda,
					  const typename Field::Element beta,
					  typename Field::Element* C,
					  const size_t ldc);


} // FFLAS

#include "fflas_fgemm.inl"

#endif // __FFLASFFPACK_fflas_fflas_level3_INL
