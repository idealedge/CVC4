/*********************                                                        */
/*! \file ce_guided_conjecture.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief class that encapsulates counterexample-guided instantiation
 **        techniques for a single SyGuS synthesis conjecture
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__QUANTIFIERS__CE_GUIDED_CONJECTURE_H
#define __CVC4__THEORY__QUANTIFIERS__CE_GUIDED_CONJECTURE_H

#include <memory>

#include "theory/quantifiers/ce_guided_pbe.h"
#include "theory/quantifiers/ce_guided_single_inv.h"
#include "theory/quantifiers/sygus_grammar_cons.h"
#include "theory/quantifiers/sygus_process_conj.h"
#include "theory/quantifiers_engine.h"

namespace CVC4 {
namespace theory {
namespace quantifiers {

/** a synthesis conjecture
 * This class implements approaches for a synthesis conecjture, given by data
 * member d_quant.
 * This includes both approaches for synthesis in Reynolds et al CAV 2015. It
 * determines which approach and optimizations are applicable to the
 * conjecture, and has interfaces for implementing them.
 */
class CegConjecture {
public:
  CegConjecture( QuantifiersEngine * qe );
  ~CegConjecture();
  /** get original version of conjecture */
  Node getConjecture() { return d_quant; }
  /** get deep embedding version of conjecture */
  Node getEmbeddedConjecture() { return d_embed_quant; }
  /** get next decision request */
  Node getNextDecisionRequest( unsigned& priority );

  //-------------------------------for counterexample-guided check/refine
  /** increment the number of times we have successfully done candidate
   * refinement */
  void incrementRefineCount() { d_refine_count++; }
  /** whether the conjecture is waiting for a call to doCheck below */
  bool needsCheck( std::vector< Node >& lem );
  /** whether the conjecture is waiting for a call to doRefine below */
  bool needsRefinement();
  /** get the list of candidates */
  void getCandidateList( std::vector< Node >& clist, bool forceOrig = false );
  /** do single invocation check 
  * This updates Gamma for an iteration of step 2 of Figure 1 of Reynolds et al CAV 2015.
  */
  void doSingleInvCheck(std::vector< Node >& lems);
  /** do syntax-guided enumerative check 
  * This is step 2(a) of Figure 3 of Reynolds et al CAV 2015.
  */
  void doCheck(std::vector< Node >& lems, std::vector< Node >& model_values);
  /** do basic check 
  * This is called for non-SyGuS synthesis conjectures
  */
  void doBasicCheck(std::vector< Node >& lems);
  /** do refinement 
  * This is step 2(b) of Figure 3 of Reynolds et al CAV 2015.
  */
  void doRefine(std::vector< Node >& lems);
  /** Print the synthesis solution
   * singleInvocation is whether the solution was found by single invocation techniques.
   */
  //-------------------------------end for counterexample-guided check/refine

  void printSynthSolution( std::ostream& out, bool singleInvocation );
  /** get guard, this is "G" in Figure 3 of Reynolds et al CAV 2015 */
  Node getGuard();
  /** is ground */
  bool isGround() { return d_inner_vars.empty(); }
  /** does this conjecture correspond to a syntax-guided synthesis input */
  bool isSyntaxGuided() const { return d_syntax_guided; }
  /** are we using single invocation techniques */
  bool isSingleInvocation() const;
  /** preregister conjecture 
  * This is used as a heuristic for solution reconstruction, so that we 
  * remember expressions in the conjecture before preprocessing, since they
  * may be helpful during solution reconstruction (Figure 5 of Reynolds et al CAV 2015)
  */
  void preregisterConjecture( Node q );
  /** assign conjecture q to this class */
  void assign( Node q );
  /** has a conjecture been assigned to this class */
  bool isAssigned() { return !d_embed_quant.isNull(); }
  /** get model values for terms n, store in vector v */
  void getModelValues( std::vector< Node >& n, std::vector< Node >& v );
  /** get model value for term n */
  Node getModelValue( Node n );

