#include "config.hpp"

ACKCONFIG engine_config =
{
    argv0:        NULL,
    organization: "teamretro",
    application:  "acknext",
    windowTitle:  "Acknext Game Engine",
    resolution:   { 1280, 720 },
    vsync:        VSYNC_ADAPTIVE,
    flags:        USE_VFS | VFS_USE_CWD,
};
