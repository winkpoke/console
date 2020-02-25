#ifndef _CONTROL_PATIENT_INCLUDE_H_
#define _CONTROL_PATIENT_INCLUDE_H_

#include "cl.h"
#include "modal.hxx"


namespace mod::patient::control {
    namespace modal = mod::patient::modal;
    struct patient_t {
         cl::shared_ptr<modal::patient_t> modal;
         std::shared_mutex mutex;
         bool dirty;
    };

    bool init(patient_t* p, modal::patient_t* modal);
    bool init(patient_t* p, cl::shared_ptr<modal::patient_t> modal);
    void drop(patient_t* p);

    bool is_dirty(patient_t* p);
    void set_dirty(patient_t* p);
    void clear_dirty(patient_t* p);
}

#endif //!_CONTROL_PATIENT_INCLUDE_H_

#ifdef CONTROL_PATIENT_IMPLEMENTATION
#ifndef CONTROL_PATIENT_IMPLEMENTED
#define CONTROL_PATIENT_IMPLEMENTED
namespace mod::patient::control {
    using namespace mod::patient;
    bool init(patient_t* p, modal::patient_t* modal)
    {
        assert(p);
        assert(modal);
        new(&p->modal)cl::shared_ptr<modal::patient_t>(modal);
        new(&p->mutex)std::shared_mutex;
        p->dirty = false;
        return true;
    }

    bool init(patient_t* p, cl::shared_ptr<modal::patient_t> modal)
    {
        assert(p);
        assert(modal);
        new(&p->modal)cl::shared_ptr<modal::patient_t>(modal);
        new(&p->mutex)std::shared_mutex;
        p->dirty = false;
        return true;
    }

    void drop(patient_t* p)
    {
        assert(p);
        if (p) {
            p->modal.~shared_ptr();
            p->mutex.~shared_mutex();
            free(p);
        }
    }

    bool is_dirty(patient_t* p)
    {
        assert(p);
        std::shared_lock lk(p->mutex);
        return p->dirty;
    }

    void set_dirty(patient_t* p)
    {
        assert(p);
        std::scoped_lock lk(p->mutex);
        p->dirty = true;
    }

    void clear_dirty(patient_t* p)
    {
        assert(p);
        std::scoped_lock lk(p->mutex);
        p->dirty = false;
    }
}
#endif // !CONTROL_PATIENT_IMPLEMENTATION
#endif //CONTROL_PATIENT_IMPLEMENTATION