  //-----------------------------------refinement lemmas
  /** get number of refinement lemmas we have added so far */
  unsigned getNumRefinementLemmas() { return d_refinement_lemmas.size(); }
  /** get refinement lemma */
  Node getRefinementLemma( unsigned i ) { return d_refinement_lemmas[i]; }
  /** get refinement lemma */
  Node getRefinementBaseLemma( unsigned i ) { return d_refinement_lemmas_base[i]; }
  //-----------------------------------end refinement lemmas

  /** get program by examples utility */
  CegConjecturePbe* getPbe() { return d_ceg_pbe.get(); }
  /** get utility for static preprocessing and analysis of conjectures */
  CegConjectureProcess* getProcess() { return d_ceg_proc.get(); }
  /** get the symmetry breaking predicate for type */
  Node getSymmetryBreakingPredicate(
      Node x, Node e, TypeNode tn, unsigned tindex, unsigned depth);
  /** print out debug information about this conjecture */
  void debugPrint( const char * c );
private:
  /** reference to quantifier engine */
  QuantifiersEngine * d_qe;
  /** single invocation utility */
  std::unique_ptr<CegConjectureSingleInv> d_ceg_si;
  /** program by examples utility */
  std::unique_ptr<CegConjecturePbe> d_ceg_pbe;
  /** utility for static preprocessing and analysis of conjectures */
  std::unique_ptr<CegConjectureProcess> d_ceg_proc;
  /** grammar utility */
  std::unique_ptr<CegGrammarConstructor> d_ceg_gc;
  /** list of constants for quantified formula
  * The Skolems for the negation of d_embed_quant.
  */
  std::vector< Node > d_candidates;
  /** base instantiation
  * If d_embed_quant is forall d. exists y. P( d, y ), then
  * this is the formula  P( candidates, y ).
  */
  Node d_base_inst;
  /** expand base inst to disjuncts */
  std::vector< Node > d_base_disj;
  /** list of variables on inner quantification */
  std::vector< Node > d_inner_vars;
  std::vector< std::vector< Node > > d_inner_vars_disj;
  /** current extential quantifeirs whose couterexamples we must refine */
  std::vector< std::vector< Node > > d_ce_sk;

  //-----------------------------------refinement lemmas
  /** refinement lemmas */
  std::vector< Node > d_refinement_lemmas;
  std::vector< Node > d_refinement_lemmas_base;
  //-----------------------------------end refinement lemmas

  /** quantified formula asserted */
  Node d_quant;
  /** quantified formula (after simplification) */
  Node d_simp_quant;
  /** quantified formula (after simplification, conversion to deep embedding) */
  Node d_embed_quant;
  /** candidate information */
  class CandidateInfo {
  public:
    CandidateInfo(){}
    /** list of terms we have instantiated candidates with */
    std::vector< Node > d_inst;
  };
  std::map< Node, CandidateInfo > d_cinfo;  
  /** number of times we have called doRefine */
  unsigned d_refine_count;
  /** construct candidates */
  bool constructCandidates( std::vector< Node >& clist, std::vector< Node >& model_values, 
                            std::vector< Node >& candidate_values, std::vector< Node >& lems );
  /** get candidadate */
  Node getCandidate( unsigned int i ) { return d_candidates[i]; }
  /** record instantiation (this is used to construct solutions later) */
  void recordInstantiation( std::vector< Node >& vs ) {
    Assert( vs.size()==d_candidates.size() );
    for( unsigned i=0; i<vs.size(); i++ ){
      d_cinfo[d_candidates[i]].d_inst.push_back( vs[i] );
    }
  }
  //-------------------------------- sygus stream
  /** the streaming guards for sygus streaming mode */
  std::vector< Node > d_stream_guards;
  /** get current stream guard */
  Node getCurrentStreamGuard() const;
  //-------------------------------- end sygus stream
  //-------------------------------- non-syntax guided (deprecated)
  /** Whether we are syntax-guided (e.g. was the input in SyGuS format).
   * This includes SyGuS inputs where no syntactic restrictions are provided.
   */
  bool d_syntax_guided;
  /** the guard for non-syntax-guided synthesis */
  Node d_nsg_guard;
  //-------------------------------- end non-syntax guided (deprecated)
};

} /* namespace CVC4::theory::quantifiers */
} /* namespace CVC4::theory */
} /* namespace CVC4 */

#endif
