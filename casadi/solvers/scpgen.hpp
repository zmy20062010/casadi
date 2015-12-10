/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
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


#ifndef CASADI_SCPGEN_HPP
#define CASADI_SCPGEN_HPP

#include "casadi/core/function/nlpsol.hpp"

#include <casadi/solvers/casadi_nlpsol_scpgen_export.h>

#include <deque>

/** \defgroup plugin_Nlpsol_scpgen
   A structure-exploiting sequential quadratic programming
     (to be come sequential convex programming) method for nonlinear programming.
*/

/** \pluginsection{Nlpsol,scpgen} */

/// \cond INTERNAL

namespace casadi {

  /**  \brief \pluginbrief{Nlpsol,scpgen}

     @copydoc NLPSolver_doc
     @copydoc plugin_Nlpsol_scpgen

     \author Joel Andersson, Attila Kozma and Joris Gillis
     \date 2013
  */
  class CASADI_NLPSOL_SCPGEN_EXPORT Scpgen : public Nlpsol {
  public:
    explicit Scpgen(const std::string& name, const XProblem& nlp);
    virtual ~Scpgen();

    // Get name of the plugin
    virtual const char* plugin_name() const { return "scpgen";}

    /** \brief  Create a new NLP Solver */
    static Nlpsol* creator(const std::string& name, const XProblem& nlp) {
      return new Scpgen(name, nlp);
    }

    // Initialize the solver
    virtual void init();

    // Reset the solver
    virtual void reset(void* mem, const double**& arg, double**& res, int*& iw, double*& w);

    // Solve the NLP
    virtual void solve(void* mem);

    // Calculate the L1-norm of the primal infeasibility
    double primalInfeasibility();

    // Calculate the L1-norm of the dual infeasibility
    double dualInfeasibility();

    // Print iteration header
    void printIteration(std::ostream &stream);

    // Print iteration
    void printIteration(std::ostream &stream, int iter, double obj, double pr_inf, double du_inf,
                        double reg, int ls_trials, bool ls_success);

    // Evaluate the matrices in the condensed QP
    void eval_mat();

    // Evaluate the vectors in the condensed QP
    void eval_vec();

    // Evaluate the residual function
    void eval_res();

    // Regularize the condensed QP
    void regularize();

    // Solve the QP to get the (full) step
    void solve_qp();

    // Perform the line-search to take the step
    void line_search(int& ls_iter, bool& ls_success);

    // Evaluate the step expansion
    void eval_exp();

    // Timings
    double t_eval_mat_, t_eval_res_, t_eval_vec_, t_eval_exp_, t_solve_qp_, t_mainloop_;

    /// QP solver for the subproblems
    Function qpsol_;

    /// use Gauss-Newton Hessian
    bool gauss_newton_;

    /// maximum number of sqp iterations
    int max_iter_;

    /// Memory size of L-BFGS method
    int lbfgs_memory_;

    /// Tolerance on primal infeasibility
    double tol_pr_;

    /// Tolerance on dual infeasibility
    double tol_du_;

    /// Tolerance on regularization
    double tol_reg_;

    /// stopping criterion for the stepsize
    double tol_pr_step_;

    /// stopping criterion for the Lagrangian gradient
    double tol_gl_;

    /// Linesearch parameters
    ///@{
    double c1_;
    double beta_;
    int max_iter_ls_;
    std::vector<double> merit_mem_;
    int merit_memsize_;
    double merit_start_;
    int merit_ind_;
    ///@}

    /// Enable Code generation
    bool codegen_;

    /// Access Qpsol
    const Function getQpsol() const { return qpsol_;}

    /// Regularization
    bool regularize_;

    // Number of gauss_newton equations
    int ngn_;

    // Options
    double reg_threshold_;

    /// Print timers
    bool print_time_;

    /// Generate initial guess for lifted variables
    Function vinit_fcn_;

    /// Residual function
    Function res_fcn_;

    // Function to calculate the matrices in the reduced QP
    Function mat_fcn_;
    int mat_jac_, mat_hes_;

    /// Quadratic approximation
    Function vec_fcn_;
    int vec_gf_, vec_g_;

    /// Step expansion
    Function exp_fcn_;

    // Residual function io indices
    int res_x_, res_p_, res_g_lam_, res_p_lam_, res_p_d_;
    int res_f_, res_gl_, res_g_;

    // Modifier function io indices
    int mod_x_, mod_p_, mod_g_lam_;
    int mod_f_, mod_gl_, mod_g_;
    int mod_du_, mod_dlam_g_;

    struct Var {
      int n;
      MX v, v_def, v_lam, v_defL;
      MX d, d_def, d_lam, d_defL;

      // Indices of function inputs and outputs
      int res_var, res_lam, res_d, res_lam_d;
      int mod_var, mod_lam, mod_def, mod_defL;
      int exp_def, exp_defL;
    };

    std::vector<Var> v_;

    // Names of the components
    std::vector<std::string> name_x_;

    // Components to print
    std::vector<int> print_x_;

    // QP sparsity
    Sparsity spH_, spA_, spL_;

    /// A documentation string
    static const std::string meta_doc;

    // Current objective value
    double fk_;

    // Work vectors, nonlifted problem
    double *xk_, *gk_, *dxk_, *lam_xk_, *dlam_xk_, *lam_gk_, *dlam_gk_, *gfk_, *gL_, *b_gn_;

    // Memory for lifted variables
    struct VarMem {
      int n;
      double *dx, *x0, *x, *lam, *dlam;
      double *res, *resL;
    };
    std::vector<VarMem> lifted_mem_;

    // Penalty parameter of merit function
    double sigma_;

    // 1-norm of last primal step
    double pr_step_;

    // 1-norm of last dual step
    double du_step_;

    // Regularization
    double reg_;

    // Message applying to a particular iteration
    std::string iteration_note_;

    // QP
    double *qpH_, *qpA_, *qpB_, *qpL_, *qpG_, *qpH_times_du_;

    // QP solver
    double *qp_lbx_, *qp_ubx_, *qp_lba_, *qp_uba_;
  };

} // namespace casadi

/// \endcond

#endif // CASADI_SCPGEN_HPP
