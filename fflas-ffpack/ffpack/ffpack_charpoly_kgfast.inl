/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

/* ffpack/ffpack_charpoly_kgfast.inl
 * Copyright (C) 2004 Clement Pernet
 *
 * Written by Clement Pernet <Clement.Pernet@imag.fr>
 *
 * See COPYING for license information.
 */

#ifndef MIN
#define MIN(a,b) (a<b)?a:b
#endif

namespace FFPACK {
	namespace Protected {
		//---------------------------------------------------------------------
		// CharPoly: Compute the characteristic polynomial of A using
		// Keller-Gehrig's fast algorithm. A must be generic.
		//---------------------------------------------------------------------

		template <class Field, class Polynomial>
		int
		KGFast ( const Field& F, std::list<Polynomial>& charp,
			 const size_t N,
			 typename Field::Element * A, const size_t lda,
			 size_t * kg_mc, size_t* kg_mb, size_t* kg_j )
		{

			//std::cerr<<"Dans KGFast"<<std::endl;
			static typename Field::Element one, zero, mone;
			F.init(one, 1.0);
			F.neg(mone, one);
			F.init(zero, 0.0);
			size_t mc=N>>1; // Matrix A is transformed into a mc_Frobenius form
			size_t mb=N-mc;
			size_t r;
			typename Field::Element * C, *B;


			while ( mc > 0 ) {
#if 0
				std::cerr<<"Boucle1: mc,mb,N="<<mc<<" "<<mb<<" "<<N<<std::endl;
				write_field( F, std::cerr, A, N, N, lda );
#endif
				size_t j=0;
				C = A + (N-mc);
				//std::cerr<<std::endl<<"mc="<<mc<<":";
				while ( (j+1)*mc < N ) {
					mb = MIN ( mb, N-(j+1)*mc );
#if 0
					std::cerr<<"Boucle2: j,mb="<<j<<" "<<mb<<std::endl;
					write_field( F, std::cerr, A, N, N, lda );
#endif
					B = A + (N-mc-mb);

					// B1 <- C1^-1.B1
					typename Field::Element * LUP = new typename Field::Element[mc*mc];
					for (size_t i=0; i<mc; ++i)
						FFLAS::fcopy( F, mc, LUP+i*mc, 1, C+i*lda, 1);
					size_t * P = new size_t[mc];
					size_t * Q = new size_t[mc];

					if ( (r = LUdivine( F, FFLAS::FflasNonUnit, FFLAS::FflasNoTrans, mc, mc,
							    LUP, mc, P, Q, FfpackLQUP)) < mc ){
						* kg_mc = mc;
						* kg_mb = mb;
						* kg_j = j;
						delete[] P;
						delete[] Q;
						delete[] LUP;
						return -1;

					}
#if 0
					std::cerr<<"LUP="<<std::endl;
					write_field( F, std::cerr, LUP, mc, mc, mc );
#endif
					//std::cerr<<" "<<r;
					ftrsm(F, FFLAS::FflasLeft, FFLAS::FflasLower, FFLAS::FflasNoTrans, FFLAS::FflasUnit,
					      mc, mb, one, LUP, mc , B, lda);
					ftrsm(F, FFLAS::FflasLeft, FFLAS::FflasUpper, FFLAS::FflasNoTrans, FFLAS::FflasNonUnit,
					      mc, mb, one, LUP, mc , B, lda);
					delete[] LUP;
					applyP( F, FFLAS::FflasLeft, FFLAS::FflasTrans, mb, 0, (int)mc, B, lda, P );

					delete[] P;
					delete[] Q;
#if 0
					std::cerr<<"Apres B1<-C1^-1"<<std::endl;
					write_field( F, std::cerr, A, N, N, lda );
#endif

					// B2 <- B2 - C2.B1
					fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, N-mc, mb, mc,
					      mone, C+mc*lda, lda, B, lda,
					      one, B+mc*lda, lda);

#if 0
					std::cerr<<"Apres B2<-B2-C2.B1"<<std::endl;
					write_field( F, std::cerr, A, N, N, lda );
#endif

					// Shifting B: B1;B2 -> B2;B1
					typename Field::Element * tmp = new typename Field::Element[mc*mb];
					for (size_t i=0; i<mc; ++i)
						FFLAS::fcopy( F, mb, tmp+i*mb, 1, B+i*lda, 1);
					for (size_t i=mc; i<N; ++i)
						FFLAS::fcopy( F, mb, B+(i-mc)*lda, 1, B+i*lda, 1);
					for (size_t i=0; i<mc; ++i)
						FFLAS::fcopy( F, mb, B+(i+N-mc)*lda, 1, tmp+i*mb, 1);
					delete[] tmp;
#if 0
					std::cerr<<"Apres shift de B"<<std::endl;
					write_field( F, std::cerr, A, N, N, lda );
#endif


					// C3 <- B3.C1 + C3
					fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, (j+1)*mc, mc, mb,
					      one, B+(N-(j+1)*mc)*lda, lda, C+(N-(j+1)*mc-mb)*lda, lda,
					      one, C+(N-(j+1)*mc)*lda, lda);
#if 0
					std::cerr<<"C3 <- B3.C1 + C3: B3="<<std::endl;
					write_field( F, std::cerr, B+(N-(j+1)*mc)*lda, (j+1)*mc, mb, lda );
					std::cerr<<"C3 <- B3.C1 + C3: C1"<<std::endl;
					write_field( F, std::cerr,  C+(N-(j+1)*mc-mb)*lda, mb, mc, lda );
					std::cerr<<"C3 <- B3.C1 + C3: C3="<<std::endl;
					write_field( F, std::cerr, C+(N-(j+1)*mc)*lda, (j+1)*mc, mc, lda );
#endif

					int lambda = (int)(N - mb - (j+1)*mc );
					if ( int(mb) < lambda ){

#if 0
						std::cerr<<"mb<lambda"<<std::endl;
#endif
						typename Field::Element * tmp2 = new typename Field::Element[lambda*mc];

						// tmp2 <- C1
						for (int i=0; i<lambda; ++i)
							FFLAS::fcopy( F, mc, tmp2+i*mc, 1, C+i*lda, 1);

						// C1' <- B1.C2
						fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, mb, mc, mb,
						      one, B, lda, C+lambda*lda, lda,
						      zero, C, lda);

						// tmp2 <- B2.C2 + tmp2
						fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, lambda, mc, mb,
						      one, B+mb*lda, lda, C+lambda*lda, lda,
						      one, tmp2, mc);

