#include "config.hpp"

ACKCONFIG engine_config =
{
    argv0:        NULL,
    organization: "teamretro",
    application:  "acknext",
    windowTitle:  "Unnamed Game",
    resolution:   { 1280, 720 },
    flags:        USE_VFS | VFS_USE_CWD,
};
