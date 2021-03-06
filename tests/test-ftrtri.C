/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s

/*
 * Copyright (C) FFLAS-FFPACK
 * Written by Clément Pernet <clement.pernet@imag.fr>
 * This file is Free Software and part of FFLAS-FFPACK.
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

//--------------------------------------------------------------------------
//                        Test for ftrtri : 1 computation
//
//--------------------------------------------------------------------------
// Clement Pernet
//-------------------------------------------------------------------------

#define DEBUG 1
#define TIME 1

#include <iomanip>
#include <iostream>
#include "givaro/modular-balanced.h"

#include "fflas-ffpack/fflas-ffpack-config.h"

#include "fflas-ffpack/utils/timer.h"
#include "Matio.h"
#include "fflas-ffpack/ffpack/ffpack.h"



using namespace std;
using namespace FFPACK;

typedef Givaro::ModularBalanced<double> Field;

int main(int argc, char** argv)
{

	int n;
	int nbit=atoi(argv[3]); // number of times the product is performed
	cerr<<setprecision(10);

	if (argc != 4)	{
		cerr<<"Usage : test-ftrtri <p> <A> <<i>"
		    <<endl
		    <<"         to invert a triangular matrix  A mod p (i computations)"
		    <<endl;
		exit(-1);
	}
	Field F(atoi(argv[1]));
	Field::Element * A,*Ab;
	A = read_field(F,argv[2],&n,&n);
	Ab = FFLAS::fflas_new<Field::Element>(n*n);

	for (int i=0; i<n;++i){
		for(int j=i+1; j<n; ++j)
			F.assign(*(Ab+i*n+j),*(A+i*n+j));
		F.assign(*(Ab+i*(n+1)), 1.0);
		for(int j=0; j<i; ++j)
			F.assign(*(Ab+i*n+j),0.0);
	}


	Field::Element * X = FFLAS::fflas_new<Field::Element>(n*n);

 FFLAS::Timer tim,t; t.clear();tim.clear();

	for(int i = 0;i<nbit;++i){
		t.clear();
		t.start();
//				FFPACK::trinv_left (F, n, A, n, X, n);
				FFPACK::ftrtri(F, FFLAS::FflasUpper, FFLAS::FflasUnit, n, A, n);
		t.stop();
		tim+=t;
		if (i+1<nbit)
			for (int i=0; i<n*n;++i)
				F.assign(*(A+i),*(Ab+i));
	}

#if DEBUG
	FFLAS::ftrmm (F, FFLAS::FflasRight, FFLAS::FflasUpper, FFLAS::FflasNoTrans, FFLAS::FflasUnit,
		      n, n, 1.0,
		      A,n, Ab, n);
	bool wrong = false;

	for (int i=0;i<n;++i)
		for (int j=0;j<n;++j)
			if ( ((i!=j) && !F.isZero(*(Ab+i*n+j)))
			     ||((i==j) &&!F.isOne(*(Ab+i*n+j))))
				wrong = true;

	if ( wrong ){
		cerr<<"FAIL"<<endl;
		write_field (F,cerr<<"Ab="<<endl,Ab,n,n,n);
		    //write_field (F,cerr<<"X="<<endl,X,n,n,n);
	}else{

		cerr<<"PASS"<<endl;
	}
#endif

#if TIME
	double gflops = 1.0/3.0*(n/1000.0*n/1000000.0)*nbit*n/tim.usertime();
	cerr<<"n = "<<n<<" Inversion over Z/"<<atoi(argv[1])<<"Z : t= "
	     << tim.usertime()/nbit
	     << " s, Gfops = "<<gflops
	     << endl;

	cout<<n<<" "<<gflops<<" "<<tim.usertime()/nbit<<endl;
#endif
}