						// C2' <- tmp2
						for (int i=0; i<lambda; ++i)
							FFLAS::fcopy( F, mc, C+mb*lda+i*lda, 1, tmp2+i*mc, 1);
						delete[] tmp2;
					}
					else if ( lambda > 0 ){
#if 0
						std::cerr<<"lambda>0"<<std::endl;
#endif

						typename Field::Element * tmp2 = new typename Field::Element[mb*mc];
						// C1 <- B2.C2 + C1
						fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, lambda, mc, mb,
						      one, B+mb*lda, lda, C+lambda*lda, lda,
						      one, C, lda);

						// tmp2 <-B1.C2
						fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, mb, mc, mb,
						      one, B, lda, C+lambda*lda, lda,
						      zero, tmp2, mc);

						// C2' <- C1
						for (int i=0; i<lambda; ++i)
							FFLAS::fcopy( F, mc, C+mb*lda+i*lda, 1, C+i*lda, 1);

						// C1' <- tmp2
						for (size_t i=0; i<mb; ++i)
							FFLAS::fcopy( F, mc, C+i*lda, 1, tmp2+i*mc, 1);
						delete[] tmp2;
					}
					else{
#if 0
						std::cerr<<"lambda<0"<<std::endl;
#endif
						mb = N - (j+1)*mc;
						typename Field::Element * tmp2 = new typename Field::Element[mb*mc];

						// tmp2 <-B1.C1
						fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, mb, mc, mb,
						      one, B, lda, C, lda,
						      zero, tmp2, mc);

						// C1' <- tmp2
						for (size_t i=0; i<mb; ++i)
							FFLAS::fcopy( F, mc, C+i*lda, 1, tmp2+i*mc, 1);
						delete[] tmp2;
					}

					j++;
				}
				mb = mc;
				mc>>=1;
				mb -= mc;
			}

			Polynomial *minP = new Polynomial();
			minP->resize(N+1);
			minP->operator[](N) = one;
			typename Polynomial::iterator it = minP->begin();
			for (size_t j=0; j<N; ++j, it++){
				F.neg(*it, *(A+N-1+j*lda));
			}
			charp.clear();
			charp.push_front(*minP);
			return 0;
		}

		template<class Field>
		void
		fgemv_kgf( const Field& F,  const size_t N,
			   const typename Field::Element * A, const size_t lda,
			   const typename Field::Element * X, const size_t incX,
			   typename Field::Element * Y, const size_t incY,
			   const size_t kg_mc, const size_t kg_mb, const size_t kg_j )
		{

			typename Field::Element one, zero;
			F.init(one, 1.0);
			F.init(zero, 0.0);
			size_t big_truc =kg_mb-kg_mc*(kg_j+1) ;
			size_t lambda = (N<big_truc)?(0):(N-big_truc);
			// Y1 <- X2
			FFLAS::fcopy ( F, lambda, Y, incY, X+(kg_mb+kg_mc)*incX, incX );
			// Y2 <- X.B
			fgemv( F, FFLAS::FflasTrans, N, kg_mb, one, A+N-kg_mc-kg_mb, lda, X, incX, zero, Y+lambda*incY, incY );
			// Y3 <- X3
			FFLAS::fcopy ( F, kg_j*kg_mc, Y+(lambda+kg_mb)*incY, incY, X+(lambda+kg_mb+kg_mc)*incX, incX );
			// Y4 <- X.C
			fgemv( F, FFLAS::FflasTrans, N, kg_mc, one, A+N-kg_mc, lda, X, incX, zero, Y+(N-kg_mc)*incY, incY );
		}

	} // Protected

} // FFPACK
