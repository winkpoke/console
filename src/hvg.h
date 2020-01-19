#pragma once

#include <time.h>
#include "imgui.h"

//// logging
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_sinks.h"

namespace hvg {
    const int SUCCESS = 0;
    const int FAILURE = -1;
    enum error_t {
        ERR_NONE,
        ERR_STATUS,
        ERR_COM_OPEN,
        ERR_COM_CLOSE,
        ERR_COM_STATUS,
        ERR_COM_CONTEXT_NULL,
        ERR_CMD_STATUS_PARSE,
        ERR_CMD_TIMEOUT,
    };
    static error_t g_error = ERR_NONE;
    const char* error_description[] = {
        "no error",                       // ERR_NONE
        "HVG status is not ready",        // ERR_STATUS
        "cannot open COM port",           // ERR_COM_OPEN
        "cannot close COM port",          // ERR_COM_CLOSE
        "illegal COM status",             // ERR_COM_STATUS
        "COM context cannot be NULL",     // ERR_COM_CONTEXT_NULL
        "cannot parse status code",       // ERR_CMD_STATUS_PARSE
        "command execution timeout",      // ERR_CMD_TIMEOUT

    }; // error_description

    error_t last_error()
    {
        return g_error;
    }

    const char* error_str(error_t e)
    {
        return hvg::error_description[e];
    }

    const char* COMMMAND_STOPS = "\r\n";
    const size_t BUF_SIZE = 4096;

    struct context_t {
        enum status_t {
            PENDING,
            SENDING,
            SENT,
            RECIEVING,
            //RECIEVED,      // not used
            //TIMEOUT
        };
        int port;
        int baud;
        char mode[4];
        int pos;
        int len;
        char buf[BUF_SIZE];
        status_t status;
    };

    context_t* open(int port, int baud, const char* mode)
    {
        if (RS232_OpenComport(port, baud, mode, 0))
        {
            hvg::g_error = ERR_COM_OPEN;
            return NULL;
        }
        context_t* context = (context_t*)malloc(sizeof(context_t));
        context->port = port;
        context->baud = baud;
        context->pos = 0;
        context->len = 0;
        memset(context->buf, 0, BUF_SIZE);
        context->status = context_t::PENDING;
        strncpy(context->mode, mode, 4);
        return context;
    }

    int close(context_t* context)
    {
        if (context != NULL) {
            free(context);
            return hvg::SUCCESS;
        }
        else {
            hvg::g_error = ERR_COM_CONTEXT_NULL;
            return hvg::FAILURE;
        }
    }

