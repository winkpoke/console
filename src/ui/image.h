#include <memory>
#include <GLFW/glfw3.h>

#include "sil.h"


namespace ui { namespace image {
    //template <class T>
    //struct mono_image_window {
    //    std::shared_ptr<sil::image_t<T>> image;
    //    GLuint texture;
    //};

    //template <class T>
    //struct rgba_image_window {
    //    std::shared_ptr<sil::image_t<T>> image;
    //    GLuint texture;
    //};

    template <class T>
    struct mono_image_window {
        std::shared_ptr<sil::image_t<T>> image;
        GLuint texture;
        //int x;
        //int y;
        size_t width;
        size_t height;
    };

    template <class T>
    bool init(mono_image_window<T>* widget, size_t width, size_t height, std::shared_ptr<sil::image_t<T>> image)
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

        //sprintf(widget->title, "##Image_%d", rand());
        return true;
    }

    template <class T>
    void render(mono_image_window<T>* widget)
    {
        //static bool p_open;
        //ImGui::Begin("##Image", &p_open, ImGuiWindowFlags_NoTitleBar);

        if (widget->image.get() != NULL) {
            glBindTexture(GL_TEXTURE_2D, widget->texture);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widget->image->width, widget->image->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, widget->image);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widget->image->width, widget->image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, widget->image->data);
        }
        else {
            // error handling
        }
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            // error handling
        }
        ImGui::Image((void*)(intptr_t)widget->texture, ImVec2(widget->width, widget->height), ImVec2(0, 0), ImVec2(0.5, 0.5));
        //ImGui::End();
    }

}}  // namespace ui::image