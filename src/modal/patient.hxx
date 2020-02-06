#ifndef _MODAL_PATIENT_INCLUDE_H_
#define _MODAL_PATIENT_INCLUDE_H_

#include "nlohmann/json.hpp"

#include "cl.h"
#include "sil.h"

namespace modal {
    using json = nlohmann::json;

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
        char name[256];
        char id[256]; 
        cl::u8 age;
        gender_e gender;
        char category[256];
        char site[256];
        sil::image_t<cl::u8>* portrait;
    };

    bool init(patient_t* p);
    void drop(patient_t* p);

    std::shared_ptr<json> to_json(patient_t* p);
    void from_json(patient_t* p);
}

#endif //!_MODAL_PATIENT_INCLUDE_H_

#ifdef MODAL_PATIENT_IMPLEMENTATION
#ifndef MODAL_PATIENT_IMPLEMENTED

namespace modal {
    using json = nlohmann::json;

    bool init(patient_t* p)
    {
        assert(p);
        return true;
    }
    
    void drop(patient_t* p)
    {
        if (p) {
            drop(p->portrait);
            free(p);
        }
    }

    std::shared_ptr<json> to_json(patient_t* p)
    {
        json* j = new json;
        (*j)["name"] = p->name;
        (*j)["id"] = p->id;
        (*j)["age"] = p->age;
        (*j)["gender"] = p->gender;
        (*j)["category"] = p->category;
        (*j)["site"] = p->site;

        return std::shared_ptr<json>(j);
    }
}
#endif // !MODAL_PATIENT_IMPLEMENTATION
#endif //MODAL_PATIENT_IMPLEMENTATION