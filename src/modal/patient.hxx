#ifndef _MODAL_PATIENT_INCLUDE_H_
#define _MODAL_PATIENT_INCLUDE_H_

#include "cl.h"
#include "sil.h"

namespace modal {
    enum class gender_e
    {
        MALE, FEMALE, INTERSEX, UNKNOWN
    };

    std::string to_string(gender_e g)
    {
        switch (g) {
        case gender_e::MALE:
            return "Male";
        case gender_e::FEMALE :
            return "Female";
        case gender_e::UNKNOWN :
            return "Unknown";
        case gender_e::INTERSEX:
            return "Intersex";
        default:
            assert(0);
        }
    }

    struct patient_t {
        std::string name;
        std::string id; 
        cl::u8 age;
        gender_e gender;
        std::string category;
        std::string site;
        sil::image_t<cl::u8>* portrait;
    };

    bool init(patient_t* p);
    void drop(patient_t* p);
}

#endif //!_MODAL_PATIENT_INCLUDE_H_

#ifdef _MODAL_PATIENT_IMPLEMENTATION
#ifndef _MODAL_PATIENT_IMPLEMENTED

#endif // !_MODAL_PATIENT_IMPLEMENTATION
#endif //_MODAL_PATIENT_IMPLEMENTATION