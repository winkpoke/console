#pragma once

// seems stb need to be placed before other includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// logging
//#define SPDLOG_NO_FILE_LOC
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#define BASE64_IMPLEMENTATION
#include "base64.hxx"

#define CONSOLE_MODAL_IMPLEMENTATION
#include "modal/modal.h"

#define MODAL_SCAN_IMPLEMENTATION
#include "modal/scan.hxx"

#define WEBSOCKET_IMPLEMENTATION
#include "control/websocket.hxx"

#define MODAL_RUNTIME_DATA_IMPLEMENTATION
#include "control/runtime_data.hxx"

#define CONSOLE_CONTROL_IMPLEMENTATION
#include "control/control.hxx"

#define WINDOW_IMPLEMENTATION
#include "ui/window.h"

#define CONSOLE_UI_APP_IMPLEMENTATION
#include "ui/app.hxx"

#define CONSOLE_UI_IMPLEMENTATION
#include "ui/ui.h"

#define UI_LOG_IMPLEMENTATION
#include "ui/log.h"

#define HVG_SIEMENS_IMPLEMENTATION
#include "control/hvg/siemens.hxx"

#define CONTROL_HVG_IMPLEMENTATION
#include "control/hvg/hvg.hxx"
