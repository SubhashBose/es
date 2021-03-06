// 
// File    : ES_Synow_Grid.cc
// --------------------------
//
// ES: Elementary Supernova Spectrum Synthesis, Copyright (c) 2010, The
// Regents of the University of California, through Lawrence Berkeley
// National Laboratory (subject to receipt of any required approvals
// from the U.S. Dept. of Energy). All rights reserved.
//
// If you have questions about your rights to use or distribute this
// software, please contact Berkeley Lab's Technology Transfer
// Department at TTD@lbl.gov.
//
// NOTICE. This software was developed under partial funding from the
// U.S. Department of Energy. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up,
// nonexclusive, irrevocable, worldwide license in the Software to
// reproduce, prepare derivative works, and perform publicly and display
// publicly. Beginning five (5) years after the date permission to
// assert copyright is obtained from the U.S. Department of Energy, and
// subject to any subsequent five (5) year renewals, the U.S. Government
// is granted for itself and others acting on its behalf a paid-up,
// nonexclusive, irrevocable, worldwide license in the Software to
// reproduce, prepare derivative works, distribute copies to the public,
// perform publicly and display publicly, and to permit others to do so. 
//

#include "ES_Blackbody.hh"
#include "ES_Synow_Grid.hh"
#include "ES_Synow_Setup.hh"

#include <cmath>
#include <iostream>

#define B_MARGIN_FACTOR 3
#define R_MARGIN_FACTOR 1
#define C_KKMS          299.792

ES::Synow::Grid ES::Synow::Grid::create( double const min_output_wl, double const max_output_wl, double const bin_width, 
        int const v_size, double const v_outer_max )
{
    double min_wl = min_output_wl / ( 1.0 + B_MARGIN_FACTOR * v_outer_max / C_KKMS );
    double max_wl = max_output_wl * ( 1.0 + R_MARGIN_FACTOR * v_outer_max / C_KKMS );
    ES::Synow::Grid grid( min_wl, max_wl, bin_width, v_size );
    return grid;
}

ES::Synow::Grid::Grid( double const min_wl_, double const max_wl_, double const bin_width_, int const v_size_ ) :
    min_wl( min_wl_ ), max_wl( max_wl_ ), bin_width( bin_width_ ), v_size( v_size_ ) 
{
    wl_size = int( log( max_wl / min_wl ) / log( 1.0 + bin_width / 299.792 ) + 0.5 );
    wl  = new double[ wl_size ];
    v   = new double[  v_size ];
    tau = new double[ wl_size * v_size ];
    src = new double[ wl_size * v_size ];
    bb  = new Blackbody();
    _zero();
}

ES::Synow::Grid::~Grid()
{
    delete [] wl;
    delete [] v;
    delete [] tau;
    delete [] src;
    delete bb;
}

void ES::Synow::Grid::reset( ES::Synow::Setup& setup )
{
    _zero();
    double v_step = ( setup.v_outer - setup.v_phot ) / double( v_size - 1 );
    for( int i = 1; i < v_size - 1; ++ i ) v [ i ] = setup.v_phot + i * v_step;
    v[ 0 ] = setup.v_phot;
    v[ v_size - 1 ] = setup.v_outer;
    bb->temp() = setup.t_phot;
    (*bb)( min_wl );
    (*bb)( max_wl );
}

void ES::Synow::Grid::_zero()
{
    wl_used = 0;
    for( int i = 0; i < wl_size * v_size; ++ i )
    {
        if( i < wl_size ) wl[ i ] = 0.0;
        if( i < v_size  ) v [ i ] = 0.0;
        tau[ i ] = 0.0;
        src[ i ] = 0.0;
    }
}

