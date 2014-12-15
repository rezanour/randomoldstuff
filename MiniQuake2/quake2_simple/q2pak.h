#pragma once

void
PakInit(
    PCWSTR filename
    );

void
PakShutdown();

void*
PakLoadFile(
    PCSTR filename
    );

uint32_t
PakFileSize(
    PCSTR filename
    );
