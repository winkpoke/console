#ifndef _UI_IMAGE_H_
#define _UI_IMAGE_H_

#include <memory>

#include "imgui.h"
//#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "sil.h"


namespace ui {
    template <class T>
    struct image_view {
        sil::image_t<T>* image;
        GLuint texture;
        cl::i32 width;
        cl::i32 height;
    };

    template <class T>
    bool init(image_view<T>* widget, cl::i32 width, cl::i32 height, sil::image_t<T>* image);

    template <class T>
    void drop(image_view<T>* widget);

    template <class T>
    void render(image_view<T>* widget);

    template <class T>
    image_view<T>* clone(image_view<T>* widget);

}  // namespace ui  

// Implementation
namespace ui {

    template <class T>
    constexpr auto gl_type() 
    {
        if (std::is_signed<T>::value) {
            switch (sizeof(T)) {
            case 1:
                return GL_BYTE;
            case 2:
                return GL_SHORT;
            case 4:
                return GL_INT;
            }
        }
        else {
            switch (sizeof(T)) {
            case 1:
                return GL_UNSIGNED_BYTE;
            case 2:
                return GL_UNSIGNED_SHORT;
            case 4:
                return GL_UNSIGNED_INT;
            }
        }
        return GL_NONE;
    }

    template <class T>
    bool init(image_view<T>* widget, cl::i32 width, cl::i32 height, sil::image_t<T>* image)
    {
        if (!widget) {
            return false;
        }
        glGenTextures(1, &widget->texture);
        glBindTexture(GL_TEXTURE_2D, widget->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        widget->width = width;
        widget->height = height;
        widget->image = image;

        const GLsizei image_width = static_cast<GLsizei>(sil::get_width(image));
        const GLsizei image_height = static_cast<GLsizei>(sil::get_height(image));
        const T* image_data = sil::get_data(image);
        
        static_assert(gl_type<T>() != GL_NONE, "wrong image type!");
        if (widget->image != NULL) {
            glBindTexture(GL_TEXTURE_2D, widget->texture);
            if (widget->image->channel == 1) {
                //if (strcmp(typeid(T).name(), "unsigned short") == 0) {
                    //static_assert(gl_type<T>() == GL_UNSIGNED_SHORT || gl_type<T>() == GL_UNSIGNED_BYTE, "fail");
                
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, image_width, image_height, 0, GL_DEPTH_COMPONENT, gl_type<T>(), image_data);
                //} 
            }
            else if (widget->image->channel == 4) {
                //if (strcmp(typeid(T).name(), "unsigned char") == 0) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, gl_type<T>(), image_data);
                //}
            }
            else {
                // error handling
            }
        }
        else {
            // error handling
        }
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            // error handling
        }

        return true;
    }

    template <class T>
    void drop(image_view<T>* widget)
    {
        if (widget) {
            glDeleteTextures(1, &widget->texture);
            cl::recycle(widget->image);
        }
    }

    template <class T>
    void render(image_view<T>* widget)
    {
        assert(widget);
        ImGui::Image((void*)(intptr_t)widget->texture, ImVec2(widget->width, widget->height)/*, ImVec2(0, 0), ImVec2(0.5, 0.5)*/);
    }

    template <class T>
    image_view<T>* clone(image_view<T>* widget)
    {
        assert(widget);
        
        const auto image = widget->image;
        const auto width = widget->width;
        const auto height = widget->height;
        
        auto ptr = cl::alloc<image_view<T>>();
        if (!ptr) {
            return nullptr;
        }
        auto img = sil::clone(image);
        if (!ptr) {
            return nullptr;
        }
        if (!init(ptr, width, height, img)) {
            return nullptr;
        }
        return ptr;
    }
}

#endif // !_UI_IMAGE_H_
