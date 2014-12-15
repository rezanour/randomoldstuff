#pragma once

// Ensures file is loaded and cached
void
WalLoadImage(
    _In_ PCSTR filename,
    _Out_opt_ uint32_t* width,
    _Out_opt_ uint32_t* height
    );

// Copy pixels out in 32bpp format
void
WalCopyPixels(
    _In_ PCSTR filename,
    _In_ uint8_t* buffer
    );

// Dumps any cached images
void
WalClearAllImages();
