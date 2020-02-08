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
        // the program shall not be here
        // but just want to make sure every path
        // returns a value
        return "Unknown";
    }

    struct patient_t {
        using unique_ptr = std::unique_ptr<patient_t, void(patient_t*)>;
        char name[256];
        char id[256]; 
        cl::u8 age;
        gender_e gender;
        char category[256];
        char site[256];
        sil::image_t<cl::u8>* portrait;
    };

    bool init(patient_t* p);
    bool init(patient_t* p, const char* name, const char* id, cl::u8 age,
        gender_e gender, const char* category, const char* site);
    bool init(patient_t* p, const char* name, const char* id, cl::u8 age, 
        gender_e gender, const char* category, const char* site, 
        cl::unique_ptr<sil::image_t<cl::u8>> portrait);
    bool init(patient_t* p, std::string str);
    void drop(patient_t* p);

    std::string to_json(patient_t* p);
    bool from_json(patient_t* p, std::string& str);
}

#endif //!_MODAL_PATIENT_INCLUDE_H_

#ifdef MODAL_PATIENT_IMPLEMENTATION
#ifndef MODAL_PATIENT_IMPLEMENTED

#include "base64.hxx"

namespace modal {
    using json = nlohmann::json;

    bool init(patient_t* p)
    {
        assert(p);
        return true;
    }

    bool init(patient_t* p, const char* name, const char* id, cl::u8 age,
        gender_e gender, const char* category, const char* site)
    {
        if (!p || !name || !id || !category || !site) {
            return false;
        }
        strncpy(p->name, name, sizeof(p->name));
        strncpy(p->id, id, sizeof(p->id));
        strncpy(p->category, category, sizeof(p->category));
        strncpy(p->site, site, sizeof(p->site));

        p->age = age;
        p->gender = gender;
        return true;
    }

    bool init(patient_t* p, const char* name, const char* id, cl::u8 age,
        gender_e gender, const char* category, const char* site,
        cl::unique_ptr<sil::image_t<cl::u8>> portrait)
    {

        init(p, name, id, age, gender, category, site);

        if (portrait) {
            // transfer the ownership
            p->portrait = portrait.release();
        }
        return true;
    }

    bool init(patient_t* p, std::string str)
    {
        return from_json(p, str);
    }
    
    void drop(patient_t* p)
    {
        if (p) {
            drop(p->portrait);
            free(p);
        }
    }

    bool from_json(patient_t* p, std::string& str)
    {
        assert(p);
        try {
            auto j = json::parse(str);
            std::string name = j[u8"name"];
            std::string id = j[u8"id"];
            std::string category = j[u8"category"];
            std::string site = j[u8"site"];
            
            strncpy(p->name, name.c_str(), 256);
            strncpy(p->id, id.c_str(), 256);
            strncpy(p->category, category.c_str(), 256);
            strncpy(p->site, site.c_str(), 256);
            
            j.at("age").get_to(p->age);
            j.at("gender").get_to(p->gender);
            
            auto portrait = j.find("portrait");
            if (portrait != j.end()) {
                int width = j["portrait"]["width"];
                int height = j["portrait"]["height"];
                int channel = j["portrait"]["channel"];
                size_t input_len = j["portrait"]["len"];
                std::string str = j["portrait"]["data"];
                size_t output_len;
                auto data = base64_decode(str.c_str(), str.length(), &output_len);

                p->portrait = cl::build_raw <sil::image_t<cl::u8>>(width, height, channel, data);
            }
        }
        catch (...) {
            return false;
        }
        return true;
    }

    std::string to_json(patient_t* p)
    {
        json* j = new json;
        (*j)["name"] = p->name;
        (*j)["id"] = p->id;
        (*j)["age"] = p->age;
        (*j)["gender"] = p->gender;

        (*j)["category"] = p->category;
        (*j)["site"] = p->site;
        if (p->portrait) {
            size_t len;
            auto img = p->portrait;
            const char* data = base64_encode(img->data, img->width * img->height * img->channel * sizeof(cl::u8), &len);
            (*j)["portrait"]["width"] = img->width;
            (*j)["portrait"]["height"] = img->height;
            (*j)["portrait"]["channel"] = img->channel;
            (*j)["portrait"]["len"] = len;
            (*j)["portrait"]["data"] = data;
            free((void*)data);
        }
        return j->dump();
    }
}
#endif // !MODAL_PATIENT_IMPLEMENTATION
#endif //MODAL_PATIENT_IMPLEMENTATION