    int send(context_t* context, const char* command, char* msg, int* condition, int timeout = 100 /* ms */)
    {
        if (context == NULL) {
            hvg::g_error = ERR_COM_CONTEXT_NULL;
            return hvg::FAILURE;
        }

        // TODO: check command is valid

        const char* stops = hvg::COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { // && status != context_t::TIMEOUT) {
            hvg::g_error = ERR_COM_STATUS;
            return hvg::FAILURE;
        }

        // always flush RX/TX buffer before sending command
        RS232_flushRXTX(port);

        // Send command to HVG
        sprintf(send_buf, "%s%s", command, stops);
        status = context_t::SENDING;
        RS232_cputs(port, send_buf);
        status = context_t::SENT;

        if (msg == NULL) {
            // in case of no return data
            status = context_t::PENDING;
            return 0;
        }

        unsigned char* buf = (unsigned char*)context->buf;
        int& n = context->len;
        int& pos = context->pos;
        time_t start = time(NULL);
        time_t now = time(NULL);

        status = context_t::RECIEVING;
        n = RS232_PollComport(port, buf, BUF_SIZE);
        pos = 0;

        char* d = msg;
        while (difftime(now, start) < timeout / 1000.0) {
            if (n > 0) {
                buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)buf + pos;
                while (n > 0) {
                    //SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {2.2X}, d = {0.2X} \n", pos, n, *s, *d);
                    SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {}", pos, n, *s);
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        const int len = d - msg - 1;
                        if (strncmp((char*)msg, ">RDY", 4) != 0) {
                            hvg::g_error = ERR_CMD_STATUS_PARSE;
                            status = context_t::PENDING;
                            return len;
                        }

                        // in case the HVG status is changed 
                        int cond0 = 0;
                        int cond1 = 0;
                        int rtv = sscanf((char*)msg, ">RDY %x %x", &cond0, &cond1);
                        if (rtv != 2) {
                            status = context_t::PENDING;
                            return FAILURE;
                        }
                        *condition = (cond0 << 16) | cond1;
                        s++, pos++, n--;    // skip \n
                        d = msg;
                    }
                    *d++ = *s++;
                    pos++, n--;
                }
            }
            n = RS232_PollComport(port, buf, BUF_SIZE);
            pos = 0;
            now = time(NULL);
            // spdlog::debug("difftime = {:f}", difftime(now, start));
        }
        status = context_t::PENDING;
        hvg::g_error = ERR_CMD_TIMEOUT;
        return FAILURE;
    }

    enum callback_return_t {
        CONTINUE,
        BREAK,
    };

    typedef int (*HVG_CALLBACK_SEND)(const char* msg, int n);
    
    int send(context_t* context, const char* command, HVG_CALLBACK_SEND callback)
    {
        if (context == NULL) {
            hvg::g_error = ERR_COM_CONTEXT_NULL;
            return hvg::FAILURE;
        }

        // TODO: check command is valid

        const char* stops = hvg::COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { // && status != context_t::TIMEOUT) {
            hvg::g_error = ERR_COM_STATUS;
            return hvg::FAILURE;
        }

        // always flush RX/TX buffer before sending command
        RS232_flushRXTX(port);

        // Send command to HVG
        sprintf(send_buf, "%s%s", command, stops);
        status = context_t::SENDING;
        RS232_cputs(port, send_buf);
        status = context_t::SENT;

        unsigned char* buf = (unsigned char*)context->buf;
        int& n = context->len;
        int& pos = context->pos;
        time_t start = time(NULL);
        time_t now = time(NULL);

        status = context_t::RECIEVING;
        n = RS232_PollComport(port, buf, BUF_SIZE);
        pos = 0;

        char msg[1024];
        char* d = msg;
        while (1) {
            if (n > 0) {
                buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)buf + pos;
                while (n > 0) {
                    //SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {2.2X}, d = {0.2X} \n", pos, n, *s, *d);
                    SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {}", pos, n, *s);
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        const int len = d - msg - 1;
                        int e = callback(msg, len);
                        if (e == BREAK) {
                            status = context_t::PENDING;
                            return 0;
                        }
                        s++, pos++, n--;    // skip \n
                        d = msg;
                    }
                    *d++ = *s++;
                    pos++, n--;
                }
            }
            n = RS232_PollComport(port, buf, BUF_SIZE);
            pos = 0;
        }
    }

    int send(int port, const char* command, char* str, int timeout = 100 /* ms */)
    {
        const char* stops = hvg::COMMMAND_STOPS;
        static char send_buf[4096];

        sprintf(send_buf, "%s%s", command, stops);
        RS232_cputs(port, send_buf);

        unsigned char r_buf[4096];
        time_t start = time(NULL);
        time_t now = time(NULL);
        printf("difftime = %f", difftime(now, start));
        while (difftime(now, start) < timeout / 1000.0) {
            int n = RS232_PollComport(port, r_buf, 4095);
            if (n > 0) {
                r_buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)r_buf;
                char* d = str;
                while (n-- > 0) {
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        return d - str - 1;
                    }
                    else {
                        *d++ = *s++;
                    }
                }
            }
            now = time(NULL);
        }
        return 0;
    }

    int init_rs232(int port, int baud)
    {
        //unsigned char buf[4096];
        char mode[] = { '8','N','2',0 };

        if (RS232_OpenComport(port, baud, mode, 0))
        {
            hvg::g_error = ERR_COM_OPEN;
            return hvg::FAILURE;
        }
        return port;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll;     // Keep scrolling if already at the bottom

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
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
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
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
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};
