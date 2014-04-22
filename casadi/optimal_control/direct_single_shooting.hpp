/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef DIRECT_SINGLE_SHOOTING_HPP
#define DIRECT_SINGLE_SHOOTING_HPP

#include "casadi/symbolic/function/ocp_solver.hpp"
#include "casadi/symbolic/function/nlp_solver.hpp"

#include <casadi/optimal_control/casadi_optimal_control_export.h>

namespace casadi {
  class DirectSingleShootingInternal;


  /** \brief Direct Single Shooting
   *
   *   ns: Number of shooting nodes: from option number_of_grid_points\n
   *   nx: Number of differential states: from <tt>ffcn.input(INTEGRATOR_X0).size()</tt> \n
   *   nc: Number of constants during integration: <tt>ffcn.input(INTEGRATOR_P).size()</tt>
   *   nu: Number of controls: from nc - np \n
   *   np: Number of parameters: from option number_of_parameters\n
   *   nh: Number of point constraints: from <tt>cfcn.input(0).size()</tt>
   *
   *
   *   \author Joel Andersson
   *   \date 2013
  */
class CASADI_OPTIMAL_CONTROL_EXPORT DirectSingleShooting : public OCPSolver {
  public:
    /// Default constructor
    DirectSingleShooting();

    /** \brief Create a multiple shooting OCP solver
    * \param ffcn Continuous time dynamics, an casadi::Function with the following mapping:
    * \copydoc scheme_DAEInput
    * \copydoc scheme_DAEOutput
    * Important notes:
    *  - In the above table, INTEGRATOR_P input is not really of shape (np x 1),
    *    but rather ( (np+nu) x 1 ).
    *  - The first np entries of the INTEGRATOR_P input are interpreted as parameters
    *    to be optimized but constant over the whole domain.
    *    The remainder are interpreted as controls.
    *  - BEWARE: if the right hand side of \a ffcn is dependent on time, the results will be incorrect.
    *
    * \param mfcn Mayer term, casadi::Function mapping to cost (1 x 1)
    * @copydoc scheme_MayerInput
    * \param cfcn Path constraints, casadi::Function mapping to (nh x 1)
    * @copydoc scheme_DAEInput
    * \param rfcn Initial value constraints
    */
    explicit DirectSingleShooting(const Function& ffcn, const Function& mfcn,
                                  const Function& cfcn=Function(),
                                  const Function& rfcn=Function());

    /// Access functions of the node
    DirectSingleShootingInternal* operator->();

    /// Const access functions of the node
    const DirectSingleShootingInternal* operator->() const;

    /// Get the variables
    void getGuess(std::vector<double>& V_init) const;

    /// Get the variables
    void getVariableBounds(std::vector<double>& V_min, std::vector<double>& V_max) const;

    /// Get the constraints
    void getConstraintBounds(std::vector<double>& G_min, std::vector<double>& G_max) const;

    /// Set the optimal solution
    void setOptimalSolution(const std::vector<double> &V_opt);

    // Access the underlying NLPSolver object
    NLPSolver getNLPSolver() const;

    // Prints out a human readable report about possible constraint violations, after solving
    void reportConstraints(std::ostream &stream=std::cout);

    std::string getReportConstraints()
    { std::stringstream s; reportConstraints(s); return s.str(); }




};

} // namespace casadi

#endif // DIRECT_SINGLE_SHOOTING_HPP
