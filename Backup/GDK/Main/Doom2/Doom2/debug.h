#ifndef _DEBUG_H_
#define _DEBUG_H_

// Prints (to debug out) any allocations still tracked by the memory manager.
// Since this method should only be called at the end of the game, there should
// not be any allocations left, unless we leaked. Therefore, if any are found,
// DebugMemoryReport will break into the debugger (or assert if no debugger present).
void DebugMemoryReport();
void DebugOut(const char* format, ...);
void DebugOutLn(const char* format, ...);
void DebugWadDumpLumps(BOOL headerLumpsOnly);
void DebugWadSaveLumpToFile(const char* name, const char* filePath);

void DebugGraphicsDrawPalette(byte_t paletteIndex, int x, int y, int size);
void DebugGraphicsSetColormap(byte_t index);
void DebugGraphicsTestLines();

void DebugAudioPlaySound(const char* name);
void DebugAudioPlayMusic(const char* name);
void DebugAudioPlayMidiFile(const char* filePath, BOOL useMidiStream);

byte_t DebugAudioGetNextMusicId(byte_t id);
void DebugAudioPlayMusicByIndex(byte_t id);



void DebugMapDraw(short xOffset, short yOffset, float scale);

#endif // _DEBUG_H_