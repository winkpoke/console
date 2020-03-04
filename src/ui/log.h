#ifndef _UI_LOG_H_
#define _UI_LOG_H_

#include "cl.h"

#include "imgui.h"

namespace ui {
    struct log_t {
        ImGuiTextBuffer     buf;
        ImGuiTextFilter     filter;
        ImVector<int>       line_offsets;
        bool                auto_scroll;
    };

    bool init(log_t* log);
    void drop(log_t* log);

    void clear(log_t* log);
    void add_log(log_t* log, const char* fmt, ...) IM_FMTARGS(2);
    void draw(log_t* log, const char* title, bool* p_open = NULL);
}
#endif // !_UI_LOG_H_

#ifdef UI_LOG_IMPLEMENTATION
#ifndef UI_LOG_IMPLEMENTED
#define UI_LOG_IMPLEMENTED

namespace ui {
    void clear(log_t* log)
    {
        log->buf.clear();
        log->line_offsets.clear();
        log->line_offsets.push_back(0);
    }

    bool init(log_t* log)
    {
        new(&log->buf)ImGuiTextBuffer;
        new(&log->filter)ImGuiTextFilter;
        new(&log->line_offsets)ImVector<int>;

        log->auto_scroll = true;
        clear(log);
        return true;
    }

    void drop(log_t* log)
    {
        cl::dealloc(log);
    }

    void add_log(log_t* log, const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = log->buf.size();
        va_list args;
        va_start(args, fmt);
        log->buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = log->buf.size(); old_size < new_size; old_size++)
            if (log->buf[old_size] == '\n')
                log->line_offsets.push_back(old_size + 1);
    }

    void draw(log_t* log, const char* title, bool* p_open)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &log->auto_scroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        log->filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            ui::clear(log);
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = log->buf.begin();
        const char* buf_end = log->buf.end();
        if (log->filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < log->line_offsets.Size; line_no++)
            {
                const char* line_start = buf + log->line_offsets[line_no];
                const char* line_end = (line_no + 1 < log->line_offsets.Size) ? (buf + log->line_offsets[line_no + 1] - 1) : buf_end;
                if (log->filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
            // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
            // Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
            // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
            ImGuiListClipper clipper;
            clipper.Begin(log->line_offsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + log->line_offsets[line_no];
                    const char* line_end = (line_no + 1 < log->line_offsets.Size) ? (buf + log->line_offsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (log->auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
}

#endif // !UI_LOG_IMPLEMENTED
#endif // UI_LOG_IMPLEMENTATION