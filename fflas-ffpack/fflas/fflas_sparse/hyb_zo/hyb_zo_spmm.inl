/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/*
 * Copyright (C) 2014 the FFLAS-FFPACK group
 *
 * Written by   Bastien Vialla <bastien.vialla@lirmm.fr>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * ========LICENCE========
 *.
 */

#ifndef __FFLASFFPACK_fflas_sparse_HYB_ZO_spmm_INL
#define __FFLASFFPACK_fflas_sparse_HYB_ZO_spmm_INL

namespace FFLAS {
namespace sparse_details_impl {

template <class Field>
inline void fspmm(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                  typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                  FieldCategories::GenericTag) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one(F, *(A.one), blockSize, x, ldx, y, ldy, FieldCategories::GenericTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone(F, *(A.mone), blockSize, x, ldx, y, ldy, FieldCategories::GenericTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm(F, *(A.dat), blockSize, x, ldx, y, ldy, FieldCategories::GenericTag());
}

template <class Field>
inline void fspmm(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                  typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                  FieldCategories::UnparametricTag) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one(F, *(A.one), blockSize, x, ldx, y, ldy, FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone(F, *(A.mone), blockSize, x, ldx, y, FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm(F, *(A.dat), blockSize, x, ldx, y, ldy, FieldCategories::UnparametricTag());
}

#ifdef __FFLASFFPACK_USE_SIMD

template <class Field>
inline void fspmm_simd_aligned(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                               typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                               FieldCategories::UnparametricTag) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one_simd_aligned(F, *(A.one), blockSize, x, ldx, y, ldy,
                                                     FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone_simd_aligned(F, *(A.mone), blockSize, x, ldx, y, ldy,
                                                    FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm_simd_aligned(F, *(A.dat), blockSize, x, ldx, y, ldy,
                                                FieldCategories::UnparametricTag());
}

template <class Field>
inline void fspmm_simd_unaligned(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                                 typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                                 FieldCategories::UnparametricTag) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one_simd_unaligned(F, *(A.one), blockSize, x, ldx, y, ldy,
                                                       FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone_simd_unaligned(F, *(A.mone), blockSize, x, ldx, y, ldy,
                                                      FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm_simd_unaligned(F, *(A.dat), blockSize, x, ldx, y, ldy,
                                                  FieldCategories::UnparametricTag());
}

#endif

template <class Field>
inline void fspmm(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                  typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy, uint64_t kmax) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one(F, *(A.one), blockSize, x, ldx, y, ldy, FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone(F, *(A.mone), blockSize, x, ldx, y, ldy, FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm(F, *(A.dat), blockSize, x, ldx, y, ldy, kmax);
}

#ifdef __FFLASFFPACK_USE_SIMD

template <class Field>
inline void fspmm_simd_aligned(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                               typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                               uint64_t kmax) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one_simd_aligned(F, *(A.one), blockSize, x, ldx, y, ldy,
                                                     FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone_simd_aligned(F, *(A.mone), blockSize, x, ldx, y, ldy,
                                                    FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm_simd_aligned(F, *(A.dat), blockSize, x, ldx, y, ldy, kmax);
}

template <class Field>
inline void fspmm_simd_unaligned(const Field &F, const Sparse<Field, SparseMatrix_t::HYB_ZO> &A, size_t blockSize,
                                 typename Field::ConstElement_ptr x, int ldx, typename Field::Element_ptr y, int ldy,
                                 uint64_t kmax) {
    if (A.one != nullptr)
        sparse_details_impl::fspmm_one_simd_unaligned(F, *(A.one), blockSize, x, ldx, y, ldy,
                                                       FieldCategories::UnparametricTag());
    if (A.mone != nullptr)
        sparse_details_impl::fspmm_mone_simd_unaligned(F, *(A.mone), blockSize, x, ldx, y, ldy,
                                                      FieldCategories::UnparametricTag());
    if (A.dat != nullptr)
        sparse_details_impl::fspmm_simd_unaligned(F, *(A.dat), blockSize, x, ldx, y, ldy, kmax);
}

#endif

} // HYB_ZO_details

} // FFLAS

#endif //  __FFLASFFPACK_fflas_HYB_ZO_spmm_INL
