#include <memory>
#include <GLFW/glfw3.h>

#include "control/sil.h"


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
    struct image_view {
        std::shared_ptr<sil::image_t<T>> image;
        GLuint texture;
        size_t width;
        size_t height;


    };

    template <class T>
    static bool init(image_view<T>* widget, size_t width, size_t height, std::shared_ptr<sil::image_t<T>> image)
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

        return true;
    }

    template <class T>
    static void drop(image_view<T>* widget) {}

    template <class T>
    void render(image_view<T>* widget)
    {
        if (widget->image.get() != NULL) {
            glBindTexture(GL_TEXTURE_2D, widget->texture);
            if (widget->image->channel == 1) {
                if (strcmp(typeid(T).name(), "unsigned short") == 0) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widget->image->width, widget->image->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, widget->image->data);
                }
            } else if (widget->image->channel == 4) {
                if (strcmp(typeid(T).name(), "unsigned char") == 0) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widget->image->width, widget->image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, widget->image->data);
                }
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
        ImGui::Image((void*)(intptr_t)widget->texture, ImVec2(widget->width, widget->height)/*, ImVec2(0, 0), ImVec2(0.5, 0.5)*/);
    }

}}  // namespace ui::image