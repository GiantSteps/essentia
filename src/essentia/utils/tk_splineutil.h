/*
 * spline.h
 *
 * simple cubic spline interpolation library without external
 * dependencies
 *
 * ---------------------------------------------------------------------
 * Copyright (C) 2011, 2014 Tino Kluge (ttk448 at gmail.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------
 *
 */


#ifndef _tk_spline_h
#define _tk_spline_h

#include <cstdio>
#include <cassert>
#include <vector>
namespace essentia{
namespace util{


template <typename T>
void FreeAll( T & t ) {
    T tmp;
    t.swap( tmp );
}


        
        
        // spline interpolation
        class Tk_spline {
        protected:
            std::vector<double> m_x,m_y;           // x,y coordinates of points
            // interpolation parameters
            // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i
            std::vector<double> m_a,m_b,m_c,m_d;
        public:
        Tk_spline(){};
    	~Tk_spline(){
    	FreeAll(m_x);
    	FreeAll(m_y);
    	FreeAll(m_a);
    	FreeAll(m_b);
    	FreeAll(m_c);
    	FreeAll(m_d);
    	
    	};
            void set_points( std::vector<double> x,
                             std::vector<double> y, bool cubic_spline=true);
            double operator() (double x) const;
        };
        
        



        // band matrix solver
        class tk_band_matrix {
        protected:
            std::vector< std::vector<double> > m_upper;  // upper band
            std::vector< std::vector<double> > m_lower;  // lower band
        public:
            tk_band_matrix() {};                             // constructor
            tk_band_matrix(int dim, int n_u, int n_l);       // constructor
            ~tk_band_matrix() {
            std::vector< std::vector<double> > emp ;
            m_upper.swap(emp);
            m_lower.swap(emp);
            };                            // destructor
            void resize(int dim, int n_u, int n_l);      // init with dim,n_u,n_l
            int dim() const;                             // matrix dimension
            int num_upper() const {
                return m_upper.size()-1;
            }
            int num_lower() const {
                return m_lower.size()-1;
            }
            // access operator
            double & operator () (int i, int j);            // write
            double   operator () (int i, int j) const;      // read
            // we can store an additional diogonal (in m_lower)
            double& saved_diag(int i);
            double  saved_diag(int i) const;
            void lu_decompose();
            std::vector<double> r_solve(const std::vector<double> b) const;
            std::vector<double> l_solve(const std::vector<double> b) const;
            std::vector<double> lu_solve(const std::vector<double> b,
                                         bool is_lu_decomposed=false);
            
        };

}// namespace essentia
}//namespace util
#endif /* _tk_spline_h */