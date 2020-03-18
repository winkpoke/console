#ifndef _CONSOLE_CONFIG_INCLUDE_H_
#define _CONSOLE_CONFIG_INCLUDE_H_

namespace control {
    struct config_t {
        std::string raw_data_folder;
        std::string raw_data_basename;
        std::string output_folder;
        static const char* CONFIG_PATH;
        static config_t* _this;
    };
    bool init(config_t* p);
    void drop(config_t*);

    config_t* get_config();
}

#endif //!_CONSOLE_CONFIG_INCLUDE_H_

#ifdef CONSOLE_CONFIG_IMPLEMENTATION
#ifndef CONSOLE_CONFIG_IMPLEMENTED
#define CONSOLE_CONFIG_IMPLEMENTED

#include <fstream>

#define TOML_EXCEPTIONS 0
#include "toml++/toml.h"

#include "cl.h"


namespace control {
    config_t* config_t::_this = nullptr;
    const char* config_t::CONFIG_PATH = "config.toml";
    
    bool init(config_t* p)
    {
        assert(p);

        new (&p->raw_data_folder)string;
        new (&p->raw_data_basename)string;
        new (&p->output_folder)string;

        auto result = toml::parse_file(config_t::CONFIG_PATH);

        if (!result) {
            SPDLOG_ERROR("Error: read config.toml.");
            return false;
        }

        auto& config = result.get();
        auto path = config["raw_data_folder"].value<std::string_view>();
        if (!path) {
            SPDLOG_ERROR("key raw_data_folder is missing in config.toml file.");
            return false;
        }
        auto basename = config["raw_data_basename"].value<std::string_view>();
        if (!basename) {
            SPDLOG_ERROR("key raw_data_basename is missing in config.toml file.");
            return false;
        }

        auto output_folder = config["output_folder"].value<std::string_view>();
        if (!output_folder) {
            SPDLOG_ERROR("key output_folder is missing in config.toml file.");
            return false;
        }

        p->raw_data_folder = *path;
        p->raw_data_basename = *basename;
        p->output_folder = *output_folder;
        
        return true;
    }

    void drop(config_t* p) {
        
    }

    config_t* get_config()
    {
        if (!config_t::_this) {
            config_t::_this = cl::build_raw<config_t>();
        }
        return config_t::_this;
    }
}

#endif // !CONSOLE_CONFIG_IMPLEMENTATION
#endif //CONSOLE_CONFIG_IMPLEMENTATION