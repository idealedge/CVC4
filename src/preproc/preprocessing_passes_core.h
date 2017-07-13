#include "cvc4_public.h"

#ifndef __CVC4__PREPROC__PREPROCESSING_PASSES_CORE_H
#define __CVC4__PREPROC__PREPROCESSING_PASSES_CORE_H

#include "preproc/preprocessing_pass.h"
#include "theory/substitutions.h"
#include "smt/smt_engine.h"

namespace CVC4 {
namespace preproc {

typedef std::hash_map<Node, Node, NodeHashFunction> NodeMap;
typedef std::hash_map<Node, Node, NodeHashFunction> NodeToNodeHashMap;
typedef context::CDList<Node> NodeList;
  
class NlExtPurifyPass : public PreprocessingPass {
 public:
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  NlExtPurifyPass(ResourceManager* resourceManager);
 private:
  Node purifyNlTerms(TNode n, NodeMap& cache, NodeMap& bcache,
                     std::vector<Node>& var_eq, bool beneathMult = false);
};

class CEGuidedInstPass : public PreprocessingPass {
 public:
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  CEGuidedInstPass(ResourceManager* resourceManager, TheoryEngine* theoryEngine);
 private:
  TheoryEngine* d_theoryEngine;
};
 
class SolveRealAsIntPass : public PreprocessingPass {
 public:
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  SolveRealAsIntPass(ResourceManager* resourceManager);
 private:
  Node realToInt(TNode n, NodeMap& cache, std::vector<Node>& var_eq);
};

class SolveIntAsBVPass : public PreprocessingPass {
 public:
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  SolveIntAsBVPass(ResourceManager* resourceManager);
 private:
  Node intToBV(TNode n, NodeToNodeHashMap& cache);
  Node intToBVMakeBinary(TNode n, NodeMap& cache); 
};

class BitBlastModePass : public PreprocessingPass {
 public:
   virtual void apply(AssertionPipeline* assertionsToPreprocess);
   BitBlastModePass(ResourceManager* resourceManager,
      TheoryEngine* theoryEngine); 
 private:
   TheoryEngine* d_theoryEngine;
};

class BVAbstractionPass : public PreprocessingPass {
 public: 
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  BVAbstractionPass(ResourceManager* resourceManager, 
      SmtEngine* smt, TheoryEngine* theoryEngine);
 private:
  SmtEngine* d_smt;
  TheoryEngine* d_theoryEngine;
  // Abstract common structure over small domains to UF
  // return true if changes were made.
  void bvAbstraction(AssertionPipeline* assertionsToPreprocess);  
};

class UnconstrainedSimpPass : public PreprocessingPass {
 public:
  virtual void apply(AssertionPipeline* assertionsToPreprocess);
  UnconstrainedSimpPass(ResourceManager* resourceManager, 
      TimerStat unconstrainedSimpTime, TheoryEngine* theoryEngine);
 private:
  TimerStat d_unconstrainedSimpTime;
  TheoryEngine* d_theoryEngine;
  // Simplify based on unconstrained values
};

class RewritePass : public PreprocessingPass {
 public:
    virtual void apply(AssertionPipeline* assertionsToPreprocess);
    RewritePass(ResourceManager* resourceManager);
};
 
class NotUnsatCoresPass : public PreprocessingPass {
 public:
    virtual void apply(AssertionPipeline* assertionsToPreprocess);
    NotUnsatCoresPass(ResourceManager* resourceManager,
       theory::SubstitutionMap* topLevelSubstitutions);
 private:
    theory::SubstitutionMap* d_topLevelSubstitutions;
};
 
class BVToBoolPass : public PreprocessingPass {
 public:
   virtual void apply(AssertionPipeline* assertionsToPreprocess);
   BVToBoolPass(ResourceManager* resourceManager, 
      TheoryEngine* theoryEngine);
 private:
  // Lift bit-vectors of size 1 to booleans
  TheoryEngine* d_theoryEngine;
  void bvToBool(AssertionPipeline* assertionsToPreprocess);
};

class BoolToBVPass : public PreprocessingPass {
 public:
   virtual void apply(AssertionPipeline* assertionsTopreprocess);
   BoolToBVPass(ResourceManager* resourceManager,
     TheoryEngine* theoryEngine);
 private:
   // Convert booleans to bit-vectors of size 1
  TheoryEngine* d_theoryEngine;
  void boolToBv(AssertionPipeline* assertionsToPreprocess);
};

class SepPreSkolemEmpPass : public PreprocessingPass {
  public:
   virtual void apply(AssertionPipeline* assertionsToPreprocess);
   SepPreSkolemEmpPass(ResourceManager* resourceManager);
};

class QuantifiedPass : public PreprocessingPass {
  public:
    virtual void apply(AssertionPipeline* assertionsToPreprocess);
    QuantifiedPass(ResourceManager* resourceManager, 
      TheoryEngine* theoryEngine, NodeList* &fmfRecFunctionsDefined, 
      std::map<Node,TypeNode> &fmfRecFunctionsAbs, 
      std::map<Node, std::vector<Node> > &fmfRecFunctionsConcrete); 
  private:
    TheoryEngine* d_theoryEngine;
    NodeList* d_fmfRecFunctionsDefined;
    std::map<Node,TypeNode> d_fmfRecFunctionsAbs;
    std::map<Node, std::vector<Node> > d_fmfRecFunctionsConcrete;
};
    
}  // namespace preproc
}  // namespace CVC4

#endif /* __CVC4__PREPROC__PREPROCESSING_PASSES_CORE_H */
