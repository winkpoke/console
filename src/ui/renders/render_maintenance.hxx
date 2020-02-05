#pragma once

#include "imgui.h"
#include "rs232.h"

#include "ui/log.h"

namespace ui {
    struct app_t;
    bool render_maintenance_window(app_t* app)
    {
        static bool open = true;
        static bool* p_open = &open;

        static auto log = cl::build_unique<log_t>(drop);

        static int port = 2;
        static unsigned char sbuf[4096];
        int n = RS232_PollComport(port, sbuf, 4095);
        static char str[4096];
        if (n > 0) {
            sbuf[n] = 0;   /* always put a "null" at the end of a string! */
            char* s = (char*)sbuf;
            static char* d = str;
            while (n-- > 0) {
                if (*s == '\n' && *(d - 1) == '\r') {
                    *(d - 1) = 0;
                    ui::add_log(log.get(), "received %i bytes: %s\n", d - str - 1, str);
                    d = str;
                    s++;
                }
                else {
                    *d++ = *s++;
                }
            }
        }

        // For the demo: add a debug button _BEFORE_ the normal log window contents
        // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
        // Most of the contents of the window will be added by the log.Draw() call.
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        static char hvg_command_window_title[256] = "HVG Command";
        ImGui::Begin(hvg_command_window_title, p_open);
        const char* items[] = { "ES3", "IFV" };
        static const char* item_current = items[0];
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::Combo("Predefined commands", &item_current_3, items, IM_ARRAYSIZE(items));

        static char buf[256] = "";
        ImGui::InputText("HVG Command", buf, 256);

        if (ImGui::Button("Send")) {
            ui::add_log(log.get(), "HVG Command: %s\n", buf);
            static char send_buf[4096];
            sprintf(send_buf, "%s\r\n", buf);
            RS232_cputs(port, send_buf);
        }

        ImGui::End();

        // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
        ui::draw(log.get(), hvg_command_window_title, p_open);
        return true;
    }
}