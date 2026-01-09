#ifndef GEARLOADER_HPP
#define GEARLOADER_HPP

#include "gearLoader_c.h"
#include <string>



inline bool operator==(const SemanticVersion& a, const SemanticVersion& b) {
    return a.major == b.major &&
        a.minor == b.minor &&
        a.patchNum == b.patchNum;
}
inline bool operator>(const SemanticVersion& a, const SemanticVersion& b) {
    return a.major > b.major ||
        (a.major == b.major && a.minor > b.minor) ||
        (a.major == b.major && a.minor == b.minor && a.patchNum > b.patchNum);
}
inline bool operator!=(const SemanticVersion& a, const SemanticVersion& b) { return !(a == b); }
inline bool operator<(const SemanticVersion& a, const SemanticVersion& b) { return b > a; }
inline bool operator>=(const SemanticVersion& a, const SemanticVersion& b) { return !(b > a); }
inline bool operator<=(const SemanticVersion& a, const SemanticVersion& b) { return !(a > b); }

namespace GearLoader {

    class Api {
    public:
        Api(GearLoaderApi* c_api)
            : base(c_api){}

        template<typename ApiType>
        int RetrieveModApi(GearLoaderContext* ctx,
                           std::string name,
                           std::string versionConstraint,
                           const ApiType** pApi,
                           SemanticVersion* retrievedVersion) {
            const void* retApi;
            int result = base->RetrieveModApi(
                ctx,
                name.c_str(),
                versionConstraint.c_str(),
                &retApi,
                retrievedVersion
            );

            *pApi = reinterpret_cast<const ApiType*>(retApi);
            return result;
        }
        int RegisterApi (GearLoaderContext* ctx,
                         const void* api,
                         std::string name,
                         SemanticVersion version) {
            return base->RegisterApi(ctx, api, name.c_str(), version);
        }
        
    private:
        GearLoaderApi* base;
    };

}

#endif
