#pragma once

// seems stb need to be placed before other includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// logging
//#define SPDLOG_NO_FILE_LOC
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#define WINDOW_IMPLEMENTATION
#include "ui/window.h"

#define CONSOLE_UI_APP_IMPLEMENTATION
#include "ui/app.hxx"

#define CONSOLE_UI_IMPLEMENTATION
#include "ui/ui.h"

#define CONSOLE_MODAL_IMPLEMENTATION
#include "modal/modal.h"

#define CONSOLE_CONTROL_IMPLEMENTATION
#include "control/control.h"

#define MODAL_SCAN_IMPLEMENTATION
#include "modal/scan.hxx"

#define MODAL_PATIENT_IMPLEMENTATION
#include "modal/patient.hxx"

#define FLAT_PANEL_IMPLEMENTATION
#include "common/flat_panel.hxx"

#define CONTROL_FPD_IMPLEMENTATION
#include "control/fpd.hxx"

#define BASE64_IMPLEMENTATION
#include "base64.hxx"

#define UI_LOG_IMPLEMENTATION
#include "ui/log.h"

#define WEBSOCKET_IMPLEMENTATION
#include "control/websocket.hxx"

#define MODAL_RUNTIME_DATA_IMPLEMENTATION
#include "control/runtime_data.hxx"
