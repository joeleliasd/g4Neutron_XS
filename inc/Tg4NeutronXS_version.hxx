#ifndef Tg4NeutronXS_version_hxx_seen
#define Tg4NeutronXS_version_hxx_seen

// Source for Tg4NeutronXS_version.hxx auto-generated using the
// Tpackage_version.hxx.in template file.

#include <TPackageVersion.hxx>

namespace ND {
    class Tg4NeutronXS_version;
};

/// This has fields for the library version and can add stuff to the TROOT
/// environment.
class ND::Tg4NeutronXS_version: public ND::TPackageVersion {
private:
    static Tg4NeutronXS_version* fThis;
    
public:
    Tg4NeutronXS_version();
    ~Tg4NeutronXS_version();

    /// Return a reference to the singleton.
    static Tg4NeutronXS_version* Get(void); 

    /// Return the version of this library.
    const char* GetName(void) const;

    /// Return the version of this library.
    const char* GetVersion(void) const;

    /// Return the date that this library was compiled.
    const char* GetCompilationDate(void) const;
    
    /// Return the host that this library was compiled on.
    const char* GetCompilationHost(void) const;

    /// Return the directory from which this library was compiled.
    const char* GetCompilationDirectory(void) const;

    /// Return the machine information for the machine that compiled this 
    /// library.  On most machines this is generated from "uname -a".
    const char* GetCompilationMachineInfo(void) const; 

    /// Do any initialization needed for the oaUtility library.  This is
    /// called by the Tg4NeutronXS_version constructor.
    void Initialize(void);
    
    ClassDef(Tg4NeutronXS_version,0);
};
#endif
