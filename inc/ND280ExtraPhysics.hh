#ifndef ND280ExtraPhysics_hh_seen
#define ND280ExtraPhysics_hh_seen
////////////////////////////////////////////////////////////
// $Id$
////////////////////////////////////////////////////////////

#include "globals.hh"

#include "G4VPhysicsConstructor.hh"

/// A G4VPhysicsConstructor to provide extra physics processes required by the
/// ND280mc such as step length limiters.  The extra physics processes must be
/// explicitly added to every physics list (as an EM list) that is created in
/// ND280PhysicsList::AddPhysicsList(); however, this only effects code
/// actually in that method.  This class should not be used outside of
/// ND280PhysicsList.
class ND280ExtraPhysics: public G4VPhysicsConstructor {
public:

    ND280ExtraPhysics();
    virtual ~ND280ExtraPhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

};
#endif
